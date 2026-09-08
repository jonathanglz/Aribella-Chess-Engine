#include "movegen.h"
#include <cassert>

namespace Aribella {
    namespace {
        void add_moves_from_bitboard(MoveList &moves, const Position &position,
                                     Square from, Bitboard destinations) noexcept {
            assert(is_valid(from));

            while (destinations) {
                const Square to = pop_lsb(destinations);

                const MoveFlag flag = position.is_occupied(to)
                                          ? MoveFlag::Capture
                                          : MoveFlag::Quiet;

                moves.push_back(
                    Move{
                        .from = from,
                        .to = to,
                        .flag = flag,
                        .promotion = PieceType::None
                    }
                );
            }
        }

        constexpr std::array<PieceType, 4> PromotionPieces{
            PieceType::Queen,
            PieceType::Rook,
            PieceType::Bishop,
            PieceType::Knight
        };

        void add_promotion_moves(MoveList &moves, Square from, Square to, MoveFlag flag) noexcept {
            for (const PieceType promotion: PromotionPieces) {
                moves.push_back(
                    Move{
                        .from = from,
                        .to = to,
                        .flag = flag,
                        .promotion = promotion
                    }
                );
            }
        }
    }

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

    Bitboard knight_moves(const Position &position, Square sq, Color side) noexcept {
        assert(is_valid(sq));
        assert(is_valid(side));

        const Bitboard attacks = knight_attacks(sq);

        return attacks & ~position.occupancy(side);
    }

    Bitboard king_moves(const Position &position, Square sq, Color side) noexcept {
        assert(is_valid(sq));
        assert(is_valid(side));

        const Bitboard attacks = king_attacks(sq);

        return attacks & ~position.occupancy(side);
    }

    void generate_rook_moves(MoveList &moves, const Position &position, Square sq, Color side) noexcept {
        assert(is_valid(side));
        assert(is_valid(sq));

        const Bitboard destinations = rook_moves(position, sq, side);

        add_moves_from_bitboard(moves, position, sq, destinations);
    }

    void generate_bishop_moves(MoveList &moves, const Position &position, Square sq, Color side) noexcept {
        assert(is_valid(side));
        assert(is_valid(sq));

        const Bitboard destinations = bishop_moves(position, sq, side);

        add_moves_from_bitboard(moves, position, sq, destinations);
    }

    void generate_queen_moves(MoveList &moves, const Position &position, Square sq, Color side) noexcept {
        assert(is_valid(side));
        assert(is_valid(sq));

        const Bitboard destinations = queen_moves(position, sq, side);

        add_moves_from_bitboard(moves, position, sq, destinations);
    }

    void generate_knight_moves(MoveList &moves, const Position &position, Square sq, Color side) noexcept {
        assert(is_valid(side));
        assert(is_valid(sq));

        const Bitboard destinations = knight_moves(position, sq, side);

        add_moves_from_bitboard(moves, position, sq, destinations);
    }

    void generate_king_moves(MoveList &moves, const Position &position, Square sq, Color side) noexcept {
        assert(is_valid(side));
        assert(is_valid(sq));

        const Bitboard destinations = king_moves(position, sq, side);

        add_moves_from_bitboard(moves, position, sq, destinations);
    }

    void generate_pawn_moves(MoveList &moves, const Position &position, Color side) noexcept {
        assert(is_valid(side));

        Bitboard pawns = position.pieces(side, PieceType::Pawn);

        while (pawns) {
            const Square from = pop_lsb(pawns);

            const Bitboard from_bb = square_bb(from);

            Bitboard single = (side == Color::White)
                                  ? shift_north(from_bb)
                                  : shift_south(from_bb);

            single &= ~position.occupancy();

            if (single) {
                const Square to = lsb(single);

                const bool promotion = (side == Color::White)
                                           ? rank_of(to) == Rank::R8
                                           : rank_of(to) == Rank::R1;

                if (promotion) {
                    add_promotion_moves(moves, from, to, MoveFlag::Promotion);
                } else {
                    moves.push_back(
                        Move{
                            .from = from,
                            .to = to,
                            .flag = MoveFlag::Quiet,
                            .promotion = PieceType::None
                        }
                    );
                }
            }

            const bool on_starting_rank = (side == Color::White)
                                              ? rank_of(from) == Rank::R2
                                              : rank_of(from) == Rank::R7;

            if (on_starting_rank && single) {
                Bitboard double_push = (side == Color::White)
                                           ? shift_north(single)
                                           : shift_south(single);

                double_push &= ~position.occupancy();

                if (double_push) {
                    moves.push_back(
                        Move{
                            .from = from,
                            .to = lsb(double_push),
                            .flag = MoveFlag::DoublePawnPush,
                            .promotion = PieceType::None
                        }
                    );
                }
            }

            Bitboard captures = pawn_attacks(side, from_bb) & position.occupancy(~side);

            while (captures) {
                const Square to = pop_lsb(captures);

                const bool promotion = (side == Color::White)
                                           ? rank_of(to) == Rank::R8
                                           : rank_of(to) == Rank::R1;

                if (promotion) {
                    add_promotion_moves(
                        moves,
                        from,
                        to,
                        MoveFlag::PromotionCapture
                    );
                } else {
                    moves.push_back(
                        Move{
                            .from = from,
                            .to = to,
                            .flag = MoveFlag::Capture,
                            .promotion = PieceType::None
                        }
                    );
                }
            }

            const Square ep = position.en_passant_square();

            if (ep != Square::None) {
                const Bitboard ep_target = square_bb(ep);

                if (pawn_attacks(side, from_bb) & ep_target) {
                    moves.push_back(
                        Move{
                            .from = from,
                            .to = ep,
                            .flag = MoveFlag::EnPassant,
                            .promotion = PieceType::None
                        }
                    );
                }
            }
        }
    }

    void generate_castling_moves(MoveList &moves, const Position &position, Color side) noexcept {
        assert(is_valid(side));

        const Color enemy = ~side;

        if (side == Color::White) {
            if (position.can_castle(CastlingRights::WhiteKingSide)
                && position.piece_at(Square::E1) == Piece::WhiteKing
                && position.piece_at(Square::H1) == Piece::WhiteRook
                && !position.is_occupied(Square::F1)
                && !position.is_occupied(Square::G1)
                && !in_check(position, Color::White)
                && !is_square_attacked(position, Square::F1, enemy)
                && !is_square_attacked(position, Square::G1, enemy)
            ) {
                moves.push_back(
                    Move{
                        .from = Square::E1,
                        .to = Square::G1,
                        .flag = MoveFlag::KingCastle,
                        .promotion = PieceType::None
                    }
                );
            }

            if (position.can_castle(CastlingRights::WhiteQueenSide)
                && position.piece_at(Square::E1) == Piece::WhiteKing
                && position.piece_at(Square::A1) == Piece::WhiteRook
                && !position.is_occupied(Square::D1)
                && !position.is_occupied(Square::C1)
                && !position.is_occupied(Square::B1)
                && !in_check(position, Color::White)
                && !is_square_attacked(position, Square::D1, enemy)
                && !is_square_attacked(position, Square::C1, enemy)
            ) {
                moves.push_back(
                    Move{
                        .from = Square::E1,
                        .to = Square::C1,
                        .flag = MoveFlag::QueenCastle,
                        .promotion = PieceType::None
                    }
                );
            }

            return;
        }

        if (position.can_castle(CastlingRights::BlackKingSide)
            && position.piece_at(Square::E8) == Piece::BlackKing
            && position.piece_at(Square::H8) == Piece::BlackRook
            && !position.is_occupied(Square::F8)
            && !position.is_occupied(Square::G8)
            && !in_check(position, Color::Black)
            && !is_square_attacked(position, Square::F8, enemy)
            && !is_square_attacked(position, Square::G8, enemy)
        ) {
            moves.push_back(
                Move{
                    .from = Square::E8,
                    .to = Square::G8,
                    .flag = MoveFlag::KingCastle,
                    .promotion = PieceType::None
                }
            );
        }

        if (position.can_castle(CastlingRights::BlackQueenSide)
            && position.piece_at(Square::E8) == Piece::BlackKing
            && position.piece_at(Square::A8) == Piece::BlackRook
            && !position.is_occupied(Square::D8)
            && !position.is_occupied(Square::C8)
            && !position.is_occupied(Square::B8)
            && !in_check(position, Color::Black)
            && !is_square_attacked(position, Square::D8, enemy)
            && !is_square_attacked(position, Square::C8, enemy)
        ) {
            moves.push_back(
                Move{
                    .from = Square::E8,
                    .to = Square::C8,
                    .flag = MoveFlag::QueenCastle,
                    .promotion = PieceType::None
                }
            );
        }
    }

    MoveList generate_pseudo_legal_moves(const Position &position, Color side) noexcept {
        assert(is_valid(side));

        MoveList moves;

        generate_pawn_moves(moves, position, side);

        Bitboard knights = position.pieces(side, PieceType::Knight);

        while (knights) {
            const Square sq = pop_lsb(knights);

            generate_knight_moves(moves, position, sq, side);
        }

        Bitboard rooks = position.pieces(side, PieceType::Rook);

        while (rooks) {
            const Square sq = pop_lsb(rooks);

            generate_rook_moves(moves, position, sq, side);
        }

        Bitboard bishops = position.pieces(side, PieceType::Bishop);

        while (bishops) {
            const Square sq = pop_lsb(bishops);

            generate_bishop_moves(moves, position, sq, side);
        }

        Bitboard queens = position.pieces(side, PieceType::Queen);

        while (queens) {
            const Square sq = pop_lsb(queens);

            generate_queen_moves(moves, position, sq, side);
        }

        Bitboard kings = position.pieces(side, PieceType::King);

        while (kings) {
            const Square sq = pop_lsb(kings);

            generate_king_moves(moves, position, sq, side);
            generate_castling_moves(moves, position, side);
        }

        return moves;
    }

    bool is_square_attacked(const Position &position, Square sq, Color by_side) noexcept {
        assert(is_valid(by_side));
        assert(is_valid(sq));

        const Bitboard target = square_bb(sq);

        const Bitboard enemy_pawns = position.pieces(by_side, PieceType::Pawn);

        if (pawn_attacks(by_side, enemy_pawns) & target) {
            return true;
        }

        const Bitboard enemy_knights = position.pieces(by_side, PieceType::Knight);

        if (knight_attacks(sq) & enemy_knights) {
            return true;
        }

        const Bitboard enemy_king = position.pieces(by_side, PieceType::King);

        if (king_attacks(sq) & enemy_king) {
            return true;
        }

        const Bitboard occupied = position.occupancy();

        const Bitboard diagonal_attackers = position.pieces(by_side, PieceType::Bishop)
                                            | position.pieces(by_side, PieceType::Queen);

        if (bishop_attacks(sq, occupied) & diagonal_attackers) {
            return true;
        }

        const Bitboard orthogonal_attackers = position.pieces(by_side, PieceType::Rook)
                                              | position.pieces(by_side, PieceType::Queen);

        if (rook_attacks(sq, occupied) & orthogonal_attackers) {
            return true;
        }

        return false;
    }

    bool in_check(const Position &position, Color side) noexcept {
        assert(is_valid(side));

        const Bitboard king = position.pieces(side, PieceType::King);

        assert(popcount(king) == 1);

        const Square king_square = lsb(king);

        return is_square_attacked(position, king_square, ~side);
    }

    MoveList generate_legal_moves(Position &position, Color side) noexcept {
        assert(is_valid(side));

        const MoveList pseudo_legal = generate_pseudo_legal_moves(position, side);

        MoveList legal_moves;

        for (std::size_t i = 0; i < pseudo_legal.size(); ++i) {
            const Move move = pseudo_legal[i];

            const UndoState undo = position.make_move(move);

            const bool legal = !in_check(position, side);

            position.unmake_move(move, undo);

            if (legal) {
                legal_moves.push_back(move);
            }
        }

        return legal_moves;
    }
}
