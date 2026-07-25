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

static_assert(
    pawn_attacks(Color::White, square_bb(Square::E4))
        == (square_bb(Square::D5) | square_bb(Square::F5))
);

static_assert(
    pawn_attacks(Color::Black, square_bb(Square::E4))
        == (square_bb(Square::D3) | square_bb(Square::F3))
);

static_assert(
    pawn_attacks(Color::White, square_bb(Square::A4))
    == square_bb(Square::B5)
);

static_assert(
    pawn_attacks(Color::White, square_bb(Square::H4))
    == square_bb(Square::G5)
);

static_assert(
    pawn_attacks(Color::Black, square_bb(Square::A4))
    == square_bb(Square::B3)
);

static_assert(
    pawn_attacks(Color::Black, square_bb(Square::H4))
    == square_bb(Square::G3)
);

static_assert(
    popcount(knight_attacks(Square::E4)) == 8
);

static_assert(
    knight_attacks(Square::E4) ==
    (
        square_bb(Square::D6) |
        square_bb(Square::F6) |
        square_bb(Square::C5) |
        square_bb(Square::G5) |
        square_bb(Square::C3) |
        square_bb(Square::G3) |
        square_bb(Square::D2) |
        square_bb(Square::F2)
    )
);

static_assert(
    popcount(knight_attacks(Square::A1)) == 2
);

static_assert(
    knight_attacks(Square::A1) ==
    (
        square_bb(Square::B3) |
        square_bb(Square::C2)
    )
);

static_assert(
    popcount(knight_attacks(Square::H8)) == 2
);

static_assert(
    knight_attacks(Square::H8) ==
    (
        square_bb(Square::F7) |
        square_bb(Square::G6)
    )
);

static_assert(
    popcount(knight_attacks(Square::H4)) == 4
);

static_assert(
    knight_attacks(Square::H4) ==
    (
        square_bb(Square::F5) |
        square_bb(Square::F3) |
        square_bb(Square::G6) |
        square_bb(Square::G2)
    )
);