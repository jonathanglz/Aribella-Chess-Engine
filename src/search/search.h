#pragma once

#include <atomic>
#include <vector>
#include <chrono>

#include "transposition_table.h"
#include "../engine/game_state.h"
#include "../movegen/movegen.h"

namespace Aribella {
    inline constexpr int MateScore = 100000;

    struct SearchResult {
        Move best_move{};
        int score{};

        std::vector<Move> pv{};
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

    struct SearchControl {
        using Clock = std::chrono::steady_clock;

        Clock::time_point deadline = Clock::time_point::max();

        std::atomic_bool stopped{false};

        [[nodiscard]]
        bool should_stop() noexcept {
            if (stopped.load(std::memory_order_relaxed)) {
                return true;
            }
            if (Clock::now() >= deadline) {
                stopped.store(true, std::memory_order_relaxed);
                return true;
            }
            return false;
        }

        void stop() noexcept {
            stopped.store(true, std::memory_order_relaxed);
        }
    };

    [[nodiscard]]
    SearchResult search(
        GameState &game,
        TranspositionTable &tt,
        int depth,
        bool use_move_ordering,
        SearchControl &control
    );

    SearchStats search_stats() noexcept;

    void reset_search_stats() noexcept;

    struct IterationInfo {
        int depth{};
        Move best_move{};
        int score{};

        SearchStats stats{};

        std::vector<Move> pv{};
    };

    struct IterativeSearchResult {
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
        bool use_move_ordering,
        SearchControl &control
    );

    bool is_mate_score(int score) noexcept;

}
