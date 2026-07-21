#pragma once

#include <cstdint>

namespace Aribella {

    using Bitboard = std::uint64_t;
    using u8 = std::uint8_t;

    enum class Color : u8 {
        White = 0,
        Black = 1,
    };

    enum class PieceType : u8 {
        Pawn = 0,
        Knight,
        Bishop,
        Rook,
        Queen,
        King,

        Count,
        None = Count
    };

    enum class Piece : u8 {
        WhitePawn = 0,
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

        Count,
        None = Count
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

    constexpr int to_index(Color c) noexcept {
        return static_cast<int>(c);
    }

    constexpr int to_index(PieceType type) noexcept {
        return static_cast<int>(type);
    }

    constexpr int to_index(Piece piece) noexcept {
        return static_cast<int>(piece);
    }

    constexpr int to_index(Square sq) noexcept {
        return static_cast<int>(sq);
    }

    constexpr bool is_valid(Color c) noexcept {
        return to_index(c) < 2;
    }

    constexpr bool is_valid(PieceType type) noexcept {
        return to_index(type) < to_index(PieceType::Count);
    }

    constexpr bool is_valid(Piece piece) noexcept {
        return to_index(piece) < to_index(Piece::Count);
    }

    constexpr bool is_valid(Square sq) noexcept {
        return to_index(sq) < to_index(Square::Count);
    }

    constexpr Color operator~(Color c) noexcept {
        return c == Color::White ? Color::Black : Color::White;
    }

    constexpr Piece make_piece(Color c, PieceType type) noexcept {
        assert(is_valid(c));
        assert(is_valid(type));

        return static_cast<Piece>(
            to_index(c) * to_index(PieceType::Count) + to_index(type)
        );
    }

    constexpr Color color_of(Piece piece) noexcept {
        assert(is_valid(piece));

        return static_cast<Color>(to_index(piece) / to_index(PieceType::Count));
    }

    constexpr PieceType type_of(Piece piece) noexcept {
        assert(is_valid(piece));

        return static_cast<PieceType>(to_index(piece) % to_index(PieceType::Count));
    }

}
