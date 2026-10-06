#include <gtest/gtest.h>

#include "../../src/movegen/movegen.h"

using namespace Aribella;

namespace {
    bool contains_move(const MoveList &moves, const Move &target) {
        for (const Move &move: moves) {
            if (move == target) {
                return true;
            }
        }
        return false;
    }
}

TEST(MoveGenTest, StartingPositionLegalMoveCount) {
    Position position;
    position.set_start_position();

    const MoveList moves = generate_legal_moves(position, Color::White);

    EXPECT_EQ(moves.size(), 20U);
}

TEST(MoveGenTest, LegalMovesExcludeSelfCheck) {
    Position position;

    ASSERT_TRUE(position.set_from_fen("4r1k1/8/8/8/8/8/4R3/4K3 w - - 0 1"));

    const Move illegal{
        Square::E2,
        Square::D2,
        MoveFlag::Quiet,
        PieceType::None
    };

    const MoveList moves = generate_legal_moves(position, Color::White);

    EXPECT_FALSE(contains_move(moves, illegal));
}

TEST(MoveGenTest, PinnedPieceCannotExposeKing) {
    Position position;

    ASSERT_TRUE(position.set_from_fen("4r1k1/8/8/8/8/8/4R3/4K3 w - - 0 1"));

    const MoveList moves = generate_legal_moves(position, Color::White);

    EXPECT_FALSE(contains_move(
        moves,
        {Square::E2, Square::D2,
        MoveFlag::Quiet, PieceType::None}
    ));

    EXPECT_TRUE(contains_move(
        moves,
        {Square::E2, Square::E3,
        MoveFlag::Quiet, PieceType::None}
    ));
}

TEST(MoveGenTest, KingCannotMoveIntoCheck) {
    Position position;

    ASSERT_TRUE(position.set_from_fen("4r1k1/8/8/8/8/8/8/4K3 w - - 0 1"));

    const MoveList moves = generate_legal_moves(position, Color::White);

    EXPECT_FALSE(contains_move(
        moves,
        {Square::E1, Square::E2,
        MoveFlag::Quiet, PieceType::None}
    ));
}

TEST(MoveGenTest, KingCannotCaptureProtectedPiece) {
    Position position;

    ASSERT_TRUE(position.set_from_fen("7k/8/8/8/8/2b5/3r4/4K3 w - - 0 1"));

    const MoveList moves = generate_legal_moves(position, Color::White);

    EXPECT_FALSE(contains_move(
        moves,
        {Square::E1, Square::D2,
        MoveFlag::Capture, PieceType::None}
    ));
}

TEST(MoveGenTest, KingsideCastling) {
    Position position;

    ASSERT_TRUE(position.set_from_fen("4k3/8/8/8/8/8/8/4K2R w K - 0 1"));

    const MoveList moves = generate_legal_moves(position, Color::White);

    EXPECT_TRUE(contains_move(
        moves,
        {Square::E1, Square::G1,
        MoveFlag::KingCastle, PieceType::None}
    ));
}

TEST(MoveGenTest, QueensideCastling) {
    Position position;

    ASSERT_TRUE(position.set_from_fen("4k3/8/8/8/8/8/8/R3K3 w Q - 0 1"));

    const MoveList moves = generate_legal_moves(position, Color::White);

    EXPECT_TRUE(contains_move(
        moves,
        {Square::E1, Square::C1,
        MoveFlag::QueenCastle, PieceType::None}
    ));
}

TEST(MoveGenTest, CannotCastleThroughCheck) {
    Position position;

    ASSERT_TRUE(position.set_from_fen("4kr2/8/8/8/8/8/8/4K2R w K - 0 1"));

    const MoveList moves = generate_legal_moves(position, Color::White);

    EXPECT_FALSE(contains_move(
        moves,
        {Square::E1, Square::G1,
        MoveFlag::KingCastle, PieceType::None}
    ));
}

TEST(MoveGenTest, CannotCastleWhileInCheck) {
    Position position;

    ASSERT_TRUE(position.set_from_fen("4r1k1/8/8/8/8/8/8/4K2R w K - 0 1"));

    const MoveList moves = generate_legal_moves(position, Color::White);

    EXPECT_FALSE(contains_move(
        moves,
        {Square::E1, Square::G1,
        MoveFlag::KingCastle, PieceType::None}
    ));
}

TEST(MoveGenTest, CannotCastleWithoutRights) {
    Position position;

    ASSERT_TRUE(position.set_from_fen("4k3/8/8/8/8/8/8/4K2R w - - 0 1"));

    const MoveList moves = generate_legal_moves(position, Color::White);

    EXPECT_FALSE(contains_move(
        moves,
        {Square::E1, Square::G1,
        MoveFlag::KingCastle, PieceType::None}
    ));
}

TEST(MoveGenTest, LegalEnPassant) {
    Position position;

    ASSERT_TRUE(position.set_from_fen("4k3/8/8/3pP3/8/8/8/4K3 w - d6 0 1"));

    const MoveList moves = generate_legal_moves(position, Color::White);

    EXPECT_TRUE(contains_move(
        moves,
        {Square::E5, Square::D6,
        MoveFlag::EnPassant, PieceType::None}
    ));
}

TEST(MoveGenTest, IllegalEnPassantExposesKing) {
    Position position;

    ASSERT_TRUE(position.set_from_fen("k3r3/8/8/3pP3/8/8/8/4K3 w - d6 0 1"));

    const MoveList moves = generate_legal_moves(position, Color::White);

    EXPECT_FALSE(contains_move(
        moves,
        {Square::E5, Square::D6,
        MoveFlag::EnPassant, PieceType::None}
    ));
}

TEST(MoveGenTest, GenerateAllPromotions) {
    Position position;

    ASSERT_TRUE(position.set_from_fen("7k/P7/8/8/8/8/8/4K3 w - - 0 1"));

    const MoveList moves = generate_legal_moves(position, Color::White);

    EXPECT_TRUE(contains_move(
        moves, {Square::A7, Square::A8,
        MoveFlag::Promotion, PieceType::Queen}));

    EXPECT_TRUE(contains_move(
        moves, {Square::A7, Square::A8,
        MoveFlag::Promotion, PieceType::Rook}));

    EXPECT_TRUE(contains_move(
        moves, {Square::A7, Square::A8,
        MoveFlag::Promotion, PieceType::Bishop}));

    EXPECT_TRUE(contains_move(
        moves, {Square::A7, Square::A8,
        MoveFlag::Promotion, PieceType::Knight}));
}

TEST(MoveGenTest, GenerateCapturePromotions) {
    Position position;

    ASSERT_TRUE(position.set_from_fen("1r5k/P7/8/8/8/8/8/4K3 w - - 0 1"));

    const MoveList moves =
            generate_legal_moves(position, Color::White);

    EXPECT_TRUE(contains_move(
        moves, {Square::A7, Square::B8,
        MoveFlag::PromotionCapture, PieceType::Queen}));

    EXPECT_TRUE(contains_move(
        moves, {Square::A7, Square::B8,
        MoveFlag::PromotionCapture, PieceType::Rook}));

    EXPECT_TRUE(contains_move(
        moves, {Square::A7, Square::B8,
        MoveFlag::PromotionCapture, PieceType::Bishop}));

    EXPECT_TRUE(contains_move(
        moves, {Square::A7, Square::B8,
        MoveFlag::PromotionCapture, PieceType::Knight}));
}

TEST(MoveGenTest, CheckmateHasNoLegalMoves) {
    Position position;

    ASSERT_TRUE(position.set_from_fen("7k/6Q1/6K1/8/8/8/8/8 b - - 0 1"));

    const MoveList moves = generate_legal_moves(position, Color::Black);

    EXPECT_TRUE(moves.empty());
}

TEST(MoveGenTest, StalemateHasNoLegalMoves) {
    Position position;

    ASSERT_TRUE(position.set_from_fen("7k/5Q2/6K1/8/8/8/8/8 b - - 0 1"));

    const MoveList moves = generate_legal_moves(position, Color::Black);

    EXPECT_TRUE(moves.empty());
}

TEST(MoveGenTest, PseudoLegalMovesMayLeaveKingInCheck) {
    Position position;

    ASSERT_TRUE(position.set_from_fen("4r1k1/8/8/8/8/8/4R3/4K3 w - - 0 1"));

    const Move pinnedMove{
        Square::E2,
        Square::D2,
        MoveFlag::Quiet,
        PieceType::None
    };

    const MoveList pseudoMoves = generate_pseudo_legal_moves(position, Color::White);

    EXPECT_TRUE(contains_move(pseudoMoves, pinnedMove));
}
