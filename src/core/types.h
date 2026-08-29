#pragma once

#include <cstdint>
#include <cassert>

namespace Aribella {
    using Bitboard = std::uint64_t;
    using u8 = std::uint8_t;
    using i8 = std::int8_t;


    // --------------------------------------------------
    // Color
    // --------------------------------------------------

    enum class Color : u8 {
        White = 0,
        Black,

        Count
    };

    constexpr std::size_t to_index(Color c) noexcept {
        return static_cast<std::size_t>(c);
    }

    constexpr bool is_valid(Color c) noexcept {
        return to_index(c) < to_index(Color::Count);
    }

    constexpr Color operator~(Color c) noexcept {
        assert(is_valid(c));

        return c == Color::White
                   ? Color::Black
                   : Color::White;
    }

    // ---------------------------------------------------
    // PieceType
    // ---------------------------------------------------
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

    constexpr std::size_t to_index(PieceType type) noexcept {
        return static_cast<std::size_t>(type);
    }

    constexpr bool is_valid(PieceType type) noexcept {
        return to_index(type) < to_index(PieceType::Count);
    }

    // --------------------------------------------------
    // Piece
    // --------------------------------------------------

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

    constexpr std::size_t to_index(Piece piece) noexcept {
        return static_cast<std::size_t>(piece);
    }

    constexpr bool is_valid(Piece piece) noexcept {
        return to_index(piece) < to_index(Piece::Count);
    }

    constexpr Color color_of(Piece piece) noexcept {
        assert(is_valid(piece));

        return static_cast<Color>(to_index(piece) / to_index(PieceType::Count));
    }

    constexpr PieceType type_of(Piece piece) noexcept {
        assert(is_valid(piece));

        return static_cast<PieceType>(to_index(piece) % to_index(PieceType::Count));
    }

    constexpr Piece make_piece(Color c, PieceType type) noexcept {
        assert(is_valid(c));
        assert(is_valid(type));

        return static_cast<Piece>(
            to_index(c) * to_index(PieceType::Count) + to_index(type)
        );
    }

    // ---------------------------------------------------------
    // File
    // ---------------------------------------------------------

    enum class File : u8 {
        A = 0,
        B,
        C,
        D,
        E,
        F,
        G,
        H,

        Count
    };

    constexpr std::size_t to_index(File file) noexcept {
        return static_cast<std::size_t>(file);
    }

    constexpr bool is_valid(File file) noexcept {
        return to_index(file) < to_index(File::Count);
    }

    // ------------------------------------------------------
    // Rank
    // ------------------------------------------------------

    enum class Rank : u8 {
        R1 = 0,
        R2,
        R3,
        R4,
        R5,
        R6,
        R7,
        R8,

        Count
    };

    constexpr std::size_t to_index(Rank rank) noexcept {
        return static_cast<std::size_t>(rank);
    }

    constexpr bool is_valid(Rank rank) noexcept {
        return to_index(rank) < to_index(Rank::Count);
    }

    // ------------------------------------------------------
    // Square
    //-------------------------------------------------------

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

    constexpr std::size_t to_index(Square sq) noexcept {
        return static_cast<std::size_t>(sq);
    }

    constexpr bool is_valid(Square sq) noexcept {
        return to_index(sq) < to_index(Square::Count);
    }

    // Little-endian rank-file mapping:
    // file = square & 7
    // rank = square >> 3
    // square = (rank << 3) + file
    // See: ChessProgramming Wiki - Square_Mapping_Considerations
    constexpr File file_of(Square sq) noexcept {
        assert(is_valid(sq));

        return static_cast<File>(to_index(sq) & 7);
    }

    constexpr Rank rank_of(Square sq) noexcept {
        assert(is_valid(sq));

        return static_cast<Rank>(to_index(sq) >> 3);
    }

    constexpr Square make_square(File file, Rank rank) noexcept {
        assert(is_valid(file));
        assert(is_valid(rank));

        return static_cast<Square>(
            (to_index(rank) << 3) + to_index(file)
        );
    }

    // ------------------------------------------------------
    // Direction
    // ------------------------------------------------------

    enum class Direction : i8 {
        North = 8,
        South = -8,

        East = 1,
        West = -1,

        NorthEast = 9,
        NorthWest = 7,
        SouthEast = -7,
        SouthWest = -9
    };

    constexpr int offset(Direction direction) noexcept {
        return static_cast<int>(direction);
    }

    enum class CastlingRights : u8 {
        None = 0,
        WhiteKingSide = 1 << 0,
        WhiteQueenSide = 1 << 1,
        BlackKingSide = 1 << 2,
        BlackQueenSide = 1 << 3,
    };

    constexpr CastlingRights operator|(CastlingRights lhs, CastlingRights rhs) noexcept {
        return static_cast<CastlingRights>(static_cast<u8>(lhs) | static_cast<u8>(rhs));
    }

    constexpr CastlingRights operator&(CastlingRights lhs, CastlingRights rhs) noexcept {
        return static_cast<CastlingRights>(static_cast<u8>(lhs) & static_cast<u8>(rhs));
    }

    constexpr bool has_castling_rights(CastlingRights rights, CastlingRights right) noexcept {
        return static_cast<u8>(rights & right) != 0;
    }

    
}
