#include <cassert>
#include <cstddef>

#include "../src/core/bitboard.h"

using namespace Aribella;

// square_bb
static_assert(square_bb(Square::A1) == Bitboard{1});
static_assert(square_bb(Square::H8) == (Bitboard{1} << 63));

// popcount
static_assert(popcount(Bitboard{0}) == 0);
static_assert(popcount(square_bb(Square::E4)) == 1);
static_assert(popcount(square_bb(Square::E4) | square_bb(Square::D4)) == 2);

// is_set
static_assert(is_set(square_bb(Square::E4), Square::E4));
static_assert(!is_set(square_bb(Square::E4), Square::D4));

// masks
static_assert(file_mask(File::A) == FileA);
static_assert(file_mask(File::H) == FileH);

static_assert(rank_mask(Rank::R1) == Rank1);
static_assert(rank_mask(Rank::R8) == Rank8);

// shifts
static_assert(
    shift_north(square_bb(Square::E4))
    == square_bb(Square::E5)
);

static_assert(
    shift_south(square_bb(Square::E4))
    == square_bb(Square::E3)
);

static_assert(
    shift_east(square_bb(Square::E4))
    == square_bb(Square::F4)
);

static_assert(
    shift_west(square_bb(Square::E4))
    == square_bb(Square::D4)
);

// diagonal shifts
static_assert(
    shift_northeast(square_bb(Square::E4))
    == square_bb(Square::F5)
);

static_assert(
    shift_northwest(square_bb(Square::E4))
    == square_bb(Square::D5)
);

static_assert(
    shift_southeast(square_bb(Square::E4))
    == square_bb(Square::F3)
);

static_assert(
    shift_southwest(square_bb(Square::E4))
    == square_bb(Square::D3)
);

// wraparound protection
static_assert(
    shift_east(square_bb(Square::H4)) == Bitboard{0}
);

static_assert(
    shift_west(square_bb(Square::A4)) == Bitboard{0}
);

static_assert(
    shift_northeast(square_bb(Square::H4)) == Bitboard{0}
);

static_assert(
    shift_northwest(square_bb(Square::A4)) == Bitboard{0}
);

static_assert(
    shift_southeast(square_bb(Square::H4)) == Bitboard{0}
);

static_assert(
    shift_southwest(square_bb(Square::A4)) == Bitboard{0}
);