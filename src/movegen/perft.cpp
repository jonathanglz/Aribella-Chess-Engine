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


    std::uint64_t perft(Position &position, int depth) {
        assert(depth >= 0);

        if (depth == 0) {
            return 1;
        }

        const Color side =
                position.side_to_move();

        const MoveList moves =
                generate_legal_moves(position, side);

        std::uint64_t nodes{0};

        for (std::size_t i = 0; i < moves.size(); ++i) {
            const Move move =
                    moves[i];

            const Position before =
                    position;

            const UndoState undo =
                    position.make_move(move);

            nodes += perft(position, depth - 1);

            position.unmake_move(move, undo);

            assert(position == before);
        }

        return nodes;
    }

    void pertf_divide(Position &position, int depth) {
        assert(depth >= 1);

        const Color side =
                position.side_to_move();
        const MoveList moves =
                generate_legal_moves(position, side);

        std::uint64_t total{0};

        for (std::size_t i = 0; i < moves.size(); ++i) {
            const Move move =
                    moves[i];

            const UndoState undo =
                    position.make_move(move);

            const std::uint64_t nodes =
                    perft(position, depth - 1);

            position.unmake_move(move, undo);

            print_move(move);

            std::cout << ": " << nodes << '\n';

            total += nodes;
        }

        std::cout << "\nTotal: " << total << '\n';
    }
}
