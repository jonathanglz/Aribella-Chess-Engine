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


        Bitboard white_pawns = position.pieces(Color::White, PieceType::Pawn);

        while (white_pawns) {
            const Square square = static_cast<Square>(pop_lsb(white_pawns));

            score += PawnTable[pst_index(square, Color::White)];
        }

        Bitboard black_pawns = position.pieces(Color::Black, PieceType::Pawn);

        while (black_pawns) {
            const Square square = static_cast<Square>(pop_lsb(black_pawns));

            score -= PawnTable[pst_index(square, Color::Black)];
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


        Bitboard white_bishops = position.pieces(Color::White, PieceType::Bishop);

        while (white_bishops) {
            const Square square = static_cast<Square>(pop_lsb(white_bishops));

            score += BishopTable[pst_index(square, Color::White)];
        }

        Bitboard black_bishops = position.pieces(Color::Black, PieceType::Bishop);

        while (black_bishops) {
            const Square square = static_cast<Square>(pop_lsb(black_bishops));

            score -= BishopTable[pst_index(square, Color::Black)];
        }


        Bitboard white_rooks = position.pieces(Color::White, PieceType::Rook);

        while (white_rooks) {
            const Square square = static_cast<Square>(pop_lsb(white_rooks));

            score += RookTable[pst_index(square, Color::White)];
        }

        Bitboard black_rooks = position.pieces(Color::Black, PieceType::Rook);

        while (black_rooks) {
            const Square square = static_cast<Square>(pop_lsb(black_rooks));

            score -= RookTable[pst_index(square, Color::Black)];
        }


        Bitboard white_queens = position.pieces(Color::White, PieceType::Queen);

        while (white_queens) {
            const Square square = static_cast<Square>(pop_lsb(white_queens));

            score += QueenTable[pst_index(square, Color::White)];
        }

        Bitboard black_queens = position.pieces(Color::Black, PieceType::Queen);

        while (black_queens) {
            const Square square = static_cast<Square>(pop_lsb(black_queens));

            score -= QueenTable[pst_index(square, Color::Black)];
        }

        
        return score;
    }
}
