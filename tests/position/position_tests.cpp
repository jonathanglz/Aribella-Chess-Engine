#include <gtest/gtest.h>

#include "../../src/position/position.h"

using namespace Aribella;

TEST(PositionTest, StartingPosition) {
    Position position;
    position.set_start_position();

    EXPECT_EQ(position.side_to_move(), Color::White);

    EXPECT_EQ(popcount(position.pieces(Color::White, PieceType::Pawn)), 8);
    EXPECT_EQ(popcount(position.pieces(Color::Black, PieceType::Pawn)), 8);

    EXPECT_EQ(popcount(position.occupancy(Color::White)), 16);
    EXPECT_EQ(popcount(position.occupancy(Color::Black)), 16);

    EXPECT_EQ(popcount(position.occupancy()), 32);

    EXPECT_EQ(position.piece_at(Square::E1), Piece::WhiteKing);
    EXPECT_EQ(position.piece_at(Square::E8), Piece::BlackKing);
}

TEST(PositionTest, ClearPosition) {
    Position position;
    position.set_start_position();

    EXPECT_EQ(popcount(position.occupancy()), 32);

    position.clear();

    EXPECT_EQ(popcount(position.occupancy()), 0);
    EXPECT_EQ(position.piece_at(Square::E1), Piece::None);
}

TEST(PositionTest, SetAndRemovePiece) {
    Position position;
    position.set_piece(Color::White, PieceType::Pawn, Square::E2);

    EXPECT_EQ(popcount(position.occupancy(Color::White)), 1);

    position.set_piece(Color::Black, PieceType::Pawn, Square::E7);

    EXPECT_EQ(popcount(position.occupancy(Color::Black)), 1);
    EXPECT_EQ(popcount(position.occupancy()), 2);

    position.remove_piece(Color::White, PieceType::Pawn, Square::E2);

    EXPECT_EQ(popcount(position.occupancy(Color::White)), 0);
}

TEST(PositionTest, PieceAtSquare) {
    Position position;
    position.set_start_position();

    EXPECT_EQ(position.piece_at(Square::E1), Piece::WhiteKing);
    EXPECT_EQ(position.piece_at(Square::E2), Piece::WhitePawn);

    EXPECT_EQ(position.piece_at(Square::E4), Piece::None);

    EXPECT_EQ(position.piece_at(Square::A8), Piece::BlackRook);
    EXPECT_EQ(position.piece_at(Square::F8), Piece::BlackBishop);
}

TEST(PositionTest, ColorAtSquare) {
    Position position;
    position.set_start_position();

    EXPECT_EQ(position.color_at(Square::E1), Color::White);
    EXPECT_EQ(position.color_at(Square::E8), Color::Black);
}

TEST(PositionTest, OccupancyByColor) {
    Position position;
    position.set_start_position();

    EXPECT_EQ(popcount(position.occupancy(Color::White)), 16);
    EXPECT_EQ(popcount(position.occupancy(Color::Black)), 16);
}

TEST(PositionTest, CombinedOccupancy) {
    Position position;
    position.set_start_position();

    EXPECT_EQ(popcount(position.occupancy()), 32);

    position.clear();

    EXPECT_EQ(popcount(position.occupancy()), 0);
}

TEST(PositionTest, ParseStartingPositionFen) {
    Position position;

    const bool success = position.set_from_fen(
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR "
        "w KQkq - 0 1"
    );

    EXPECT_TRUE(success);

    EXPECT_EQ(position.side_to_move(), Color::White);

    EXPECT_EQ(popcount(position.occupancy(Color::White)), 16);
    EXPECT_EQ(popcount(position.occupancy(Color::Black)), 16);
    EXPECT_EQ(popcount(position.occupancy()), 32);

    EXPECT_EQ(popcount( position.pieces(Color::White, PieceType::Pawn)), 8);

    EXPECT_EQ(popcount(position.pieces(Color::Black, PieceType::Pawn)), 8);

    EXPECT_TRUE(position.can_castle(CastlingRights::WhiteKingSide));
    EXPECT_TRUE(position.can_castle(CastlingRights::WhiteQueenSide));
    EXPECT_TRUE(position.can_castle(CastlingRights::BlackKingSide));
    EXPECT_TRUE(position.can_castle(CastlingRights::BlackQueenSide));

    EXPECT_EQ(position.en_passant_square(), Square::None);

    EXPECT_EQ(position.halfmove_clock(), 0);
    EXPECT_EQ(position.fullmove_number(), 1);
}

TEST(PositionTest, ParseCustomFen) {
    Position position;

    ASSERT_TRUE(position.set_from_fen(
        "8/8/3k4/8/4P3/8/8/4K3 b - - 7 23"
    ));

    EXPECT_TRUE(position.is_occupied(Square::D6));
    EXPECT_TRUE(position.is_occupied(Square::E4));
    EXPECT_TRUE(position.is_occupied(Square::E1));

    EXPECT_EQ(position.pieces(Color::Black, PieceType::King), square_bb(Square::D6));

    EXPECT_EQ(position.pieces(Color::White, PieceType::Pawn), square_bb(Square::E4));

    EXPECT_EQ(position.pieces(Color::White, PieceType::King), square_bb(Square::E1));

    EXPECT_EQ(position.side_to_move(), Color::Black);
    EXPECT_EQ(position.halfmove_clock(), 7);
    EXPECT_EQ(position.fullmove_number(), 23);
}

TEST(PositionTest, RejectInvalidFen) {
    Position position;

    EXPECT_FALSE(position.set_from_fen("8/8/8/8/8/8/8/8"));

    EXPECT_FALSE(position.set_from_fen("8/8/8/8/8/8/8/8 x - - 0 1"));

    EXPECT_FALSE(position.set_from_fen("9/8/8/8/8/8/8/8 w - - 0 1"));

    EXPECT_FALSE(position.set_from_fen("8/8/8/8/8/8/8/4X3 w - - 0 1"));
}

TEST(PositionTest, ParseSideToMove) {
    Position position;

    ASSERT_TRUE(position.set_from_fen("8/8/8/8/8/8/8/8 w - - 0 1"));

    EXPECT_EQ(position.side_to_move(), Color::White);

    ASSERT_TRUE(position.set_from_fen("8/8/8/8/8/8/8/8 b - - 0 1"));

    EXPECT_EQ(position.side_to_move(), Color::Black);
}

TEST(PositionTest, ParseCastlingRights) {
    Position position;

    ASSERT_TRUE(position.set_from_fen("r3k2r/8/8/8/8/8/8/R3K2R w Kq - 0 1"));

    EXPECT_TRUE(position.can_castle(CastlingRights::WhiteKingSide));

    EXPECT_FALSE(position.can_castle(CastlingRights::WhiteQueenSide));

    EXPECT_FALSE(position.can_castle(CastlingRights::BlackKingSide));

    EXPECT_TRUE(position.can_castle(CastlingRights::BlackQueenSide));
}

TEST(PositionTest, ParseEnPassantSquare) {
    Position position;

    ASSERT_TRUE(position.set_from_fen("4k3/8/8/3pP3/8/8/8/4K3 w - d6 0 1"));

    EXPECT_EQ(position.en_passant_square(), Square::D6);

    ASSERT_TRUE(position.set_from_fen("4k3/8/8/8/8/8/8/4K3 w - - 0 1"));

    EXPECT_EQ(position.en_passant_square(), Square::None);
}

TEST(PositionTest, ParseMoveCounters) {
    Position position;

    ASSERT_TRUE(position.set_from_fen("8/8/8/8/8/8/8/8 w - - 37 84"));

    EXPECT_EQ(position.halfmove_clock(), 37);
    EXPECT_EQ(position.fullmove_number(), 84);
}
