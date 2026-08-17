#pragma once

#include "../core/types.h"
#include "../position/position.h"
#include "magic_tables.h"

namespace Aribella {
    [[nodiscard]]
    Bitboard rook_moves(const Position& position, Square sq, Color side) noexcept;

    [[nodiscard]]
    Bitboard bishop_moves(const Position& position, Square sq, Color side) noexcept;

    [[nodiscard]]
    Bitboard queen_moves(const Position& position, Square sq, Color side) noexcept;
}
