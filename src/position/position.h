#pragma once

#include <array>
#include "../core/types.h"
#include "../core/bitboard.h"

namespace Aribella {

    class Position {
    public:
        [[nodiscard]]
        Bitboard pieces(Color color, PieceType type) const noexcept;

        [[nodiscard]]
        Bitboard occupancy(Color color) const noexcept;

        [[nodiscard]]
        Bitboard occupancy() const noexcept;

        void set_piece(Color color, PieceType type, Square sq) noexcept;

        void remove_piece(Color color, PieceType type, Square sq) noexcept;

        void clear() noexcept;

        [[nodiscard]]
        Piece piece_at(Square sq) const noexcept;

        [[nodiscard]]
        bool is_occupied(Square sq) const noexcept;

        [[nodiscard]]
        Color color_at(Square sq) const noexcept;

    private:
        std::array<std::array<Bitboard, to_index(PieceType::Count)>, to_index(Color::Count)> pieces_{};
    };

}