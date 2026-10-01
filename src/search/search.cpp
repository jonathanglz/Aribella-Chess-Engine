#include "search.h"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <utility>
#include <array>

#include "../evaluation/evaluation.h"
#include "../movegen/movegen.h"
#include "../movegen/move_format.h"

namespace Aribella {
    namespace {
        inline constexpr int Infinity = 1'000'000;

        std::uint64_t Nodes = 0;
        std::uint64_t TTHits = 0;
        std::uint64_t QNodes = 0;

        inline constexpr int MaxSearchPly = 128;

        inline constexpr int MateThreshold = MateScore - MaxSearchPly;

        std::array<std::array<Move, MaxSearchPly>, MaxSearchPly> PvTable{};

        std::array<int, MaxSearchPly> PvLength{};

        [[nodiscard]]
        constexpr int score_to_tt(int score, int ply) noexcept {
            assert(ply >= 0);

            if (score >= MateThreshold) {
                return score + ply;
            }

            if (score <= -MateThreshold) {
                return score - ply;
            }

            return score;
        }

        [[nodiscard]]
        constexpr int score_from_tt(int score, int ply) noexcept {
            assert(ply >= 0);

            if (score >= MateThreshold) {
                return score - ply;
            }

            if (score <= -MateThreshold) {
                return score + ply;
            }

            return score;
        }

        [[nodiscard]]
        constexpr int ordering_piece_value(PieceType type) noexcept {
            switch (type) {
                case PieceType::Pawn:
                    return 100;

                case PieceType::Knight:
                    return 320;

                case PieceType::Bishop:
                    return 330;

                case PieceType::Rook:
                    return 500;

                case PieceType::Queen:
                    return 900;

                case PieceType::King:
                    return 20'000;

                default:
                    return 0;
            }
        }


        [[nodiscard]]
        int move_ordering_score(
            const Position &position,
            const Move &move,
            const Move *tt_move
        ) noexcept {
            // ---------------------------------------------------------
            // TT move
            // ---------------------------------------------------------

            if (tt_move != nullptr &&
                move == *tt_move) {
                return
                        Infinity;
            }

            // ---------------------------------------------------------
            // Promotions
            // ---------------------------------------------------------
            if (is_promotion(move)) {
                int score =
                        700'000 + ordering_piece_value(move.promotion);

                if (move.flag == MoveFlag::PromotionCapture) {
                    score +=
                            25'000;
                }

                return score;
            }


            // ---------------------------------------------------------
            // Captures --- Most Valuable Victim - Least Valuable Attacker
            // ---------------------------------------------------------

            if (is_captured(move)) {
                const Piece attacker =
                        position.piece_at(move.from);

                assert(is_valid(attacker));

                const PieceType attacker_type
                        = type_of(attacker);

                PieceType victim_type;

                if (move.flag == MoveFlag::EnPassant) {
                    victim_type =
                            PieceType::Pawn;
                } else {
                    const Piece victim =
                            position.piece_at(move.to);

                    assert(is_valid(victim));

                    victim_type =
                            type_of(victim);
                }

                return 100'000
                       + ordering_piece_value(victim_type) * 10
                       - ordering_piece_value(attacker_type);
            }

            return 0;
        }


        void order_moves(
            const Position &position,
            MoveList &moves,
            const Move *tt_move
        ) noexcept {
            for (std::size_t i = 0; i < moves.size(); ++i) {
                std::size_t best_index = i;

                int best_score =
                        move_ordering_score(
                            position,
                            moves[i],
                            tt_move
                        );

                for (std::size_t j = i + 1; j < moves.size(); ++j) {
                    const int score =
                            move_ordering_score(
                                position,
                                moves[j],
                                tt_move
                            );

                    if (score > best_score) {
                        best_score = score;

                        best_index = j;
                    }
                }

                if (best_index != i) {
                    std::swap(moves[i], moves[best_index]);
                }
            }
        }

        [[nodiscard]]
        int quiescence(
            GameState &game,
            int ply,
            int alpha,
            int beta,
            bool use_move_ordering,
            SearchControl &control
        ) {
            if (control.should_stop()) {
                return 0;
            }

            ++QNodes;
            ++Nodes;
            assert(ply >= 0);

            if (alpha >= beta) {
                std::cerr
                        << "INVALID QUIESCENCE WINDOW"
                        << " ply=" << ply
                        << " alpha=" << alpha
                        << " beta=" << beta
                        << '\n';
            }

            assert(alpha < beta);

            const Position &position = game.position();
            const Color side = position.side_to_move();

            if (game.is_threefold_repetition()) {
                return 0;
            }

            if (position.halfmove_clock() >= 100) {
                return 0;
            }

            const bool checked =
                    in_check(position, side);

            Position search_position = position;

            MoveList moves = generate_legal_moves(search_position, side);

            if (moves.empty()) {
                if (checked) {
                    return -MateScore + ply;
                }
                return 0;
            }

            if (ply >= MaxSearchPly && !checked) {
                const int evaluation =
                        evaluate(position);

                return
                        side == Color::White
                            ? evaluation
                            : -evaluation;
            }

            if (!checked) {
                const int evaluation =
                        evaluate(position);

                const int stand_pat =
                        side == Color::White
                            ? evaluation
                            : -evaluation;

                if (stand_pat >= beta) {
                    return stand_pat;
                }

                if (stand_pat > alpha) {
                    alpha = stand_pat;
                }
            }

            if (use_move_ordering) {
                order_moves(position, moves, nullptr);
            }

            int best_score =
                    checked
                        ? -Infinity
                        : alpha;

            for (std::size_t i = 0; i < moves.size(); ++i) {
                const Move move = moves[i];

                if (!checked &&
                    !is_captured(move) &&
                    !is_promotion(move)
                ) {
                    continue;
                }

                const UndoState undo =
                        game.make_move(move);

                const int score =
                        -quiescence(game,
                                    ply + 1,
                                    -beta,
                                    -alpha,
                                    use_move_ordering,
                                    control
                        );

                game.unmake_move(move, undo);

                if (control.should_stop()) {
                    return 0;
                }

                if (score > best_score) {
                    best_score = score;
                }

                if (score > alpha) {
                    alpha = score;
                }

                if (alpha >= beta) {
                    break;
                }
            }

            return best_score;
        }

        // ---------------------------------------------------------
        // Internal Negamax + Alpha-Beta + TT
        // ---------------------------------------------------------

        [[nodiscard]]
        int negamax_impl(
            GameState &game,
            TranspositionTable &tt,
            int depth,
            int ply,
            int alpha,
            int beta,
            bool use_move_ordering,
            SearchControl &control
        ) {
            if (control.should_stop()) {
                return 0;
            }

            ++Nodes;
            assert(depth >= 0);
            assert(ply >= 0);

            if (ply < MaxSearchPly) {
                PvLength[ply] = 0;
            }

            if (alpha >= beta) {
                std::cerr
                        << "INVALID NEGAMAX WINDOW"
                        << " depth=" << depth
                        << " ply=" << ply
                        << " alpha=" << alpha
                        << " beta=" << beta
                        << '\n';
            }

            assert(alpha < beta);

            const Position &position =
                    game.position();


            // ---------------------------------------------------------
            // Threefold repetition
            // ---------------------------------------------------------

            if (game.is_threefold_repetition()) {
                return 0;
            }


            // ---------------------------------------------------------
            // Fifty-move rule
            // ---------------------------------------------------------

            if (position.halfmove_clock() >= 100) {
                return 0;
            }


            // ---------------------------------------------------------
            // Leaf node
            // ---------------------------------------------------------

            if (depth == 0) {
                return quiescence(game,
                                  ply,
                                  alpha,
                                  beta,
                                  use_move_ordering,
                                  control
                );
            }


            // ---------------------------------------------------------
            // Current position key
            // ---------------------------------------------------------

            const Zobrist::Key key =
                    position.key();


            // ---------------------------------------------------------
            // Probe transposition table
            // ---------------------------------------------------------

            const TTEntry *entry =
                    tt.probe(key);

            if (entry != nullptr && entry->depth >= depth) {
                const int tt_score = score_from_tt(entry->score, ply);

                if (entry->flag == TTFlag::Exact) {
                    ++TTHits;
                    return tt_score;
                }

                if (entry->flag == TTFlag::LowerBound) {
                    if (tt_score >= beta) {
                        ++TTHits;
                        return tt_score;
                    }

                    alpha = std::max(
                        alpha,
                        tt_score
                    );
                } else if (entry->flag == TTFlag::UpperBound) {
                    if (tt_score <= alpha) {
                        ++TTHits;
                        return tt_score;
                    }

                    beta = std::min(
                        beta,
                        tt_score
                    );
                }

                // A non-cutoff TT bound must leave a valid window.
                assert(alpha < beta);
            }


            // ---------------------------------------------------------
            // Save the actual window used for this search.
            // ---------------------------------------------------------

            const int search_alpha =
                    alpha;

            const int search_beta =
                    beta;


            // ---------------------------------------------------------
            // Generate legal moves
            // ---------------------------------------------------------

            Position search_position = position;

            MoveList moves =
                    generate_legal_moves(search_position,
                                         position.side_to_move());


            // ---------------------------------------------------------
            // Checkmate / stalemate
            // ---------------------------------------------------------

            if (moves.empty()) {
                if (in_check(search_position,
                             position.side_to_move())) {
                    return -MateScore + ply;
                }
                // stalemate is draw
                return 0;
            }


            // ---------------------------------------------------------
            // TT best move
            // ---------------------------------------------------------

            if (use_move_ordering) {
                const Move *tt_move =
                        entry != nullptr
                            ? &entry->best_move
                            : nullptr;

                order_moves(position, moves, tt_move);
            }


            // ---------------------------------------------------------
            // Move ordering
            //
            // Current priority:
            //
            // 1. TT move
            // 2. Promotions
            // 3. Captures (MVV-LVA)
            // 4. Quiet moves
            // ---------------------------------------------------------

            // ---------------------------------------------------------
            // Search variables
            // ---------------------------------------------------------

            int best_score = -Infinity;

            Move best_move{};


            // ---------------------------------------------------------
            // Search legal moves
            // ---------------------------------------------------------

            for (std::size_t i = 0; i < moves.size(); ++i) {
                const Move move = moves[i];

                // -----------------------------------------------------
                // Make move
                // -----------------------------------------------------

                const UndoState undo = game.make_move(move);


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
                        -negamax_impl(game,
                                      tt,
                                      depth - 1,
                                      ply + 1,
                                      -beta,
                                      -alpha,
                                      use_move_ordering,
                                      control
                        );


                // -----------------------------------------------------
                // Restore position
                // -----------------------------------------------------

                game.unmake_move(move, undo);


                if (control.should_stop()) {
                    return 0;
                }

                // -----------------------------------------------------
                // New best move?
                // -----------------------------------------------------

                if (score > best_score) {
                    best_score = score;
                    best_move = move;

                    if (ply < MaxSearchPly) {
                        PvTable[ply][0] = move;

                        int length = 1;

                        if (ply + 1 < MaxSearchPly) {
                            const int child_length = PvLength[ply + 1];

                            for (int j = 0; j < child_length && length < MaxSearchPly; ++j) {
                                PvTable[ply][length] = PvTable[ply + 1][j];

                                ++length;
                            }
                        }
                        PvLength[ply] = length;
                    }
                }


                // -----------------------------------------------------
                // Raise alpha
                // -----------------------------------------------------

                if (score > alpha) {
                    alpha = score;
                }


                // -----------------------------------------------------
                // Beta cutoff
                // -----------------------------------------------------

                if (alpha >= beta) {
                    break;
                }
            }


            // ---------------------------------------------------------
            // Determine what kind of TT result we have.
            // ---------------------------------------------------------

            TTFlag flag;


            // ---------------------------------------------------------
            // Store as an upper bound.
            // ---------------------------------------------------------

            if (best_score <= search_alpha) {
                flag =
                        TTFlag::UpperBound;
            }


            // ---------------------------------------------------------
            // Store as a lower bound.
            // ---------------------------------------------------------

            else if (best_score >= search_beta) {
                flag =
                        TTFlag::LowerBound;
            }


            // ---------------------------------------------------------
            // Score fell inside the window.
            // ---------------------------------------------------------

            else {
                flag =
                        TTFlag::Exact;
            }


            // ---------------------------------------------------------
            // Store the result
            // ---------------------------------------------------------

            tt.store(key,
                     depth,
                     score_to_tt(best_score, ply),
                     flag,
                     best_move);

            return best_score;
        }
    } // anonymous namespace


    // ---------------------------------------------------------
    // Root search
    // ---------------------------------------------------------

    SearchResult search(
        GameState &game,
        TranspositionTable &tt,
        int depth,
        bool use_move_ordering,
        SearchControl &control
    ) {
        if (control.should_stop()) {
            return SearchResult{};
        }

        assert(depth >= 1);

        PvLength.fill(0);

        const Position &position =
                game.position();


        // ---------------------------------------------------------
        // Threefold repetition
        // ---------------------------------------------------------

        if (game.is_threefold_repetition()) {
            return SearchResult{
                .best_move = Move{},
                .score = 0
            };
        }


        // ---------------------------------------------------------
        // Fifty-move rule
        // ---------------------------------------------------------

        if (position.halfmove_clock() >= 100) {
            return SearchResult{
                .best_move = Move{},
                .score = 0
            };
        }


        // ---------------------------------------------------------
        // Generate legal root moves
        // ---------------------------------------------------------

        Position search_position = position;

        MoveList moves =
                generate_legal_moves(search_position,
                                     position.side_to_move());


        // ---------------------------------------------------------
        // Checkmate / stalemate at root
        // ---------------------------------------------------------

        if (moves.empty()) {
            if (in_check(search_position,
                         position.side_to_move())) {
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

        if (use_move_ordering) {
            const TTEntry *entry = tt.probe(position.key());

            const Move *tt_move =
                    entry != nullptr
                        ? &entry->best_move
                        : nullptr;

            order_moves(position, moves, tt_move);
        }

        // ---------------------------------------------------------
        // Root alpha-beta values
        // ---------------------------------------------------------

        int alpha = -Infinity;

        constexpr int beta = Infinity;

        int best_score = -Infinity;

        Move best_move{};


        // ---------------------------------------------------------
        // Search every root move
        // ---------------------------------------------------------

        for (std::size_t i = 0; i < moves.size(); ++i) {
            const Move move = moves[i];


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
                        -alpha,
                        use_move_ordering,
                        control
                    );


            // -----------------------------------------------------
            // Restore root
            // -----------------------------------------------------

            game.unmake_move(move, undo);

            if (control.should_stop()) {
                return SearchResult{};
            }

            // -----------------------------------------------------
            // New best root move?
            // -----------------------------------------------------

            if (score > best_score) {
                best_score = score;

                best_move = move;

                PvTable[0][0] = move;

                int length = 1;

                const int child_length = PvLength[1];

                for (int j = 0; j < child_length && length < MaxSearchPly; ++j) {
                    PvTable[0][length] = PvTable[1][j];

                    ++length;
                }
                PvLength[0] = length;
            }


            // -----------------------------------------------------
            // Update root alpha
            // -----------------------------------------------------

            if (score > alpha) {
                alpha = score;
            }

            if (alpha >= beta) {
                break;
            }
        }


        tt.store(position.key(),
                 depth,
                 score_to_tt(best_score, 0),
                 TTFlag::Exact,
                 best_move
        );

        std::vector<Move> pv;

        pv.reserve(static_cast<std::size_t>(PvLength[0]));

        for (int i = 0; i < PvLength[0]; ++i) {
            pv.push_back(PvTable[0][i]);
        }

        return SearchResult{
            .best_move = best_move,
            .score = best_score,
            .pv = std::move(pv)
        };
    }


    SearchStats search_stats() noexcept {
        return SearchStats{
            .nodes = Nodes,
            .tt_hits = TTHits,
            .qnodes = QNodes
        };
    }


    void reset_search_stats() noexcept {
        Nodes = 0;
        TTHits = 0;
        QNodes = 0;
    }

    IterativeSearchResult iterative_deepening(
        GameState &game,
        TranspositionTable &tt,
        int max_depth,
        bool use_move_ordering,
        SearchControl &control
    ) {
        assert(max_depth >= 1);

        IterativeSearchResult result{};

        result.iterations.reserve(
            static_cast<std::size_t>(max_depth)
        );

        reset_search_stats();

        SearchStats previous_stats{};

        for (int depth = 1; depth <= max_depth; ++depth) {

            if (control.should_stop()) {
                break;
            }

            // Reuse the same GameState and TT.
            const SearchResult current =
                    search(
                        game,
                        tt,
                        depth,
                        use_move_ordering,
                        control
                    );

            if (control.should_stop()) {
                break;
            }

            // search_stats() is cumulative because
            // search() does not reset the counters.
            const SearchStats cumulative =
                    search_stats();

            // Calculate work done at this depth only.
            SearchStats iteration_stats{};

            iteration_stats.nodes =
                    cumulative.nodes - previous_stats.nodes;

            iteration_stats.qnodes =
                    cumulative.qnodes - previous_stats.qnodes;

            iteration_stats.tt_hits =
                    cumulative.tt_hits - previous_stats.tt_hits;

            result.iterations.push_back(
                IterationInfo{
                    depth,
                    current.best_move,
                    current.score,
                    iteration_stats,
                    current.pv
                }
            );

            result.result = current;
            result.completed_depth = depth;
            result.stats = cumulative;

            previous_stats = cumulative;
        }

        return result;
    }


    bool is_mate_score(int score) noexcept {
        return score >= MateThreshold ||
               score <= -MateThreshold;
    }
} // namespace Aribella
