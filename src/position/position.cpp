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

    void Position::remove_castling_right(CastlingRights right) noexcept {
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

    UndoState Position::make_move(const Move &move) noexcept {
        assert(is_valid(move.from));
        assert(is_valid(move.to));

        const Piece moving_piece = piece_at(move.from);

        assert(moving_piece != Piece::None);

        const Color side = color_of(moving_piece);

        const PieceType type = type_of(moving_piece);

        // Remember the old state before this move might destroy or overwrite
        UndoState undo{
            .captured_piece = Piece::None,
            .castling_rights = castling_rights(),
            .en_passant_square = en_passant_square()
        };

        // clear old en passant target
        clear_en_passant_square();

        // ---------------------------------
        // Captures
        // ---------------------------------
        if (move.flag == MoveFlag::Capture || move.flag == MoveFlag::PromotionCapture) {
            undo.captured_piece = remove_piece_at(move.to);

            assert(undo.captured_piece != Piece::None);
        }

        // ---------------------------------
        // En passant captures
        // ---------------------------------

        if (move.flag == MoveFlag::EnPassant) {
            assert(type == PieceType::Pawn);

            const Square captured_square = (side == Color::White)
                                               ? static_cast<Square>(to_index(move.to) - 8)
                                               : static_cast<Square>(to_index(move.to) + 8);

            undo.captured_piece = remove_piece_at(captured_square);

            assert(undo.captured_piece != Piece::None);

            assert(type_of(undo.captured_piece) == PieceType::Pawn);
        }

        // ---------------------------------
        // Remove moving piece from starting square
        // ---------------------------------
        remove_piece(side, type, move.from);

        // ---------------------------------
        // Promotion
        // ---------------------------------
        if (is_promotion(move)) {
            assert(type == PieceType::Pawn);

            // promotion move
            set_piece(side, move.promotion, move.to);
        } else {
            // normal move, capture, double push, en passant, castling king move
            set_piece(side, type, move.to);
        }

        // ---------------------------------
        // Double pawn push
        // ---------------------------------
        if (move.flag == MoveFlag::DoublePawnPush) {
            assert(type == PieceType::Pawn);

            // create new en passant square
            const Square ep = (side == Color::White)
                                  ? static_cast<Square>(to_index(move.from) + 8)
                                  : static_cast<Square>(to_index(move.from) - 8);

            set_en_passant_square(ep);
        }

        // ---------------------------------
        // King side castling
        // ---------------------------------
        if (move.flag == MoveFlag::KingCastle) {
            assert(type == PieceType::King);

            if (side == Color::White) {
                remove_piece(Color::White, PieceType::Rook, Square::H1);

                set_piece(Color::White, PieceType::Rook, Square::F1);
            } else {
                remove_piece(Color::Black, PieceType::Rook, Square::H8);

                set_piece(Color::Black, PieceType::Rook, Square::F8);
            }
        }

        // ---------------------------------
        // Queen side castling
        // ---------------------------------
        if (move.flag == MoveFlag::QueenCastle) {
            assert(type == PieceType::King);

            if (side == Color::White) {
                remove_piece(Color::White, PieceType::Rook, Square::A1);

                set_piece(Color::White, PieceType::Rook, Square::D1);
            } else {
                remove_piece(Color::Black, PieceType::Rook, Square::A8);

                set_piece(Color::Black, PieceType::Rook, Square::D8);
            }
        }

        // ---------------------------------
        // Moving a king permanently removes both castling rights
        // ---------------------------------
        if (type == PieceType::King) {
            if (side == Color::White) {
                remove_castling_right(CastlingRights::WhiteKingSide);

                remove_castling_right(CastlingRights::WhiteQueenSide);
            } else {
                remove_castling_right(CastlingRights::BlackKingSide);

                remove_castling_right(CastlingRights::BlackQueenSide);
            }
        }

        // ---------------------------------
        // Moving a rook from its original square removes
        // the corresponding castling right.
        // ---------------------------------
        if (type == PieceType::Rook) {
            switch (move.from) {
                case Square::H1:
                    remove_castling_right(
                        CastlingRights::WhiteKingSide
                    );
                    break;

                case Square::A1:
                    remove_castling_right(
                        CastlingRights::WhiteQueenSide
                    );
                    break;

                case Square::H8:
                    remove_castling_right(
                        CastlingRights::BlackKingSide
                    );
                    break;

                case Square::A8:
                    remove_castling_right(
                        CastlingRights::BlackQueenSide
                    );
                    break;

                default:
                    break;
            }
        }

        // ----------------------------------
        // Capturing a rook on its original square also removes
        // that side's corresponding castling right.
        // ----------------------------------
        if (undo.captured_piece != Piece::None && type_of(undo.captured_piece) == PieceType::Rook) {
            switch (move.to) {
                case Square::H1:
                    remove_castling_right(
                        CastlingRights::WhiteKingSide
                    );
                    break;

                case Square::A1:
                    remove_castling_right(
                        CastlingRights::WhiteQueenSide
                    );
                    break;

                case Square::H8:
                    remove_castling_right(
                        CastlingRights::BlackKingSide
                    );
                    break;

                case Square::A8:
                    remove_castling_right(
                        CastlingRights::BlackQueenSide
                    );
                    break;

                default:
                    break;
            }
        }

        return undo;
    }

    void Position::unmake_move(const Move &move, const UndoState &undo) noexcept {
        assert(is_valid(move.from));
        assert(is_valid(move.to));

        const Piece current_piece = piece_at(move.to);

        assert(current_piece != Piece::None);

        const Color side = color_of(current_piece);

        // ---------------------------------------------------------
        // Move the main piece back
        // ---------------------------------------------------------
        if (is_promotion(move)) {
            remove_piece(side, move.promotion, move.to);

            set_piece(side, PieceType::Pawn, move.from);
        } else {
            const PieceType type = type_of(current_piece);

            remove_piece(side, type, move.to);

            set_piece(side, type, move.from);
        }

        // ---------------------------------------------------------
        // Undo king-side castling rook move
        // ---------------------------------------------------------
        if (move.flag == MoveFlag::KingCastle) {
            if (side == Color::White) {
                remove_piece(Color::White, PieceType::Rook, Square::F1);

                set_piece(Color::White, PieceType::Rook, Square::H1);
            } else {
                remove_piece(Color::Black, PieceType::Rook, Square::F8);

                set_piece(Color::Black, PieceType::Rook, Square::H8);
            }
        }

        // ---------------------------------------------------------
        // Undo queen-side castling rook move
        // ---------------------------------------------------------
        if (move.flag == MoveFlag::QueenCastle) {
            if (side == Color::White) {
                remove_piece(Color::White, PieceType::Rook, Square::D1);

                set_piece(Color::White, PieceType::Rook, Square::A1);
            } else {
                remove_piece(Color::Black, PieceType::Rook, Square::D8);

                set_piece(Color::Black, PieceType::Rook, Square::A8);
            }
        }

        // ---------------------------------------------------------
        // Restore normal captured piece
        // ---------------------------------------------------------
        if (move.flag == MoveFlag::Capture || move.flag == MoveFlag::PromotionCapture) {
            assert(undo.captured_piece != Piece::None);

            set_piece(color_of(undo.captured_piece), type_of(undo.captured_piece), move.to);
        }

        // ---------------------------------------------------------
        // Restore en passant captured pawns
        // ---------------------------------------------------------
        if (move.flag == MoveFlag::EnPassant) {
            assert(undo.captured_piece != Piece::None);

            assert(type_of(undo.captured_piece) == PieceType::Pawn);

            const Square captured_square = (side == Color::White)
                                               ? static_cast<Square>(to_index(move.to) - 8)
                                               : static_cast<Square>(to_index(move.to) + 8);

            set_piece(color_of(undo.captured_piece), PieceType::Pawn, captured_square);
        }

        // ---------------------------------------------------------
        // Restore castling rights
        // ---------------------------------------------------------
        set_castling_rights(undo.castling_rights);

        // ---------------------------------------------------------
        // Restore previous en passant square
        // ---------------------------------------------------------
        if (undo.en_passant_square == Square::None) {
            clear_en_passant_square();
        } else {
            set_en_passant_square(undo.en_passant_square);
        }
    }
}
