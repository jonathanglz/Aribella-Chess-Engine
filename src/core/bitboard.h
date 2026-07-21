#pragma once

#include <bit>
#include <cassert>

#include "types.h"
#include "constants.h"

namespace Aribella {

    namespace Bitboards {
        void print_bitboard(Bitboard bb);
    }

    constexpr Bitboard square_bb(Square sq) noexcept {
        assert(is_valid(sq));
        return Bitboard{1} << to_index(sq);
    }

    inline int popcount(Bitboard bb) noexcept {
        return std::popcount(bb);
    }

    inline Square lsb(Bitboard bb) noexcept {
        assert(bb);
        return static_cast<Square>(std::countr_zero(bb));
    }

    inline Square pop_lsb(Bitboard& bb) noexcept {
        assert(bb);
        const Square sq = lsb(bb);
        bb &= bb - 1;

        return sq;
    }

    constexpr bool is_set(Bitboard bb, Square sq) noexcept {
        return (bb & square_bb(sq)) != 0;
    }

    constexpr void set_bit(Bitboard bb, Square sq) noexcept {
        bb |= square_bb(sq);
    }

    constexpr void clear_bit(Bitboard bb, Square sq) noexcept {
        bb &= ~square_bb(sq);
    }


}