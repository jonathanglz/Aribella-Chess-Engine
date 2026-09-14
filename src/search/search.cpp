#include "search.h"

#include <cassert>
#include <limits>

#include "../evaluation/evaluation.h"
#include "../movegen/movegen.h"

namespace Aribella {
    namespace {
        [[nodiscard]]
        int negamax_impl(GameState &game, int depth, int ply) {
            assert(depth >= 0);
            assert(ply >= 0);

            const Position &position = game.position();

            if (game.is_threefold_repetition()) {
                return 0;
            }

            if (position.halfmove_clock() >= 100) {
                return 0;
            }

            if (depth == 0) {
                const int score = evaluate(position);

                return position.side_to_move() == Color::White
                           ? score
                           : -score;
            }

            Position search_position = position;

            const MoveList moves = generate_legal_moves(search_position, position.side_to_move());

            if (moves.empty()) {
                if (in_check(search_position, position.side_to_move())) {
                    return -MateScore + ply;
                }
                return 0;
            }

            int best_score = std::numeric_limits<int>::min();

            for (std::size_t i = 0; i < moves.size(); ++i) {
                const Move move = moves[i];

                const UndoState undo = game.make_move(move);

                const int score = -negamax_impl(game, depth - 1, ply + 1);

                game.unmake_move(move, undo);

                if (score > best_score) {
                    best_score = score;
                }
            }
            return best_score;
        }
    }

    int negamax(GameState &game, int depth) {
        assert(depth >= 0);

        return negamax_impl(game, depth, 0);
    }
}
