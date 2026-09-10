#pragma once

#include <array>
#include <string_view>

#include "../core/bitboard.h"
#include "../movegen/move.h"
#include "undo.h"
#include "zobrist.h"

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

        [[nodiscard]]
        Square en_passant_square() const noexcept;

        void set_en_passant_square(Square sq) noexcept;

        void clear_en_passant_square() noexcept;

        [[nodiscard]]
        CastlingRights castling_rights() const noexcept;

        [[nodiscard]]
        bool can_castle(CastlingRights right) const noexcept;

        void set_castling_rights(CastlingRights right) noexcept;

        void remove_castling_right(CastlingRights right) noexcept;

        [[nodiscard]]
        Piece remove_piece_at(Square sq) noexcept;

        [[nodiscard]]
        UndoState make_move(const Move& move) noexcept;

        void unmake_move(const Move& move, const UndoState& undo) noexcept;

        [[nodiscard]]
        constexpr bool operator==(const Position &other) const noexcept = default;

        void set_start_position() noexcept;

        [[nodiscard]]
        bool set_from_fen(std::string_view fen) noexcept;

        [[nodiscard]]
        Color side_to_move() const noexcept;

        [[nodiscard]]
        std::uint16_t halfmove_clock() const noexcept;

        [[nodiscard]]
        std::uint16_t fullmove_number() const noexcept;

        [[nodiscard]]
        Zobrist::Key key() const noexcept;

        void recompute_key() noexcept;

    private:
        std::array<std::array<Bitboard, to_index(PieceType::Count)>, to_index(Color::Count)> pieces_{};

        Square en_passant_square_{Square::None};

        CastlingRights castling_rights_{CastlingRights::None};

        Color side_to_move_{Color::White};

        std::uint16_t halfmove_clock_{0};

        std::uint16_t fullmove_number_{0};

        Zobrist::Key key_{};
    };

}
