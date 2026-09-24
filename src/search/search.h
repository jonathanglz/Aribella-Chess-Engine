#pragma once

#include <vector>

#include "transposition_table.h"
#include "../engine/game_state.h"
#include "../movegen/movegen.h"

namespace Aribella {
    inline constexpr int MateScore = 100000;

    struct SearchResult {
        Move best_move{};
        int score{};
    };

    struct ScoredMove {
        Move move{};
        int score{};
    };

    struct SearchStats {
        std::uint64_t nodes{};
        std::uint64_t tt_hits{};
        std::uint64_t qnodes{};
    };

    [[nodiscard]]
    SearchResult search(GameState &game, TranspositionTable &tt, int depth, bool use_move_ordering);

    SearchStats search_stats() noexcept;

    void reset_search_stats() noexcept;


    struct IterationInfo
    {
        int depth{};
        Move best_move{};
        int score{};

        // Statistics for this iteration only.
        SearchStats stats{};
    };

    struct IterativeSearchResult
    {
        // Result from the deepest completed iteration.
        SearchResult result{};
        int completed_depth{};

        // Cumulative statistics across all iterations.
        SearchStats stats{};

        // One entry for each completed depth.
        std::vector<IterationInfo> iterations{};
    };

    [[nodiscard]]
    IterativeSearchResult iterative_deepening(
        GameState &game,
        TranspositionTable &tt,
        int max_depth,
        bool use_move_ordering = true
    );

    bool is_mate_score(int score) noexcept;

    void test_iterative_deepening();
}
