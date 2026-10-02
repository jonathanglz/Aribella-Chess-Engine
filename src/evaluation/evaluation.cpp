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

        constexpr std::array<int, 64> PawnTable{
            0, 0, 0, 0, 0, 0, 0, 0,
            5, 10, 10, -20, -20, 10, 10, 5,
            5, -5, -10, 0, 0, -10, -5, 5,
            0, 0, 0, 20, 20, 0, 0, 0,
            5, 5, 10, 25, 25, 10, 5, 5,
            10, 10, 20, 30, 30, 20, 10, 10,
            50, 50, 50, 50, 50, 50, 50, 50,
            0, 0, 0, 0, 0, 0, 0, 0
        };

        constexpr std::array<int, 64> KnightTable{
            -50, -40, -30, -30, -30, -30, -40, -50,
            -40, -20, 0, 5, 5, 0, -20, -40,
            -30, 5, 10, 15, 15, 10, 5, -30,
            -30, 0, 15, 20, 20, 15, 0, -30,
            -30, 5, 15, 20, 20, 15, 5, -30,
            -30, 0, 10, 15, 15, 10, 0, -30,
            -40, -20, 0, 0, 0, 0, -20, -40,
            -50, -40, -30, -30, -30, -30, -40, -50
        };

        constexpr std::array<int, 64> BishopTable{
            -20, -10, -10, -10, -10, -10, -10, -20,
            -10, 5, 0, 0, 0, 0, 5, -10,
            -10, 10, 10, 10, 10, 10, 10, -10,
            -10, 0, 10, 10, 10, 10, 0, -10,
            -10, 5, 5, 10, 10, 5, 5, -10,
            -10, 0, 5, 10, 10, 5, 0, -10,
            -10, 0, 0, 0, 0, 0, 0, -10,
            -20, -10, -10, -10, -10, -10, -10, -20
        };

        constexpr std::array<int, 64> RookTable{
            0, 0, 0, 5, 5, 0, 0, 0,
            -5, 0, 0, 0, 0, 0, 0, -5,
            -5, 0, 0, 0, 0, 0, 0, -5,
            -5, 0, 0, 0, 0, 0, 0, -5,
            -5, 0, 0, 0, 0, 0, 0, -5,
            -5, 0, 0, 0, 0, 0, 0, -5,
            5, 10, 10, 10, 10, 10, 10, 5,
            0, 0, 0, 0, 0, 0, 0, 0
        };

        constexpr std::array<int, 64> QueenTable{
            -20, -10, -10, -5, -5, -10, -10, -20,
            -10, 0, 5, 0, 0, 0, 0, -10,
            -10, 5, 5, 5, 5, 5, 0, -10,
            0, 0, 5, 5, 5, 5, 0, -5,
            -5, 0, 5, 5, 5, 5, 0, -5,
            -10, 0, 5, 5, 5, 5, 0, -10,
            -10, 0, 0, 0, 0, 0, 0, -10,
            -20, -10, -10, -5, -5, -10, -10, -20
        };

        constexpr std::array<int, 64> KingMiddleGameTable{
            20, 30, 10, 0, 0, 10, 30, 20,
            20, 20, 0, 0, 0, 0, 20, 20,
            -10, -20, -20, -20, -20, -20, -20, -10,
            -20, -30, -30, -40, -40, -30, -30, -20,
            -30, -40, -40, -50, -50, -40, -40, -30,
            -30, -40, -40, -50, -50, -40, -40, -30,
            -30, -40, -40, -50, -50, -40, -40, -30,
            -30, -40, -40, -50, -50, -40, -40, -30
        };

        constexpr std::array<int, 64> KingEndGameTable{
            -50, -40, -30, -20, -20, -30, -40, -50,
            -30, -20, -10, 0, 0, -10, -20, -30,
            -30, -10, 20, 30, 30, 20, -10, -30,
            -30, -10, 30, 40, 40, 30, -10, -30,
            -30, -10, 30, 40, 40, 30, -10, -30,
            -30, -10, 20, 30, 30, 20, -10, -30,
            -30, -30, 0, 0, 0, 0, -30, -30,
            -50, -30, -30, -30, -30, -30, -30, -50
        };

        [[nodiscard]]
        constexpr std::size_t pst_index(Square square, Color color) noexcept {
            const std::size_t index = static_cast<std::size_t>(square);

            if (color == Color::White) {
                return index;
            }

            return index ^ 56;
        }

        [[nodiscard]]
        int piece_square_score(const Position &position, PieceType type, const std::array<int, 64> &table) noexcept {
            int score{0};

            Bitboard white_pieces = position.pieces(Color::White, type);

            while (white_pieces) {
                const Square square = static_cast<Square>(pop_lsb(white_pieces));

                score += table[pst_index(square, Color::White)];
            }

            Bitboard black_pieces = position.pieces(Color::Black, type);

            while (black_pieces) {
                const Square square = static_cast<Square>(pop_lsb(black_pieces));

                score -= table[pst_index(square, Color::Black)];
            }

            return score;
        }
    }

    [[nodiscard]]
    bool is_endgame(const Position &position) noexcept {
        const int white_queens = popcount(position.pieces(Color::White, PieceType::Queen));

        const int black_queens = popcount(position.pieces(Color::Black, PieceType::Queen));

        if (white_queens == 0 && black_queens == 0) {
            return true;
        }

        const auto queen_side_has_low_material =
                [&position](Color color) noexcept {
                    const int queens = popcount(position.pieces(color, PieceType::Queen));

                    if (queens == 0) {
                        return true;
                    }

                    const int rooks = popcount(position.pieces(color, PieceType::Rook));

                    const int minors =
                            popcount(position.pieces(color, PieceType::Bishop)) +
                            popcount(position.pieces(color, PieceType::Knight));

                    return rooks == 0 && minors <= 1;
        };
        return queen_side_has_low_material(Color::White) && queen_side_has_low_material(Color::Black);
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

        score += piece_square_score(position, PieceType::Pawn, PawnTable);

        score += piece_square_score(position, PieceType::Knight, KnightTable);

        score += piece_square_score(position, PieceType::Bishop, BishopTable);

        score += piece_square_score(position, PieceType::Rook, RookTable);

        score += piece_square_score(position, PieceType::Queen, QueenTable);

        const bool endgame = is_endgame(position);

        const auto &king_table = endgame
                                     ? KingEndGameTable
                                     : KingMiddleGameTable;

        score += piece_square_score(position, PieceType::King, king_table);

        return score;
    }
}
