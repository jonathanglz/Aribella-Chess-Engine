#pragma once

#include <bit>
#include <cassert>
#include "constants.h"

namespace Aribella {

    void print_bitboard(Bitboard bb);

    constexpr Bitboard square_bb(Square sq) noexcept {
        assert(is_valid(sq));
        return Bitboard{1} << to_index(sq);
    }

    constexpr int popcount(Bitboard bb) noexcept {
        return std::popcount(bb);
    }

    constexpr Square lsb(Bitboard bb) noexcept {
        assert(bb);
        return static_cast<Square>(std::countr_zero(bb));
    }

    constexpr Square pop_lsb(Bitboard& bb) noexcept {
        assert(bb);
        const Square sq = lsb(bb);
        bb &= bb - 1;

        return sq;
    }

    constexpr bool is_set(Bitboard bb, Square sq) noexcept {
        return (bb & square_bb(sq)) != 0;
    }

    constexpr void set_bit(Bitboard& bb, Square sq) noexcept {
        bb |= square_bb(sq);
    }

    constexpr void clear_bit(Bitboard& bb, Square sq) noexcept {
        bb &= ~square_bb(sq);
    }

    constexpr Bitboard file_mask(File file) noexcept {
        assert(is_valid(file));
        return FileMasks[to_index(file)];
    }

    constexpr Bitboard rank_mask(Rank rank) noexcept {
        assert(is_valid(rank));
        return RankMasks[to_index(rank)];
    }

    constexpr Bitboard shift_north(Bitboard bb) noexcept {
        return bb << offset(Direction::North);
    }

    constexpr Bitboard shift_south(Bitboard bb) noexcept {
        return bb >> -offset(Direction::South);
    }

    constexpr Bitboard shift_east(Bitboard bb) noexcept {
        return (bb & ~FileH) << offset(Direction::East);
    }

    constexpr Bitboard shift_west(Bitboard bb) noexcept {
        return (bb & ~FileA) >> -offset(Direction::West);
    }

    constexpr Bitboard shift_northeast(Bitboard bb) noexcept {
        return (bb & ~FileH) << offset(Direction::NorthEast);
    }

    constexpr Bitboard shift_northwest(Bitboard bb) noexcept {
        return (bb & ~FileA) << offset(Direction::NorthWest);
    }

    constexpr Bitboard shift_southeast(Bitboard bb) noexcept {
        return (bb & ~FileH) >> -offset(Direction::SouthEast);
    }

    constexpr Bitboard shift_southwest(Bitboard bb) noexcept {
        return (bb & ~FileA) >> -offset(Direction::SouthWest);
    }

    // See ChessProgramming Wiki - Pawn Attacks(Bitboards)
    constexpr Bitboard pawn_attacks(Color c, Bitboard pawns) noexcept {
        assert(is_valid(c));

        return c == Color::White
            ? shift_northeast(pawns) | shift_northwest(pawns)
            : shift_southeast(pawns) | shift_southwest(pawns);
    }

    // See ChessProgramming Wiki - Knight_Pattern(Knight Attacks)
    constexpr Bitboard generate_knight_attacks(Square sq) noexcept {
        assert(is_valid(sq));

        const Bitboard bb = square_bb(sq);

        return
            shift_north(shift_northeast(bb)) |
            shift_north(shift_northwest(bb)) |
            shift_south(shift_southeast(bb)) |
            shift_south(shift_southwest(bb)) |
            shift_east(shift_northeast(bb))  |
            shift_east(shift_southeast(bb))  |
            shift_west(shift_northwest(bb))  |
            shift_west(shift_southwest(bb));
    }

    constexpr std::array<Bitboard, to_index(Square::Count)> make_knight_attacks_table() noexcept {
        std::array<Bitboard, to_index(Square::Count)> attacksTable{};

        for (int i = 0; i < to_index(Square::Count); ++i) {
            const auto sq = static_cast<Square>(i);
            attacksTable[i] = generate_knight_attacks(sq);
        }

        return attacksTable;
    }

    inline constexpr auto KnightAttacksTable = make_knight_attacks_table();

    constexpr Bitboard knight_attacks(Square sq) noexcept {
        assert(is_valid(sq));
        return KnightAttacksTable[to_index(sq)];
    }


}