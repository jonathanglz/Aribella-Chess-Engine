#include "movegen.h"
#include <cassert>

namespace Aribella {

    Bitboard rook_moves(const Position &position, Square sq, Color side) noexcept {
        assert(is_valid(sq));
        assert(is_valid(side));

        const Bitboard attacks = rook_attacks(sq, position.occupancy());

        return attacks & ~position.occupancy(side);
    }

    Bitboard bishop_moves(const Position &position, Square sq, Color side) noexcept {
        assert(is_valid(sq));
        assert(is_valid(side));

        const Bitboard attacks = bishop_attacks(sq, position.occupancy());

        return attacks & ~position.occupancy(side);
    }

    Bitboard queen_moves(const Position &position, Square sq, Color side) noexcept {
        assert(is_valid(sq));
        assert(is_valid(side));

        return rook_moves(position, sq, side) | bishop_moves(position, sq, side);
    }
}
