#pragma once

#include <bit>
#include <cassert>
#include "constants.h"

namespace Aribella {

    namespace Bitboards {
        void print_bitboard(Bitboard bb);
    }

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

}