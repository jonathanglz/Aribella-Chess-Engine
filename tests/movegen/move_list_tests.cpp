#include <gtest/gtest.h>

#include "../../src/movegen/move_list.h"

using namespace Aribella;

TEST(MoveListTest, InitiallyEmpty) {
    const MoveList moves;

    EXPECT_TRUE(moves.empty());
    EXPECT_EQ(moves.size(), 0);
}

TEST(MoveListTest, PushBackIncreasesSize) {
    MoveList moves;

    const Move move{
        Square::E2,
        Square::E4,
        MoveFlag::DoublePawnPush,
        PieceType::None
    };

    moves.push_back(move);

    EXPECT_EQ(moves.size(), 1);
    EXPECT_FALSE(moves.empty());
}

TEST(MoveListTest, PreservesInsertionOrder) {
    MoveList moves;

    const Move first{
        Square::E2,
        Square::E4,
        MoveFlag::DoublePawnPush,
        PieceType::None
    };

    const Move second{
        Square::D2,
        Square::D4,
        MoveFlag::DoublePawnPush,
        PieceType::None
    };

    moves.push_back(first);
    moves.push_back(second);

    EXPECT_EQ(moves[0], first);
    EXPECT_EQ(moves[1], second);
}

TEST(MoveListTest, IndexedAccess) {
    MoveList moves;

    const Move move{
        Square::G1,
        Square::F3,
        MoveFlag::Quiet,
        PieceType::None
    };

    moves.push_back(move);

    EXPECT_EQ(moves[0], move);

    const Move replacement{
        Square::B1,
        Square::C3,
        MoveFlag::Quiet,
        PieceType::None
    };

    moves[0] = replacement;

    EXPECT_EQ(moves[0], replacement);
}

TEST(MoveListTest, ClearResetsSize) {
    MoveList moves;

    const Move move{
        Square::E2,
        Square::E4,
        MoveFlag::DoublePawnPush,
        PieceType::None
    };

    moves.push_back(move);

    EXPECT_EQ(moves.size(), 1);

    moves.clear();

    EXPECT_TRUE(moves.empty());
    EXPECT_EQ(moves.size(), 0);
}

TEST(MoveListTest, BeginEndIterateValidMoves) {
    MoveList moves;

    const Move first{
        Square::E2,
        Square::E4,
        MoveFlag::DoublePawnPush,
        PieceType::None
    };

    const Move second{
        Square::G1,
        Square::F3,
        MoveFlag::Quiet,
        PieceType::None
    };

    moves.push_back(first);
    moves.push_back(second);

    std::size_t count = 0;

    for (const Move &move: moves) {
        if (count == 0)
            EXPECT_EQ(move, first);
        else if (count == 1)
            EXPECT_EQ(move, second);
        ++count;
    }

    EXPECT_EQ(count, 2U);
}

TEST(MoveListTest, ConstIteration) {
    MoveList list;

    const Move expected{
        Square::E2,
        Square::E4,
        MoveFlag::DoublePawnPush,
        PieceType::None
    };

    list.push_back(expected);

    const MoveList &moves = list;

    std::size_t count = 0;

    for (const Move &move: moves) {
        EXPECT_EQ(move, expected);
        ++count;
    }

    EXPECT_EQ(count, 1U);
}

TEST(MoveListTest, MaximumCapacity) {
    MoveList moves;

    const Move move{
        Square::E2,
        Square::E4,
        MoveFlag::DoublePawnPush,
        PieceType::None
    };

    for (std::size_t i = 0; i < MaxMoves; ++i) {
        moves.push_back(move);
    }

    EXPECT_EQ(moves.size(), MaxMoves);
    EXPECT_FALSE(moves.empty());

    std::size_t count = 0;

    for (const Move &current: moves) {
        EXPECT_EQ(current, move);
        ++count;
    }

    EXPECT_EQ(count, MaxMoves);
}
