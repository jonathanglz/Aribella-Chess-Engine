#include <gtest/gtest.h>

#include "../../src/search/transposition_table.h"

using namespace Aribella;

TEST(TranspositionTableTest, InitiallyEmpty) {
    TranspositionTable table(16);

    constexpr Zobrist::Key key = 12345;

    EXPECT_EQ(table.probe(key), nullptr);
}

TEST(TranspositionTableTest, StoreAndProbe) {
    TranspositionTable table(16);

    constexpr Zobrist::Key key = 12345;

    const Move move{
        Square::E2,
        Square::E4,
        MoveFlag::DoublePawnPush,
        PieceType::None
    };

    table.store(
        key,
        5,
        100,
        TTFlag::Exact,
        move
    );

    const TTEntry *entry = table.probe(key);

    ASSERT_NE(entry, nullptr);

    EXPECT_TRUE(entry->valid);
    EXPECT_EQ(entry->key, key);
    EXPECT_EQ(entry->depth, 5);
    EXPECT_EQ(entry->score, 100);
    EXPECT_EQ(entry->flag, TTFlag::Exact);
    EXPECT_EQ(entry->best_move, move);
}

TEST(TranspositionTableTest, ReplaceExistingEntry) {
    TranspositionTable table(16);

    constexpr Zobrist::Key key = 12345;

    const Move firstMove{
        Square::E2,
        Square::E4,
        MoveFlag::DoublePawnPush,
        PieceType::None
    };

    const Move secondMove{
        Square::D2,
        Square::D4,
        MoveFlag::DoublePawnPush,
        PieceType::None
    };

    table.store(
        key,
        3,
        50,
        TTFlag::UpperBound,
        firstMove
    );

    table.store(
        key,
        6,
        120,
        TTFlag::Exact,
        secondMove
    );

    const TTEntry *entry = table.probe(key);

    ASSERT_NE(entry, nullptr);

    EXPECT_EQ(entry->key, key);
    EXPECT_EQ(entry->depth, 6);
    EXPECT_EQ(entry->score, 120);
    EXPECT_EQ(entry->flag, TTFlag::Exact);
    EXPECT_EQ(entry->best_move, secondMove);
}

TEST(TranspositionTableTest, ShallowerSameKeyDoesNotReplace) {
    TranspositionTable table(16);

    constexpr Zobrist::Key key = 12345;

    const Move deepMove{
        Square::E2,
        Square::E4,
        MoveFlag::DoublePawnPush,
        PieceType::None
    };

    const Move shallowMove{
        Square::D2,
        Square::D4,
        MoveFlag::DoublePawnPush,
        PieceType::None
    };

    table.store(
        key,
        8,
        200,
        TTFlag::Exact,
        deepMove
    );

    table.store(
        key,
        3,
        50,
        TTFlag::UpperBound,
        shallowMove
    );

    const TTEntry *entry = table.probe(key);

    ASSERT_NE(entry, nullptr);

    EXPECT_EQ(entry->depth, 8);
    EXPECT_EQ(entry->score, 200);
    EXPECT_EQ(entry->flag, TTFlag::Exact);
    EXPECT_EQ(entry->best_move, deepMove);
}

TEST(TranspositionTableTest, HandleIndexCollision) {
    TranspositionTable table(4);

    constexpr Zobrist::Key firstKey = 1;
    constexpr Zobrist::Key secondKey = 5;

    ASSERT_EQ(firstKey % 4, secondKey % 4);

    const Move firstMove{
        Square::E2,
        Square::E4,
        MoveFlag::DoublePawnPush,
        PieceType::None
    };

    const Move secondMove{
        Square::D2,
        Square::D4,
        MoveFlag::DoublePawnPush,
        PieceType::None
    };

    table.store(
        firstKey,
        6,
        100,
        TTFlag::Exact,
        firstMove
    );

    ASSERT_NE(table.probe(firstKey), nullptr);

    table.store(
        secondKey,
        3,
        50,
        TTFlag::LowerBound,
        secondMove
    );

    EXPECT_NE(table.probe(firstKey), nullptr);
    EXPECT_EQ(table.probe(secondKey), nullptr);


    table.store(
        secondKey,
        8,
        150,
        TTFlag::LowerBound,
        secondMove
    );

    EXPECT_EQ(table.probe(firstKey), nullptr);

    const TTEntry *entry = table.probe(secondKey);

    ASSERT_NE(entry, nullptr);

    EXPECT_EQ(entry->key, secondKey);
    EXPECT_EQ(entry->depth, 8);
    EXPECT_EQ(entry->score, 150);
    EXPECT_EQ(entry->flag, TTFlag::LowerBound);
    EXPECT_EQ(entry->best_move, secondMove);
}
