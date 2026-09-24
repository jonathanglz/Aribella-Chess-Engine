#include "move_format.h"
#include <iostream>

namespace Aribella {
    namespace {
        char promotion_to_char(PieceType piece) noexcept {
            switch (piece) {
                case PieceType::Queen:
                    return 'q';

                case PieceType::Rook:
                    return 'r';

                case PieceType::Bishop:
                    return 'b';

                case PieceType::Knight:
                    return 'n';

                default:
                    assert(false && "Invalid promotion piece");
                    return '?';
            }
        }

        void append_square(std::string &result, Square square) {
            const int index = static_cast<int>(square);

            assert(index >= 0 && index < 64);

            const int file = index % 8;
            const int rank = index / 8;

            result += static_cast<char>('a' + file);
            result += static_cast<char>('1' + rank);
        }
    }

    std::string move_to_string(const Move &move) {

        if (move.from == Square::None || move.to == Square::None) {
            return "0000";
        }

        std::string result;
        result.reserve(5);

        append_square(result, move.from);
        append_square(result, move.to);

        if (is_promotion(move)) {
            result += promotion_to_char(move.promotion);
        }

        return result;
    }

}
