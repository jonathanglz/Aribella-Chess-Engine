#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <thread>

#include "engine/engine.h"

using namespace Aribella;

namespace {
    constexpr std::string_view StartFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
}


TEST(EngineTest, InitializesCorrectly) {
    Engine engine(1024);

    EXPECT_EQ(engine.tt().probe(12345), nullptr);

    EXPECT_EQ(engine.game().position().occupancy(), 0ULL);
}


TEST(EngineTest, StartsAndStopsSearch) {
    Engine engine(1024);

    ASSERT_TRUE(engine.game().set_from_fen(StartFEN));

    std::atomic_bool callbackCalled{false};

    engine.start_search(
        10,
        true,
        [&](const IterativeSearchResult &result, bool is_current) {
            callbackCalled.store(
                true,
                std::memory_order_relaxed
            );

            EXPECT_TRUE(is_current);
        }
    );

    engine.stop();
    engine.wait_for_search();

    EXPECT_TRUE(callbackCalled.load(std::memory_order_relaxed));
}


TEST(EngineTest, HandlesRepeatedSearches) {
    Engine engine(1024);

    ASSERT_TRUE(engine.game().set_from_fen(StartFEN));

    std::atomic_int callbackCount{0};

    engine.start_search(
        1,
        true,
        [&](const IterativeSearchResult &, bool is_current) {
            if (is_current) {
                callbackCount.fetch_add(
                    1,
                    std::memory_order_relaxed
                );
            }
        }
    );

    engine.wait_for_search();

    engine.start_search(
        1,
        true,
        [&](const IterativeSearchResult &, bool is_current) {
            if (is_current) {
                callbackCount.fetch_add(
                    1,
                    std::memory_order_relaxed
                );
            }
        }
    );

    engine.wait_for_search();

    EXPECT_EQ(callbackCount.load(std::memory_order_relaxed), 2);
}


TEST(EngineTest, HandlesNewGame) {
    Engine engine(1024);

    ASSERT_TRUE(engine.game().set_from_fen(StartFEN));

    const Move e2e4{
        Square::E2,
        Square::E4,
        MoveFlag::DoublePawnPush,
        PieceType::None
    };

    [[maybe_unused]]
            const UndoState undo =
                    engine.game().make_move(e2e4);

    ASSERT_TRUE(engine.game().position().is_occupied(Square::E4));

    const Move storedMove{
        Square::E2,
        Square::E4,
        MoveFlag::DoublePawnPush,
        PieceType::None
    };

    engine.tt().store(
        12345,
        5,
        100,
        TTFlag::Exact,
        storedMove
    );

    ASSERT_NE(engine.tt().probe(12345), nullptr);

    engine.new_game();

    EXPECT_EQ(engine.tt().probe(12345),nullptr);

    EXPECT_EQ(engine.game().position().occupancy(), 0ULL);
}
