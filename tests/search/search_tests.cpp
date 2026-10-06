#include <gtest/gtest.h>

#include "../../src/search/search.h"
#include "../../src/movegen/movegen.h"
#include "../../src/engine/game_state.h"
#include "../../src/search/transposition_table.h"

using namespace Aribella;

namespace {
    bool contains_move(const MoveList &moves, const Move &target) {
        for (const Move &move: moves) {
            if (move == target)
                return true;
        }

        return false;
    }
}

TEST(SearchTest, FindsMateInOne) {
    GameState game;

    ASSERT_TRUE(game.set_from_fen("7k/5Q2/6K1/8/8/8/8/8 w - - 0 1"));

    TranspositionTable tt(1024);
    SearchControl control;

    const SearchResult result =
            search(game, tt, 2, true, control);

    EXPECT_TRUE(is_mate_score(result.score));

    const UndoState undo = game.make_move(result.best_move);

    Position position = game.position();

    const MoveList replies = generate_legal_moves(position, position.side_to_move());

    EXPECT_TRUE(replies.empty());

    game.unmake_move(result.best_move, undo);
}

TEST(SearchTest, DetectsCheckmate) {
    GameState game;

    ASSERT_TRUE(game.set_from_fen("7k/6Q1/6K1/8/8/8/8/8 b - - 0 1"));

    TranspositionTable tt(1024);
    SearchControl control;

    const SearchResult result = search(game, tt, 3, true, control);

    EXPECT_TRUE(is_mate_score(result.score));
}


TEST(SearchTest, DetectsStalemate) {
    GameState game;

    ASSERT_TRUE(game.set_from_fen("7k/5Q2/6K1/8/8/8/8/8 b - - 0 1"));

    TranspositionTable tt(1024);
    SearchControl control;

    const SearchResult result = search(game, tt, 3, true, control);

    EXPECT_EQ(result.score, 0);
}


TEST(SearchTest, FindsForcedCapture) {
    GameState game;

    ASSERT_TRUE(game.set_from_fen("q3k3/8/8/8/8/8/8/R3K3 w - - 0 1"));

    TranspositionTable tt(1024);
    SearchControl control;

    const SearchResult result = search(game, tt, 2, true, control);

    const Move expected{
        Square::A1,
        Square::A8,
        MoveFlag::Capture,
        PieceType::None
    };

    EXPECT_EQ(result.best_move, expected);
}


TEST(SearchTest, ReturnsLegalBestMove) {
    GameState game;

    ASSERT_TRUE(game.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"));

    TranspositionTable tt(1024);
    SearchControl control;

    const SearchResult result = search(game, tt, 3, true, control);

    Position position = game.position();

    const MoveList legalMoves =
            generate_legal_moves(position, position.side_to_move());

    EXPECT_TRUE(contains_move(legalMoves, result.best_move));
}


TEST(SearchTest, RespectsDepthLimit) {
    GameState game;

    ASSERT_TRUE(game.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"));

    TranspositionTable tt(1024);
    SearchControl control;

    constexpr int maxDepth = 3;

    const IterativeSearchResult result =
            iterative_deepening(
                game,
                tt,
                maxDepth,
                true,
                control
            );

    EXPECT_EQ(result.completed_depth, maxDepth);

    ASSERT_EQ(result.iterations.size(), static_cast<std::size_t>(maxDepth));

    EXPECT_EQ(result.iterations[0].depth, 1);
    EXPECT_EQ(result.iterations[1].depth, 2);
    EXPECT_EQ(result.iterations[2].depth, 3);
}


TEST(SearchTest, HandlesCancellation) {
    GameState game;

    ASSERT_TRUE(game.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"));

    TranspositionTable tt(1024);
    SearchControl control;

    control.stop();

    const IterativeSearchResult result =
            iterative_deepening(
                game,
                tt,
                10,
                true,
                control
            );

    EXPECT_TRUE(control.stopped.load());
    EXPECT_LT(result.completed_depth, 10);
}

TEST(SearchTest, PreservesRootPosition) {
    GameState game;

    ASSERT_TRUE(game.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"));

    const Position original = game.position();

    TranspositionTable tt(1024);
    SearchControl control;

    const SearchResult result = search(game, tt, 4, true, control);

    (void) result;

    EXPECT_EQ(game.position(), original);
}


TEST(SearchTest, RepeatedSearchIsStable) {
    GameState firstGame;
    GameState secondGame;

    const std::string_view fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    ASSERT_TRUE(firstGame.set_from_fen(fen));
    ASSERT_TRUE(secondGame.set_from_fen(fen));

    TranspositionTable firstTT(1024);
    TranspositionTable secondTT(1024);

    SearchControl firstControl;
    SearchControl secondControl;

    const SearchResult first =
            search(
                firstGame,
                firstTT,
                3,
                true,
                firstControl
            );

    const SearchResult second =
            search(
                secondGame,
                secondTT,
                3,
                true,
                secondControl
            );

    EXPECT_EQ(first.score, second.score);
    EXPECT_EQ(first.best_move, second.best_move);
}

TEST(SearchTest, SearchStatsCountNodes) {
    GameState game;

    ASSERT_TRUE(game.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
    ));

    TranspositionTable tt(1024);
    SearchControl control;

    reset_search_stats();

    const SearchResult result = search(game, tt, 2, true, control);

    (void) result;

    const SearchStats stats = search_stats();

    EXPECT_GT(stats.nodes, 0U);
}
