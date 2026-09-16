#include "search.h"

#include <algorithm>
#include <cassert>
#include <cstdint>

#include "../evaluation/evaluation.h"
#include "../movegen/movegen.h"

namespace Aribella {

namespace {

inline constexpr int Infinity = 1'000'000;


// Temporary statistics.
// Later we can move these into a Searcher class.
std::uint64_t Nodes = 0;
std::uint64_t TTHits = 0;


// ---------------------------------------------------------
// Internal Negamax + Alpha-Beta + TT
// ---------------------------------------------------------

[[nodiscard]]
int negamax_impl(
    GameState& game,
    TranspositionTable& tt,
    int depth,
    int ply,
    int alpha,
    int beta
)
{
    ++Nodes;

    assert(depth >= 0);
    assert(ply >= 0);
    assert(alpha < beta);

    const Position& position =
        game.position();


    // ---------------------------------------------------------
    // Threefold repetition
    //
    // Must happen before TT because repetition depends
    // on history, not just the current Zobrist key.
    // ---------------------------------------------------------

    if (game.is_threefold_repetition())
    {
        return 0;
    }


    // ---------------------------------------------------------
    // Fifty-move rule
    //
    // Our Zobrist key does not include halfmove_clock.
    // ---------------------------------------------------------

    if (position.halfmove_clock() >= 100)
    {
        return 0;
    }


    // ---------------------------------------------------------
    // Current position key
    // ---------------------------------------------------------

    const Zobrist::Key key =
        position.key();


    // ---------------------------------------------------------
    // Probe transposition table
    // ---------------------------------------------------------

    const TTEntry* entry =
        tt.probe(key);

    if (entry != nullptr &&
        entry->depth >= depth)
    {
        switch (entry->flag)
        {
            // -------------------------------------------------
            // Exact score:
            //
            // We already know the answer.
            // -------------------------------------------------

            case TTFlag::Exact:
            {
                ++TTHits;

                return entry->score;
            }


            // -------------------------------------------------
            // Lower bound:
            //
            // score >= stored score
            //
            // Therefore alpha can potentially be raised.
            // -------------------------------------------------

            case TTFlag::LowerBound:
            {
                alpha =
                    std::max(
                        alpha,
                        entry->score
                    );

                break;
            }


            // -------------------------------------------------
            // Upper bound:
            //
            // score <= stored score
            //
            // Therefore beta can potentially be lowered.
            // -------------------------------------------------

            case TTFlag::UpperBound:
            {
                beta =
                    std::min(
                        beta,
                        entry->score
                    );

                break;
            }
        }


        // -----------------------------------------------------
        // TT bound itself caused a cutoff.
        // -----------------------------------------------------

        if (alpha >= beta)
        {
            ++TTHits;

            return entry->score;
        }
    }


    // ---------------------------------------------------------
    // Save the actual window used for this search.
    //
    // This happens AFTER TT bounds may have tightened it.
    // ---------------------------------------------------------

    const int search_alpha =
        alpha;

    const int search_beta =
        beta;


    // ---------------------------------------------------------
    // Leaf node
    // ---------------------------------------------------------

    if (depth == 0)
    {
        const int score =
            evaluate(position);

        return position.side_to_move() == Color::White
            ? score
            : -score;
    }


    // ---------------------------------------------------------
    // Generate legal moves
    // ---------------------------------------------------------

    Position search_position =
        position;

    const MoveList moves =
        generate_legal_moves(
            search_position,
            position.side_to_move()
        );


    // ---------------------------------------------------------
    // Checkmate / stalemate
    // ---------------------------------------------------------

    if (moves.empty())
    {
        if (in_check(
                search_position,
                position.side_to_move()))
        {
            return -MateScore + ply;
        }

        return 0;
    }


    // ---------------------------------------------------------
    // Search variables
    // ---------------------------------------------------------

    int best_score =
        -Infinity;

    Move best_move{};


    // ---------------------------------------------------------
    // Search legal moves
    // ---------------------------------------------------------

    for (std::size_t i = 0;
         i < moves.size();
         ++i)
    {
        const Move move =
            moves[i];


        // -----------------------------------------------------
        // Make move
        // -----------------------------------------------------

        const UndoState undo =
            game.make_move(move);


        // -----------------------------------------------------
        // Negamax recursive search
        //
        // Current:
        //
        //     [alpha, beta]
        //
        // Child:
        //
        //     [-beta, -alpha]
        // -----------------------------------------------------

        const int score =
            -negamax_impl(
                game,
                tt,
                depth - 1,
                ply + 1,
                -beta,
                -alpha
            );


        // -----------------------------------------------------
        // Restore position
        // -----------------------------------------------------

        game.unmake_move(
            move,
            undo
        );


        // -----------------------------------------------------
        // New best move?
        // -----------------------------------------------------

        if (score > best_score)
        {
            best_score =
                score;

            best_move =
                move;
        }


        // -----------------------------------------------------
        // Raise alpha
        // -----------------------------------------------------

        if (score > alpha)
        {
            alpha =
                score;
        }


        // -----------------------------------------------------
        // Beta cutoff
        // -----------------------------------------------------

        if (alpha >= beta)
        {
            break;
        }
    }


    // ---------------------------------------------------------
    // Determine what kind of TT result we have.
    // ---------------------------------------------------------

    TTFlag flag;


    // ---------------------------------------------------------
    // Fail-low
    //
    // The node never improved the alpha value that existed
    // when we started searching it.
    //
    // Therefore:
    //
    // actual score <= best_score
    //
    // Store as an upper bound.
    // ---------------------------------------------------------

    if (best_score <= search_alpha)
    {
        flag =
            TTFlag::UpperBound;
    }


    // ---------------------------------------------------------
    // Fail-high
    //
    // We reached/exceeded beta and may have stopped searching.
    //
    // Therefore:
    //
    // actual score >= best_score
    //
    // Store as a lower bound.
    // ---------------------------------------------------------

    else if (best_score >= search_beta)
    {
        flag =
            TTFlag::LowerBound;
    }


    // ---------------------------------------------------------
    // Score fell inside the window.
    //
    // We fully determined the node's score.
    // ---------------------------------------------------------

    else
    {
        flag =
            TTFlag::Exact;
    }


    // ---------------------------------------------------------
    // Store the result
    // ---------------------------------------------------------

    tt.store(
        key,
        depth,
        best_score,
        flag,
        best_move
    );


    return best_score;
}

} // anonymous namespace


// ---------------------------------------------------------
// Root search
// ---------------------------------------------------------

SearchResult search(
    GameState& game,
    TranspositionTable& tt,
    int depth
)
{
    assert(depth > 0);

    const Position& position =
        game.position();


    // ---------------------------------------------------------
    // Threefold repetition
    // ---------------------------------------------------------

    if (game.is_threefold_repetition())
    {
        return SearchResult{
            .best_move = Move{},
            .score = 0
        };
    }


    // ---------------------------------------------------------
    // Fifty-move rule
    // ---------------------------------------------------------

    if (position.halfmove_clock() >= 100)
    {
        return SearchResult{
            .best_move = Move{},
            .score = 0
        };
    }


    // ---------------------------------------------------------
    // Generate legal root moves
    // ---------------------------------------------------------

    Position search_position =
        position;

    const MoveList moves =
        generate_legal_moves(
            search_position,
            position.side_to_move()
        );


    // ---------------------------------------------------------
    // Checkmate / stalemate at root
    // ---------------------------------------------------------

    if (moves.empty())
    {
        if (in_check(
                search_position,
                position.side_to_move()))
        {
            return SearchResult{
                .best_move = Move{},
                .score = -MateScore
            };
        }

        return SearchResult{
            .best_move = Move{},
            .score = 0
        };
    }


    // ---------------------------------------------------------
    // Root alpha-beta values
    // ---------------------------------------------------------

    int alpha =
        -Infinity;

    constexpr int beta =
        Infinity;

    int best_score =
        -Infinity;

    Move best_move{};


    // ---------------------------------------------------------
    // Search every root move
    // ---------------------------------------------------------

    for (std::size_t i = 0;
         i < moves.size();
         ++i)
    {
        const Move move =
            moves[i];


        // -----------------------------------------------------
        // Make root move
        // -----------------------------------------------------

        const UndoState undo =
            game.make_move(move);


        // -----------------------------------------------------
        // Search child
        // -----------------------------------------------------

        const int score =
            -negamax_impl(
                game,
                tt,
                depth - 1,
                1,
                -beta,
                -alpha
            );


        // -----------------------------------------------------
        // Restore root
        // -----------------------------------------------------

        game.unmake_move(
            move,
            undo
        );


        // -----------------------------------------------------
        // New best root move?
        // -----------------------------------------------------

        if (score > best_score)
        {
            best_score =
                score;

            best_move =
                move;
        }


        // -----------------------------------------------------
        // Update root alpha
        // -----------------------------------------------------

        if (score > alpha)
        {
            alpha =
                score;
        }
    }


    return SearchResult{
        .best_move = best_move,
        .score = best_score
    };
}


// ---------------------------------------------------------
// Search statistics
// ---------------------------------------------------------

SearchStats search_stats() noexcept
{
    return SearchStats{
        .nodes = Nodes,
        .tt_hits = TTHits
    };
}


void reset_search_stats() noexcept
{
    Nodes = 0;
    TTHits = 0;
}

} // namespace Aribella