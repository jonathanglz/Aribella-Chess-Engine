#include <gtest/gtest.h>

#include "../../src/position/position.h"
#include "../../src/movegen/move.h"

using namespace Aribella;

TEST(ZobristTest, SamePositionProducesSameKey) {
    Position first;
    Position second;

    first.set_start_position();
    second.set_start_position();

    EXPECT_EQ(first.key(), second.key());
}

TEST(ZobristTest, DifferentPositionsProduceDifferentKeys) {
    Position first;
    Position second;

    first.set_start_position();

    ASSERT_TRUE(second.set_from_fen("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq - 0 1"));

    EXPECT_NE(first.key(), second.key());
}

TEST(ZobristTest, SideToMoveChangesKey) {
    Position white;
    Position black;

    ASSERT_TRUE(white.set_from_fen("4k3/8/8/8/8/8/8/4K3 w - - 0 1"));

    ASSERT_TRUE(black.set_from_fen("4k3/8/8/8/8/8/8/4K3 b - - 0 1"));

    EXPECT_NE(white.key(), black.key());
}

TEST(ZobristTest, CastlingRightsChangeKey) {
    Position withRights;
    Position withoutRights;

    ASSERT_TRUE(withRights.set_from_fen("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1"));

    ASSERT_TRUE(withoutRights.set_from_fen("r3k2r/8/8/8/8/8/8/R3K2R w - - 0 1"));

    EXPECT_NE(withRights.key(), withoutRights.key());
}

TEST(ZobristTest, EnPassantSquareChangesKey) {
    Position withEnPassant;
    Position withoutEnPassant;

    ASSERT_TRUE(withEnPassant.set_from_fen("4k3/8/8/3pP3/8/8/8/4K3 w - d6 0 1"));

    ASSERT_TRUE(withoutEnPassant.set_from_fen("4k3/8/8/3pP3/8/8/8/4K3 w - - 0 1"));

    EXPECT_NE(withEnPassant.key(), withoutEnPassant.key());
}

TEST(ZobristTest, IncrementalKeyMatchesRecomputedKey) {
    Position position;
    position.set_start_position();

    const Move move{
        Square::E2,
        Square::E4,
        MoveFlag::DoublePawnPush,
        PieceType::None
    };

    const UndoState undo = position.make_move(move);
    (void)undo;

    const Zobrist::Key incrementalKey = position.key();

    position.recompute_key();

    EXPECT_EQ(incrementalKey, position.key());
}

TEST(ZobristTest, MakeUnmakeRestoresKey) {
    Position position;
    position.set_start_position();

    const Position original = position;
    const Zobrist::Key originalKey = position.key();

    const Move move{
        Square::E2,
        Square::E4,
        MoveFlag::DoublePawnPush,
        PieceType::None
    };

    const UndoState undo = position.make_move(move);

    EXPECT_NE(position.key(), originalKey);

    position.unmake_move(move, undo);

    EXPECT_EQ(position.key(), originalKey);
    EXPECT_EQ(position, original);
}

