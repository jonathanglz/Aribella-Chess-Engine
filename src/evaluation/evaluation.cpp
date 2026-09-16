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

        return score;
    }
}
