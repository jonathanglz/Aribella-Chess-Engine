#pragma once

#include "../core/types.h"
#include "../position/position.h"
#include "attacks.h"
#include "move_list.h"

namespace Aribella {
    [[nodiscard]]
    Bitboard rook_moves(const Position &position, Square sq, Color side) noexcept;

    [[nodiscard]]
    Bitboard bishop_moves(const Position &position, Square sq, Color side) noexcept;

    [[nodiscard]]
    Bitboard queen_moves(const Position &position, Square sq, Color side) noexcept;

    [[nodiscard]]
    Bitboard knight_moves(const Position &position, Square sq, Color side) noexcept;

    [[nodiscard]]
    Bitboard king_moves(const Position &position, Square sq, Color side) noexcept;

    void generate_rook_moves(MoveList& moves, const Position& position, Square sq, Color side) noexcept;

    void generate_bishop_moves(MoveList& moves, const Position& position, Square sq, Color side) noexcept;

    void generate_queen_moves(MoveList& moves, const Position& position, Square sq, Color side) noexcept;

    void generate_knight_moves(MoveList& moves, const Position& position, Square sq, Color side) noexcept;

    void generate_king_moves(MoveList& moves, const Position& position, Square sq, Color side) noexcept;

    void generate_pawn_moves(MoveList& moves, const Position& position, Color side) noexcept;

    [[nodiscard]]
    MoveList generate_pseudo_legal_moves(const Position& position, Color side) noexcept;

    void generate_castling_moves(MoveList& moves, const Position& position, Color side) noexcept;

    [[nodiscard]]
    bool is_square_attacked(const Position& position, Square sq, Color side) noexcept;

    [[nodiscard]]
    bool in_check(const Position& position, Color side) noexcept;
}
