#include "position.h"

#include <charconv>
#include <string_view>
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
        return
                occupancy(Color::White) |
                occupancy(Color::Black);
    }

    void Position::set_piece(Color color, PieceType type, Square sq) noexcept {
        assert(is_valid(color));
        assert(is_valid(type));
        assert(is_valid(sq));

        set_bit(pieces_[to_index(color)][to_index(type)],
                sq);
    }

    void Position::remove_piece(Color color, PieceType type, Square sq) noexcept {
        assert(is_valid(color));
        assert(is_valid(type));
        assert(is_valid(sq));

        clear_bit(pieces_[to_index(color)][to_index(type)],
                  sq);
    }

    void Position::clear() noexcept {
        for (auto &color_pieces: pieces_) {
            for (Bitboard &bb: color_pieces) {
                bb = Bitboard{0};
            }
        }

        castling_rights_ =
                CastlingRights::None;

        en_passant_square_ =
                Square::None;
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
        return
                Piece::None;
    }

    bool Position::is_occupied(Square sq) const noexcept {
        assert(is_valid(sq));

        return
                is_set(occupancy(),
                       sq);
    }

    Color Position::color_at(Square sq) const noexcept {
        assert(is_valid(sq));
        assert(is_occupied(sq));

        if (is_set(occupancy(Color::White), sq)) {
            return
                    Color::White;
        }

        return
                Color::Black;
    }

    Square Position::en_passant_square() const noexcept {
        return
                en_passant_square_;
    }

    void Position::set_en_passant_square(Square sq) noexcept {
        assert(is_valid(sq));

        en_passant_square_ =
                sq;
    }

    void Position::clear_en_passant_square() noexcept {
        en_passant_square_ =
                Square::None;
    }

    CastlingRights Position::castling_rights() const noexcept {
        return
                castling_rights_;
    }

    bool Position::can_castle(CastlingRights right) const noexcept {
        return
                has_castling_rights(castling_rights_,
                                    right);
    }

    void Position::set_castling_rights(CastlingRights right) noexcept {
        castling_rights_ =
                right;
    }

    void Position::remove_castling_right(CastlingRights right) noexcept {
        castling_rights_ =
                static_cast<CastlingRights>(static_cast<u8>(castling_rights_) &
                                            ~static_cast<u8>(right)
                );
    }

    Piece Position::remove_piece_at(Square sq) noexcept {
        assert(is_valid(sq));

        const Piece piece =
                piece_at(sq);

        if (piece == Piece::None) {
            return Piece::None;
        }

        remove_piece(color_of(piece),
                     type_of(piece),
                     sq);

        return
                piece;
    }

    UndoState Position::make_move(const Move &move) noexcept {
        assert(is_valid(move.from));
        assert(is_valid(move.to));

        const Piece moving_piece =
                piece_at(move.from);

        assert(moving_piece != Piece::None);

        const Color side =
                color_of(moving_piece);

        const PieceType moving_type =
                type_of(moving_piece);

        // Remember the old state before this move might destroy or overwrite

        UndoState undo{
            .captured_piece = Piece::None,
            .castling_rights = castling_rights_,
            .en_passant_square = en_passant_square_,
            .halfmove_clock = halfmove_clock_,
            .fullmove_number = fullmove_number_,
            .side_to_move = side_to_move_,
            .key = key_
        };

        // ---------------------------------------------------------
        // Zobrist: remove old castling state
        // ---------------------------------------------------------

        key_ ^=
                Zobrist::ZobristTables.castling[static_cast<std::size_t>(castling_rights_)];

        // ---------------------------------------------------------
        // Zobrist: remove old en passant state
        // ---------------------------------------------------------

        if (en_passant_square_ != Square::None) {
            key_ ^=
                    Zobrist::ZobristTables.en_passant_file[to_index(file_of(en_passant_square_))];
        }

        // --------------------------------------------------------
        // Zobrist: remove moving piece from source square
        // ---------------------------------------------------------

        key_ ^=
                Zobrist::ZobristTables.piece_square[to_index(moving_piece)][to_index(move.from)];

        // ---------------------------------------------------------
        // Old en passant square expires
        // ---------------------------------------------------------

        clear_en_passant_square();

        // ---------------------------------------------------------
        // Normal Capture / promotion capture
        // ---------------------------------------------------------

        if (move.flag == MoveFlag::Capture || move.flag == MoveFlag::PromotionCapture) {
            const Piece captured_piece =
                    piece_at(move.to);

            assert(captured_piece != Piece::None);

            // XOR to remove captured piece
            key_ ^=
                    Zobrist::ZobristTables.piece_square[to_index(captured_piece)][to_index(move.to)];

            undo.captured_piece =
                    remove_piece_at(move.to);

            assert(undo.captured_piece != Piece::None);
        }

        // ---------------------------------------------------------
        // En passant captures
        // ---------------------------------------------------------

        if (move.flag == MoveFlag::EnPassant) {
            assert(moving_type == PieceType::Pawn);

            const Square captured_square =
                    side == Color::White
                        ? static_cast<Square>(to_index(move.to) - 8)
                        : static_cast<Square>(to_index(move.to) + 8);

            const Piece captured_piece =
                    piece_at(captured_square);

            assert(captured_piece != Piece::None);

            assert(type_of(captured_piece) == PieceType::Pawn);

            key_ ^=
                    Zobrist::ZobristTables.piece_square[to_index(captured_piece)][to_index(captured_square)];

            undo.captured_piece =
                    remove_piece_at(captured_square);

            assert(undo.captured_piece != Piece::None);
        }

        // ---------------------------------------------------------
        // Remove moving piece from starting square
        // ---------------------------------------------------------

        remove_piece(side,
                     moving_type,
                     move.from);

        // ---------------------------------------------------------
        // Place piece on destination square
        // ---------------------------------------------------------

        if (is_promotion(move)) {
            assert(moving_type == PieceType::Pawn);

            assert(
                move.promotion == PieceType::Queen ||
                move.promotion == PieceType::Rook ||
                move.promotion == PieceType::Bishop ||
                move.promotion == PieceType::Knight
            );

            const Piece promoted_piece =
                    make_piece(side,
                               move.promotion);

            // promotion move
            set_piece(side,
                      move.promotion,
                      move.to);

            // Zobrist: add promoted piece on destination.
            key_ ^=
                    Zobrist::ZobristTables.piece_square[to_index(promoted_piece)][to_index(move.to)];
        } else {
            // normal move, capture, double push, en passant, castling king move
            set_piece(side,
                      moving_type,
                      move.to);

            // Zobrist: add normal moving piece on destination.
            key_ ^=
                    Zobrist::ZobristTables.piece_square[to_index(moving_piece)][to_index(move.to)];
        }

        // ---------------------------------------------------------
        // Double pawn push
        // ---------------------------------------------------------

        if (move.flag == MoveFlag::DoublePawnPush) {
            assert(moving_type == PieceType::Pawn);

            // create new en passant square
            const Square ep =
                    side == Color::White
                        ? static_cast<Square>(to_index(move.from) + 8)
                        : static_cast<Square>(to_index(move.from) - 8);

            set_en_passant_square(ep);
        }

        // ---------------------------------------------------------
        // King side castling
        // ---------------------------------------------------------

        if (move.flag == MoveFlag::KingCastle) {
            assert(moving_type == PieceType::King);

            if (side == Color::White) {
                key_ ^=
                        Zobrist::ZobristTables.piece_square[to_index(Piece::WhiteRook)][to_index(Square::H1)];

                key_ ^=
                        Zobrist::ZobristTables.piece_square[to_index(Piece::WhiteRook)][to_index(Square::F1)];

                remove_piece(Color::White,
                             PieceType::Rook,
                             Square::H1);

                set_piece(Color::White,
                          PieceType::Rook,
                          Square::F1);
            } else {
                key_ ^=
                        Zobrist::ZobristTables.piece_square[to_index(Piece::BlackRook)][to_index(Square::H8)];

                key_ ^=
                        Zobrist::ZobristTables.piece_square[to_index(Piece::BlackRook)][to_index(Square::F8)];

                remove_piece(Color::Black,
                             PieceType::Rook,
                             Square::H8);

                set_piece(Color::Black,
                          PieceType::Rook,
                          Square::F8);
            }
        }

        /// ---------------------------------------------------------
        // Queen side castling
        // ---------------------------------------------------------

        if (move.flag == MoveFlag::QueenCastle) {
            assert(moving_type == PieceType::King);

            if (side == Color::White) {
                key_ ^=
                        Zobrist::ZobristTables.piece_square[to_index(Piece::WhiteRook)][to_index(Square::A1)];

                key_ ^=
                        Zobrist::ZobristTables.piece_square[to_index(Piece::WhiteRook)][to_index(Square::D1)];

                remove_piece(Color::White,
                             PieceType::Rook,
                             Square::A1);

                set_piece(Color::White,
                          PieceType::Rook,
                          Square::D1);
            } else {
                key_ ^=
                        Zobrist::ZobristTables.piece_square[to_index(Piece::BlackRook)][to_index(Square::A8)];

                key_ ^=
                        Zobrist::ZobristTables.piece_square[to_index(Piece::BlackRook)][to_index(Square::D8)];

                remove_piece(Color::Black,
                             PieceType::Rook,
                             Square::A8);

                set_piece(Color::Black,
                          PieceType::Rook,
                          Square::D8);
            }
        }

        // ---------------------------------------------------------
        // Moving a king permanently removes both castling rights
        // ---------------------------------------------------------

        if (moving_type == PieceType::King) {
            if (side == Color::White) {
                remove_castling_right(CastlingRights::WhiteKingSide);

                remove_castling_right(CastlingRights::WhiteQueenSide);
            } else {
                remove_castling_right(CastlingRights::BlackKingSide);

                remove_castling_right(CastlingRights::BlackQueenSide);
            }
        }

        // ---------------------------------------------------------
        // Moving a rook from its original square removes
        // the corresponding castling right.
        // ---------------------------------------------------------

        if (moving_type == PieceType::Rook) {
            switch (move.from) {
                case Square::H1:
                    remove_castling_right(CastlingRights::WhiteKingSide);
                    break;

                case Square::A1:
                    remove_castling_right(CastlingRights::WhiteQueenSide);
                    break;

                case Square::H8:
                    remove_castling_right(CastlingRights::BlackKingSide);
                    break;

                case Square::A8:
                    remove_castling_right(CastlingRights::BlackQueenSide);
                    break;

                default:
                    break;
            }
        }

        // ---------------------------------------------------------
        // Capturing a rook on its original square also removes
        // that side's corresponding castling right.
        // ---------------------------------------------------------

        if (undo.captured_piece != Piece::None &&
            type_of(undo.captured_piece) == PieceType::Rook) {
            switch (move.to) {
                case Square::H1:
                    remove_castling_right(CastlingRights::WhiteKingSide);
                    break;

                case Square::A1:
                    remove_castling_right(CastlingRights::WhiteQueenSide);
                    break;

                case Square::H8:
                    remove_castling_right(CastlingRights::BlackKingSide);
                    break;

                case Square::A8:
                    remove_castling_right(CastlingRights::BlackQueenSide);
                    break;

                default:
                    break;
            }
        }

        // ---------------------------------------------------------
        // Zobrist: add NEW castling state
        // ---------------------------------------------------------

        key_ ^=
                Zobrist::ZobristTables.castling[static_cast<std::size_t>(castling_rights_)];


        // ---------------------------------------------------------
        // Zobrist: add NEW en passant state
        // ---------------------------------------------------------

        if (en_passant_square_ != Square::None) {
            key_ ^=
                    Zobrist::ZobristTables.en_passant_file[to_index(file_of(en_passant_square_))];
        }


        // ---------------------------------------------------------
        // Update halfmove clock
        // ---------------------------------------------------------

        if (moving_type == PieceType::Pawn ||
            undo.captured_piece != Piece::None) {
            halfmove_clock_ = 0;
        } else {
            ++halfmove_clock_;
        }


        // ---------------------------------------------------------
        // Update fullmove number
        // ---------------------------------------------------------

        if (side == Color::Black) {
            ++fullmove_number_;
        }

        // ---------------------------------------------------------
        // Switch side to move
        // ---------------------------------------------------------

        side_to_move_ =
                ~side;

        // ---------------------------------------------------------
        // Zobrist: toggle side to move
        // ---------------------------------------------------------

        key_ ^=
                Zobrist::ZobristTables.side_to_move;

        return undo;
    }

    void Position::unmake_move(const Move &move, const UndoState &undo) noexcept {
        assert(is_valid(move.from));
        assert(is_valid(move.to));

        const Piece current_piece =
                piece_at(move.to);

        assert(current_piece != Piece::None);

        const Color side =
                color_of(current_piece);

        // ---------------------------------------------------------
        // Move the main piece back
        // ---------------------------------------------------------
        if (is_promotion(move)) {
            remove_piece(side,
                         move.promotion,
                         move.to);

            set_piece(side,
                      PieceType::Pawn,
                      move.from);
        } else {
            const PieceType type =
                    type_of(current_piece);

            remove_piece(side,
                         type,
                         move.to);

            set_piece(side,
                      type,
                      move.from);
        }

        // ---------------------------------------------------------
        // Undo king-side castling rook move
        // ---------------------------------------------------------
        if (move.flag == MoveFlag::KingCastle) {
            if (side == Color::White) {
                remove_piece(Color::White,
                             PieceType::Rook,
                             Square::F1);

                set_piece(Color::White,
                          PieceType::Rook,
                          Square::H1);
            } else {
                remove_piece(Color::Black,
                             PieceType::Rook,
                             Square::F8);

                set_piece(Color::Black,
                          PieceType::Rook,
                          Square::H8);
            }
        }

        // ---------------------------------------------------------
        // Undo queen-side castling rook move
        // ---------------------------------------------------------
        if (move.flag == MoveFlag::QueenCastle) {
            if (side == Color::White) {
                remove_piece(Color::White,
                             PieceType::Rook,
                             Square::D1);

                set_piece(Color::White,
                          PieceType::Rook,
                          Square::A1);
            } else {
                remove_piece(Color::Black,
                             PieceType::Rook,
                             Square::D8);

                set_piece(Color::Black,
                          PieceType::Rook,
                          Square::A8);
            }
        }

        // ---------------------------------------------------------
        // Restore normal captured piece
        // ---------------------------------------------------------
        if (move.flag == MoveFlag::Capture ||
            move.flag == MoveFlag::PromotionCapture) {
            assert(undo.captured_piece != Piece::None);

            set_piece(color_of(undo.captured_piece),
                      type_of(undo.captured_piece),
                      move.to);
        }

        // ---------------------------------------------------------
        // Restore en passant captured pawns
        // ---------------------------------------------------------
        if (move.flag == MoveFlag::EnPassant) {
            assert(undo.captured_piece != Piece::None);

            assert(type_of(undo.captured_piece) == PieceType::Pawn);

            const Square captured_square =
                    side == Color::White
                        ? static_cast<Square>(to_index(move.to) - 8)
                        : static_cast<Square>(to_index(move.to) + 8);

            set_piece(color_of(undo.captured_piece),
                      PieceType::Pawn, captured_square);
        }

        castling_rights_ =
                undo.castling_rights;

        en_passant_square_ =
                undo.en_passant_square;

        halfmove_clock_ =
                undo.halfmove_clock;

        fullmove_number_ =
                undo.fullmove_number;

        side_to_move_ =
                undo.side_to_move;

        key_ =
                undo.key;
    }

    void Position::set_start_position() noexcept {
        const bool ok = set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        assert(ok);
    }

    bool Position::set_from_fen(std::string_view fen) noexcept {
        Position parsed;
        parsed.clear();

        std::string_view fields[6];

        std::size_t start = 0;

        for (std::size_t i = 0; i < 6; ++i) {
            const std::size_t end =
                    fen.find(' ', start);

            if (i < 5) {
                if (end == std::string_view::npos) {
                    return false;
                }

                fields[i] =
                        fen.substr(start, end - start);

                start = end + 1;
            } else {
                fields[i] =
                        fen.substr(start);
            }
        }

        // -----------------------------------------------------
        // 1. Piece placement
        // -----------------------------------------------------

        std::size_t rank = 7;
        std::size_t file = 0;

        for (const char c: fields[0]) {
            if (c == '/') {
                if (file != 8 || rank == 0) {
                    return false;
                }

                --rank;
                file = 0;

                continue;
            }

            if (c >= '1' && c <= '8') {
                file +=
                        static_cast<std::size_t>(
                            c - '0'
                        );

                if (file > 8) {
                    return false;
                }

                continue;
            }

            if (file >= 8) {
                return false;
            }

            Color color;
            PieceType type;

            switch (c) {
                case 'P':
                    color = Color::White;
                    type = PieceType::Pawn;
                    break;

                case 'N':
                    color = Color::White;
                    type = PieceType::Knight;
                    break;

                case 'B':
                    color = Color::White;
                    type = PieceType::Bishop;
                    break;

                case 'R':
                    color = Color::White;
                    type = PieceType::Rook;
                    break;

                case 'Q':
                    color = Color::White;
                    type = PieceType::Queen;
                    break;

                case 'K':
                    color = Color::White;
                    type = PieceType::King;
                    break;

                case 'p':
                    color = Color::Black;
                    type = PieceType::Pawn;
                    break;

                case 'n':
                    color = Color::Black;
                    type = PieceType::Knight;
                    break;

                case 'b':
                    color = Color::Black;
                    type = PieceType::Bishop;
                    break;

                case 'r':
                    color = Color::Black;
                    type = PieceType::Rook;
                    break;

                case 'q':
                    color = Color::Black;
                    type = PieceType::Queen;
                    break;

                case 'k':
                    color = Color::Black;
                    type = PieceType::King;
                    break;

                default:
                    return false;
            }

            const Square sq =
                    make_square(
                        static_cast<File>(file),
                        static_cast<Rank>(rank)
                    );

            parsed.set_piece(
                color,
                type,
                sq
            );

            ++file;
        }

        if (rank != 0 || file != 8) {
            return false;
        }

        // -----------------------------------------------------
        // 2. Side to move
        // -----------------------------------------------------

        if (fields[1] == "w") {
            parsed.side_to_move_ = Color::White;
        } else if (fields[1] == "b") {
            parsed.side_to_move_ = Color::Black;
        } else {
            return false;
        }

        // -----------------------------------------------------
        // 3. Castling rights
        // -----------------------------------------------------

        parsed.castling_rights_ =
                CastlingRights::None;

        if (fields[2] != "-") {
            for (const char c: fields[2]) {
                switch (c) {
                    case 'K':
                        parsed.castling_rights_ =
                                parsed.castling_rights_
                                | CastlingRights::WhiteKingSide;
                        break;

                    case 'Q':
                        parsed.castling_rights_ =
                                parsed.castling_rights_
                                | CastlingRights::WhiteQueenSide;
                        break;

                    case 'k':
                        parsed.castling_rights_ =
                                parsed.castling_rights_
                                | CastlingRights::BlackKingSide;
                        break;

                    case 'q':
                        parsed.castling_rights_ =
                                parsed.castling_rights_
                                | CastlingRights::BlackQueenSide;
                        break;

                    default:
                        return false;
                }
            }
        }

        // -----------------------------------------------------
        // 4. En passant target
        // -----------------------------------------------------

        if (fields[3] == "-") {
            parsed.en_passant_square_ =
                    Square::None;
        } else {
            if (fields[3].size() != 2) {
                return false;
            }

            const char file_char =
                    fields[3][0];

            const char rank_char =
                    fields[3][1];

            if (file_char < 'a'
                || file_char > 'h') {
                return false;
            }

            if (rank_char < '1'
                || rank_char > '8') {
                return false;
            }

            const File ep_file =
                    static_cast<File>(
                        file_char - 'a'
                    );

            const Rank ep_rank =
                    static_cast<Rank>(
                        rank_char - '1'
                    );

            parsed.en_passant_square_ =
                    make_square(
                        ep_file,
                        ep_rank
                    );
        }

        // -----------------------------------------------------
        // 5. Halfmove clock
        // -----------------------------------------------------

        {
            const char *begin =
                    fields[4].data();

            const char *end =
                    begin + fields[4].size();

            const auto result =
                    std::from_chars(
                        begin,
                        end,
                        parsed.halfmove_clock_
                    );

            if (result.ec != std::errc{}
                || result.ptr != end) {
                return false;
            }
        }

        // -----------------------------------------------------
        // 6. Fullmove number
        // -----------------------------------------------------

        {
            const char *begin =
                    fields[5].data();

            const char *end =
                    begin + fields[5].size();

            const auto result =
                    std::from_chars(
                        begin,
                        end,
                        parsed.fullmove_number_
                    );

            if (result.ec != std::errc{}
                || result.ptr != end) {
                return false;
            }
        }

        if (parsed.fullmove_number_ == 0) {
            return false;
        }

        parsed.recompute_key();
        *this = parsed;

        return true;
    }

    void Position::recompute_key() noexcept {
        Zobrist::Key key{0};

        for (std::size_t color = 0; color < to_index(Color::Count); ++color) {
            for (std::size_t type = 0; type < to_index(PieceType::Count); ++type) {
                Bitboard bb =
                        pieces_[color][type];

                while (bb) {
                    const Square sq =
                            pop_lsb(bb);

                    const Piece piece =
                            make_piece(static_cast<Color>(color),
                                       static_cast<PieceType>(type));

                    key ^=
                            Zobrist::ZobristTables.piece_square[to_index(piece)][to_index(sq)];
                }
            }
        }

        if (side_to_move_ == Color::Black) {
            key ^=
                    Zobrist::ZobristTables.side_to_move;
        }

        key ^=
                Zobrist::ZobristTables.castling[static_cast<std::size_t>(castling_rights_)];

        if (en_passant_square_ != Square::None) {
            key ^=
                    Zobrist::ZobristTables.en_passant_file[to_index(file_of(en_passant_square_))];
        }

        key_ = key;
    }
}
