#include "evaluation.h"

#include <array>

namespace Aribella {
    namespace {
        constexpr std::array<int, 6> PieceValues{
            100, // pawn
            320, // knight
            330, // bishop
            500, // rook
            900, // queen
            0 // king
        };

        constexpr std::array<int, 64> KnightTable{
            -50, -40, -30, -30, -30, -30, -40, -50,
            -40, -20,   0,   5,   5,   0, -20, -40,
            -30,   5,  10,  15,  15,  10,   5, -30,
            -30,   0,  15,  20,  20,  15,   0, -30,
            -30,   5,  15,  20,  20,  15,   5, -30,
            -30,   0,  10,  15,  15,  10,   0, -30,
            -40, -20,   0,   0,   0,   0, -20, -40,
            -50, -40, -30, -30, -30, -30, -40, -50
        };

        [[nodiscard]]
        constexpr std::size_t pst_index(Square square, Color color) noexcept {
            const std::size_t index = static_cast<std::size_t>(square);

            if (color == Color::White) {
                return index;
            }

            return index ^ 56;
        }
    }

    int evaluate(const Position &position) noexcept {
        int score{0};

        for (std::size_t type_index = 0; type_index < to_index(PieceType::Count); ++type_index) {
            const auto type =
                    static_cast<PieceType>(type_index);

            const int value =
                    PieceValues[type_index];

            const int white_count =
                    popcount(position.pieces(Color::White, type));

            const int black_count =
                    popcount(position.pieces(Color::Black, type));

            score += white_count * value;

            score -= black_count * value;
        }

        Bitboard white_knights = position.pieces(Color::White, PieceType::Knight);

        while (white_knights) {
            const Square square = static_cast<Square>(pop_lsb(white_knights));

            score += KnightTable[pst_index(square, Color::White)];
        }

        Bitboard black_knights = position.pieces(Color::Black, PieceType::Knight);

        while (black_knights) {
            const Square square = static_cast<Square>(pop_lsb(black_knights));

            score -= KnightTable[pst_index(square, Color::Black)];
        }

        return score;
    }
}
