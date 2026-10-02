#include <gtest/gtest.h>

#include "../../src/evaluation/evaluation.h"
#include "../../src/position/position.h"

using namespace Aribella;

TEST(EvaluationTest, StartingPositionIsEqual) {
    Position position;

    ASSERT_TRUE(position.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"));

    EXPECT_EQ(evaluate(position), 0);
}

TEST(EvaluationTest, PawnColorSymmetry) {
    Position white_position;
    Position black_position;

    ASSERT_TRUE(white_position.set_from_fen("4k3/8/8/8/8/8/4P3/4K3 w - - 0 1"));

    ASSERT_TRUE(black_position.set_from_fen("4k3/4p3/8/8/8/8/8/4K3 b - - 0 1"));

    const int white_score = evaluate(white_position);
    const int black_score = evaluate(black_position);

    EXPECT_EQ(white_score, -black_score);
}

TEST(EvaluationTest, KnightColorSymmetry) {
    Position white_position;
    Position black_position;

    ASSERT_TRUE(white_position.set_from_fen("4k3/8/8/8/8/8/8/1N2K3 w - - 0 1"));

    ASSERT_TRUE(black_position.set_from_fen("1n2k3/8/8/8/8/8/8/4K3 b - - 0 1"));

    const int white_score = evaluate(white_position);
    const int black_score = evaluate(black_position);

    EXPECT_EQ(white_score, -black_score);
}

TEST(EvaluationTest, BishopColorSymmetry)
{
    Position white_position;
    Position black_position;

    ASSERT_TRUE(white_position.set_from_fen("4k3/8/8/8/8/2B5/8/4K3 w - - 0 1"));

    ASSERT_TRUE(black_position.set_from_fen("4k3/8/2b5/8/8/8/8/4K3 b - - 0 1"));

    const int white_score = evaluate(white_position);
    const int black_score = evaluate(black_position);

    EXPECT_EQ(white_score, -black_score);
}

TEST(EvaluationTest, RookColorSymmetry)
{
    Position white_position;
    Position black_position;

    ASSERT_TRUE(white_position.set_from_fen("4k3/8/8/8/8/8/8/R3K3 w - - 0 1"));

    ASSERT_TRUE(black_position.set_from_fen("r3k3/8/8/8/8/8/8/4K3 b - - 0 1"));

    const int white_score = evaluate(white_position);
    const int black_score = evaluate(black_position);

    EXPECT_EQ(white_score, -black_score);
}

TEST(EvaluationTest, QueenColorSymmetry)
{
    Position white_position;
    Position black_position;

    ASSERT_TRUE(white_position.set_from_fen("4k3/8/8/8/8/8/8/3QK3 w - - 0 1"));

    ASSERT_TRUE(black_position.set_from_fen("3qk3/8/8/8/8/8/8/4K3 b - - 0 1"));

    const int white_score = evaluate(white_position);
    const int black_score = evaluate(black_position);

    EXPECT_EQ(white_score, -black_score);
}

TEST(EvaluationTest, KingColorSymmetry)
{
    Position white_position;
    Position black_position;

    ASSERT_TRUE(white_position.set_from_fen("4k3/8/8/8/3K4/8/8/8 w - - 0 1"));

    ASSERT_TRUE(black_position.set_from_fen("8/8/8/3k4/8/8/8/4K3 b - - 0 1"));

    const int white_score = evaluate(white_position);
    const int black_score = evaluate(black_position);

    EXPECT_EQ(white_score, -black_score);
}

TEST(EvaluationTest, PawnPositionalEvaluation)
{
    Position starting_position;
    Position advanced_position;

    ASSERT_TRUE(starting_position.set_from_fen("4k3/8/8/8/8/8/4P3/4K3 w - - 0 1"));

    ASSERT_TRUE(advanced_position.set_from_fen("4k3/8/8/8/3P4/8/8/4K3 w - - 0 1"));

    const int starting_score = evaluate(starting_position);
    const int advanced_score = evaluate(advanced_position);

    EXPECT_EQ(advanced_score - starting_score, 40);
}

TEST(EvaluationTest, KnightDevelopment) {
    Position starting_position;
    Position developed_position;

    ASSERT_TRUE(starting_position.set_from_fen("4k3/8/8/8/8/8/8/1N2K3 w - - 0 1"));

    ASSERT_TRUE(developed_position.set_from_fen("4k3/8/8/8/8/2N5/8/4K3 w - - 0 1"));

    const int starting_score = evaluate(starting_position);
    const int developed_score = evaluate(developed_position);

    EXPECT_EQ(developed_score - starting_score, 50);
}

TEST(EvaluationTest, EndgameKingActivity) {
    Position corner_position;
    Position central_position;

    ASSERT_TRUE(corner_position.set_from_fen("4k3/8/8/8/8/8/8/K7 w - - 0 1"));

    ASSERT_TRUE(central_position.set_from_fen("4k3/8/8/8/3K4/8/8/8 w - - 0 1"));

    const int corner_score = evaluate(corner_position);
    const int central_score = evaluate(central_position);

    EXPECT_EQ(corner_score, -30);
    EXPECT_EQ(central_score, 60);

    EXPECT_EQ(central_score - corner_score, 90);
}

TEST(EvaluationTest, KingTableChangesWithGamePhase) {
    Position middlegame;
    Position endgame;

    ASSERT_TRUE(middlegame.set_from_fen("r2qk2r/8/8/8/3K4/8/8/R2Q3R w kq - 0 1"));

    ASSERT_TRUE(endgame.set_from_fen("4k3/8/8/8/3K4/8/8/8 w - - 0 1"));

    EXPECT_EQ(evaluate(endgame), 60);
}

TEST(EvaluationTest, QueenlessPositionIsEndgame)
{
    Position position;

    ASSERT_TRUE(position.set_from_fen("r3k2r/8/8/8/8/8/8/R3K2R w - - 0 1"));

    EXPECT_TRUE(is_endgame(position));
}

TEST(EvaluationTest, QueenWithOneMinorPieceIsEndgame)
{
    Position position;

    ASSERT_TRUE(position.set_from_fen("3qk1n1/8/8/8/8/8/8/3QK1N1 w - - 0 1"));

    EXPECT_TRUE(is_endgame(position));
}

TEST(EvaluationTest, QueenWithTwoMinorPiecesIsMiddlegame)
{
    Position position;

    ASSERT_TRUE(position.set_from_fen("3qkbn1/8/8/8/8/8/8/3QKBN1 w - - 0 1"));

    EXPECT_FALSE(is_endgame(position));
}

TEST(EvaluationTest, QueenWithRookIsMiddlegame)
{
    Position position;

    ASSERT_TRUE(position.set_from_fen("3qk2r/8/8/8/8/8/8/3QK2R w - - 0 1"));

    EXPECT_FALSE(is_endgame(position));
}

TEST(EvaluationTest, QueenlessOpponentWithRemainingPieces)
{
    Position position;

    ASSERT_TRUE(position.set_from_fen("rnb1kbnr/8/8/8/8/8/8/3QK3 w - - 0 1"));

    EXPECT_TRUE(is_endgame(position));
}

TEST(EvaluationTest, MiddlegameKingTableSelection)
{
    Position corner_position;
    Position central_position;

    ASSERT_TRUE(corner_position.set_from_fen("r2qk2r/8/8/8/8/8/8/R2QK2R w - - 0 1"));

    ASSERT_TRUE(central_position.set_from_fen("r2qk2r/8/8/8/3K4/8/8/R2Q3R w - - 0 1"));

    ASSERT_FALSE(is_endgame(corner_position));
    ASSERT_FALSE(is_endgame(central_position));

    const int corner_score = evaluate(corner_position);
    const int central_score = evaluate(central_position);

    EXPECT_EQ(central_score - corner_score, -40);
}