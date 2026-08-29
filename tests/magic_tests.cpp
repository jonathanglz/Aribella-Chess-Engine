#include "../src/movegen/attacks.h"
#include "../src/movegen/magic_constants.h"


using namespace Aribella;

constexpr Bitboard mask = square_bb(Square::B2) | square_bb(Square::C3) | square_bb(Square::D4);

static_assert(set_occupancy(0, mask) == Bitboard{0});

static_assert(set_occupancy(1, mask) == square_bb(Square::B2));

static_assert(set_occupancy(3, mask) == (square_bb(Square::B2) | square_bb(Square::C3)));

static_assert(set_occupancy(7, mask) == mask);

constexpr auto rook_table = generate_rook_occupancy_attack_table(Square::D4);

constexpr Bitboard rook_mask = generate_rook_relevant_mask(Square::D4);

static_assert(rook_table.count == (std::size_t{1} << popcount(rook_mask)));

constexpr auto bishop_table = generate_bishop_occupancy_attack_table(Square::D4);

constexpr Bitboard bishop_mask = generate_bishop_relevant_mask(Square::D4);

static_assert(bishop_table.count == (std::size_t{1} << popcount(bishop_mask)));

static_assert(rook_table.entries[0].occupancy == Bitboard{0});

static_assert(rook_table.entries[0].attacks == generate_rook_attacks(Square::D4, Bitboard{0}));

static_assert(bishop_table.entries[0].occupancy == Bitboard{0});

static_assert(bishop_table.entries[0].attacks == generate_bishop_attacks(Square::D4, Bitboard{0}));

static_assert(rook_table.entries[rook_table.count - 1].occupancy == rook_mask);

static_assert(rook_table.entries[rook_table.count - 1].attacks ==
    generate_rook_attacks(Square::D4, rook_mask));

static_assert(bishop_table.entries[bishop_table.count - 1].occupancy == bishop_mask);

static_assert(bishop_table.entries[bishop_table.count - 1].attacks ==
    generate_bishop_attacks(Square::D4, bishop_mask));
