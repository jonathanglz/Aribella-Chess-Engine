#pragma once

#include <cstdint>

namespace Aribella {

    using Bitboard = std::uint64_t;
    using u8 = std::uint8_t;

    enum class Color : u8 {
        White = 0,
        Black = 1,
    };

    constexpr Color operator~(Color c) noexcept {
        return c == Color::White ? Color::Black : Color::White;
    }

    enum class PieceType : u8 {
        Pawn = 0,
        Knight,
        Bishop,
        Rook,
        Queen,
        King,

        None
    };

    enum class Piece : u8 {
        None,

        WhitePawn,
        WhiteKnight,
        WhiteBishop,
        WhiteRook,
        WhiteQueen,
        WhiteKing,

        BlackPawn,
        BlackKnight,
        BlackBishop,
        BlackRook,
        BlackQueen,
        BlackKing,
    };

    enum class Square : u8 {
        A1, B1, C1, D1, E1, F1, G1, H1,
        A2, B2, C2, D2, E2, F2, G2, H2,
        A3, B3, C3, D3, E3, F3, G3, H3,
        A4, B4, C4, D4, E4, F4, G4, H4,
        A5, B5, C5, D5, E5, F5, G5, H5,
        A6, B6, C6, D6, E6, F6, G6, H6,
        A7, B7, C7, D7, E7, F7, G7, H7,
        A8, B8, C8, D8, E8, F8, G8, H8,

        Count,
        None = Count
    };

    constexpr int to_index(Square sq) noexcept {
        return static_cast<int>(sq);
    }

    constexpr bool is_valid(Square sq) noexcept {
        return to_index(sq) < to_index(Square::Count);
    }

}
