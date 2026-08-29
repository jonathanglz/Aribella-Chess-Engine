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
        for (auto &color_pieces: pieces_) {
            for (Bitboard &bb: color_pieces) {
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

    Square Position::en_passant_square() const noexcept {
        return en_passant_square_;
    }

    void Position::set_en_passant_square(Square sq) noexcept {
        assert(is_valid(sq));

        en_passant_square_ = sq;
    }

    void Position::clear_en_passant_square() noexcept {
        en_passant_square_ = Square::None;
    }

    CastlingRights Position::castling_rights() const noexcept {
        return castling_rights_;
    }

    bool Position::can_castle(CastlingRights right) const noexcept {
        return has_castling_rights(castling_rights_, right);
    }

    void Position::set_castling_rights(CastlingRights right) noexcept {
        castling_rights_ = right;
    }

    void Position::remove_castling_rights(CastlingRights right) noexcept {
        castling_rights_ = static_cast<CastlingRights>(
            static_cast<u8>(castling_rights_) & ~static_cast<u8>(right)
        );
    }

    Piece Position::remove_piece_at(Square sq) noexcept {
        assert(is_valid(sq));

        const Piece piece = piece_at(sq);

        if (piece == Piece::None) {
            return Piece::None;
        }

        remove_piece(color_of(piece), type_of(piece), sq);

        return piece;
    }

    UndoState Position::make_move(Position& position, const Move& move) noexcept {
        assert(is_valid(move.from));
        assert(is_valid(move.to));

        const Piece moving_piece = position.piece_at(move.from);

        assert(moving_piece != Piece::None);

        const Color side = color_of(moving_piece);

        const PieceType type = type_of(moving_piece);

        UndoState undo {
            .captured_piece = Piece::None,
            .castling_rights = position.castling_rights(),
            .en_passant_square = position.en_passant_square()
        };

        position.clear_en_passant_square();

        if (move.flag == MoveFlag::Capture || move.flag == MoveFlag::PromotionCapture) {
            undo.captured_piece = position.remove_piece_at(move.to);

            assert(undo.captured_piece != Piece::None);
        }

        if (move.flag == MoveFlag::EnPassant) {
            assert(type == PieceType::Pawn);

            const Square captured_square = (side == Color::White)
            ? static_cast<Square>(to_index(move.from) - 8)
            : static_cast<Square>(to_index(move.to) + 8);

            undo.captured_piece = position.remove_piece_at(captured_square);

            assert(undo.captured_piece != Piece::None);

            assert(type_of(undo.captured_piece) == PieceType::Pawn);
        }

        position.remove_piece(side, type, move.from);

        if (is_promotion(move)) {
            assert(type == PieceType::Pawn);

            position.set_piece(side, move.promotion, move.to);

            position.set_piece(side, move.promotion, move.to);
        }
        else {
            position.set_piece(side, type, move.to);
        }

        if (move.flag == MoveFlag::DoublePawnPush) {
            assert(type == PieceType::Pawn);

            const Square ep = (side == Color::White)
            ? static_cast<Square>(to_index(move.from) + 8)
            : static_cast<Square>(to_index(move.to) - 8);

            position.set_en_passant_square(ep);
        }

        if (move.flag == MoveFlag::KingCastle) {
            assert(type == PieceType::King);

            if (side == Color::White) {
                position.remove_piece(Color::White, PieceType::Rook, Square::H1);

                position.set_piece(Color::White, PieceType::Rook, Square::F1);
            }
            else {
                position.remove_piece(Color::Black, PieceType::Rook, Square::H8);

                position.set_piece(Color::Black, PieceType::Rook, Square::F8);
            }
        }

        if (move.flag == MoveFlag::QueenCastle) {
            assert(type == PieceType::King);

            if (side == Color::White) {
                position.remove_piece(Color::White, PieceType::Rook, Square::A1);

                position.set_piece(Color::White, PieceType::Rook, Square::D1);
            }
            else {
                position.remove_piece(Color::Black, PieceType::Rook, Square::A8);

                position.set_piece(Color::Black, PieceType::Rook, Square::D8);
            }
        }

        if (type == PieceType::King)
        return undo;
    }

    void Position::unmake_move(Position &position, const Move &move, const UndoState &undo) noexcept {

    }
}
