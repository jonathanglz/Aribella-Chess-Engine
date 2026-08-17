#include "position.h"

#include <cassert>

namespace Aribella {
    Bitboard Position::pieces(Color color, PieceType type) const noexcept {
        assert(is_valid(color));
        assert(is_valid(type));

        return pieces_[to_index(color)][to_index(type)];
    }

    Bitboard Position::occupancy(Color color) const noexcept {
        assert(is_valid(color));

        Bitboard occupied{0};

        for (std::size_t type = 0; type < to_index(PieceType::Count); ++type) {
            occupied |= pieces_[to_index(color)][type];
        }

        return occupied;
    }

    Bitboard Position::occupancy() const noexcept {
        return occupancy(Color::White) | occupancy(Color::Black);
    }

    void Position::set_piece(Color color, PieceType type, Square sq) noexcept {
        assert(is_valid(color));
        assert(is_valid(type));
        assert(is_valid(sq));

        set_bit(pieces_[to_index(color)][to_index(type)], sq);
    }

    void Position::remove_piece(Color color, PieceType type, Square sq) noexcept {
        assert(is_valid(color));
        assert(is_valid(type));
        assert(is_valid(sq));

        clear_bit(pieces_[to_index(color)][to_index(type)], sq);
    }

    void Position::clear() noexcept {
        for (auto& color_pieces : pieces_) {
            for (Bitboard& bb : color_pieces) {
                bb = Bitboard{0};
            }
        }
    }

    Piece Position::piece_at(Square sq) const noexcept {
        assert(is_valid(sq));

        for (std::size_t color = 0; color < to_index(Color::Count); ++color) {
            for (std::size_t type = 0; type < to_index(PieceType::Count); ++type) {
                if (is_set(pieces_[color][type], sq)) {
                    return make_piece(static_cast<Color>(color), static_cast<PieceType>(type));
                }
            }
        }
        return Piece::None;
    }

    bool Position::is_occupied(Square sq) const noexcept {
        assert(is_valid(sq));

        return is_set(occupancy(), sq);
    }

    Color Position::color_at(Square sq) const noexcept {
        assert(is_valid(sq));
        assert(is_occupied(sq));

        if (is_set(occupancy(Color::White), sq)) {
            return Color::White;
        }

        return Color::Black;
    }

}
