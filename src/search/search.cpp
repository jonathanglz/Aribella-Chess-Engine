#include "search.h"

#include <cassert>
#include <limits>

#include "../evaluation/evaluation.h"
#include "../movegen/movegen.h"

namespace Aribella {
    namespace {
        inline constexpr int Infinity = 1000000;

        [[nodiscard]]
        int negamax_impl(GameState &game, int depth, int ply, int alpha, int beta) {
            assert(depth >= 0);
            assert(ply >= 0);
            assert(alpha < beta);

            const Position &position = game.position();


            // ---------------------------------------------------------
            // Draw by repetition
            // ---------------------------------------------------------

            if (game.is_threefold_repetition()) {
                return 0;
            }


            // ---------------------------------------------------------
            // Draw by fifty-move rule
            // ---------------------------------------------------------

            if (position.halfmove_clock() >= 100) {
                return 0;
            }


            // ---------------------------------------------------------
            // Leaf node
            // ---------------------------------------------------------

            if (depth == 0) {
                const int score = evaluate(position);

                return position.side_to_move() == Color::White
                           ? score
                           : -score;
            }


            // ---------------------------------------------------------
            // Generate legal moves
            // ---------------------------------------------------------

            Position search_position = position;

            const MoveList moves = generate_legal_moves(search_position, position.side_to_move());


            // ---------------------------------------------------------
            // Checkmate / stalemate
            // ---------------------------------------------------------

            if (moves.empty()) {
                if (in_check(search_position, position.side_to_move())) {
                    return -MateScore + ply;
                }
                return 0;
            }


            // ---------------------------------------------------------
            // Search all moves
            // ---------------------------------------------------------

            int best_score = -Infinity;

            for (std::size_t i = 0; i < moves.size(); ++i) {
                const Move move = moves[i];

                const UndoState undo = game.make_move(move);

                const int score = -negamax_impl(game, depth - 1, ply + 1, -beta, -alpha);

                game.unmake_move(move, undo);

                if (score > best_score) {
                    best_score = score;
                }

                if (score > alpha) {
                    alpha = score;
                }

                // alpha-beta cutoff
                if (alpha >= beta) {
                    break;
                }
            }
            return best_score;
        }
    }

    
    SearchResult search(GameState &game, int depth) {
        assert(depth >= 0);

        const Position &position = game.position();

        if (game.is_threefold_repetition()) {
            return SearchResult{
                .best_move = Move{},
                .score = 0
            };
        }

        if (position.halfmove_clock() >= 100) {
            return SearchResult{
                .best_move = Move{},
                .score = 0
            };
        }

        Position search_position = position;

        const MoveList moves = generate_legal_moves(search_position, position.side_to_move());

        // No legal moves
        if (moves.empty()) {
            if (in_check(search_position, position.side_to_move())) {
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

        Move best_move{};

        int best_score = -Infinity;

        int alpha = -Infinity;

        constexpr int beta = Infinity;

        for (std::size_t i = 0; i < moves.size(); ++i) {
            const Move move = moves[i];

            const UndoState undo = game.make_move(move);

            const int score = -negamax_impl(game, depth - 1, 1, -beta, -alpha);

            game.unmake_move(move, undo);

            if (score > best_score) {
                best_score = score;
                best_move = move;
            }

            if (score > alpha) {
                alpha = score;
            }
        }

        return SearchResult{
            .best_move = best_move,
            .score = best_score
        };
    }
}
