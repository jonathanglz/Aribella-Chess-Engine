#include "perft.h"
#include <cassert>
#include <iostream>

#include "movegen.h"

namespace Aribella {
    namespace {
        char file_char(Square sq) noexcept {
            return static_cast<char>('a' + to_index(file_of(sq)));
        }

        char rank_char(Square sq) noexcept {
            return static_cast<char>('1' + to_index(rank_of(sq)));
        }

        void print_move(const Move &move) {
            std::cout << file_char(move.from) << rank_char(move.from) << file_char(move.to) << rank_char(move.to);

            if (is_promotion(move)) {
                switch (move.promotion) {
                    case PieceType::Queen:
                        std::cout << 'q';
                        break;

                    case PieceType::Rook:
                        std::cout << 'r';
                        break;

                    case PieceType::Bishop:
                        std::cout << 'b';
                        break;

                    case PieceType::Knight:
                        std::cout << 'n';
                        break;

                    default:
                        break;
                }
            }
        }
    }


    std::uint64_t perft(GameState &game, int depth) {
        assert(depth >= 0);

        // One leaf: current position
        if (depth == 0) {
            return 1;
        }

        const Position &position = game.position();

        Position move_position = position;

        const MoveList moves =
                generate_legal_moves(move_position, position.side_to_move());

        if (depth == 1) {
            return static_cast<std::uint64_t>(moves.size());
        }

        std::uint64_t nodes{0};

        for (std::size_t i = 0; i < moves.size(); ++i) {
            const Move move = moves[i];

            const UndoState undo = game.make_move(move);

            nodes += perft(game, depth - 1);

            game.unmake_move(move, undo);
        }

        return nodes;
    }

    void pertf_divide(GameState &game, int depth) {
        assert(depth >= 1);

        const Position &position = game.position();

        Position move_position = position;

        const MoveList moves =
                generate_legal_moves(move_position, position.side_to_move());

        std::uint64_t total{0};

        for (std::size_t i = 0; i < moves.size(); ++i) {
            const Move move =
                    moves[i];

            const UndoState undo =
                    game.make_move(move);

            const std::uint64_t nodes =
                    perft(game, depth - 1);

            game.unmake_move(move, undo);

            print_move(move);

            std::cout << ": " << nodes << '\n';

            total += nodes;
        }

        std::cout << "\nTotal: " << total << '\n';
    }
}
