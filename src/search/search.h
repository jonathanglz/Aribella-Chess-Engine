#pragma once

#include "transposition_table.h"
#include "../engine/game_state.h"

namespace Aribella {
    inline constexpr int MateScore = 100000;

    struct SearchResult {
        Move best_move{};
        int score{};
    };

    struct SearchStats {
        std::uint64_t nodes{};
        std::uint64_t tt_hits{};
    };

    [[nodiscard]]
    SearchResult search(GameState &game, TranspositionTable &tt, int depth);

    [[nodiscard]]
    SearchStats search_stats() noexcept;

    void reset_search_stats() noexcept;

}
