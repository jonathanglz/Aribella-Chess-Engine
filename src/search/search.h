#pragma once

#include "../engine/game_state.h"

namespace Aribella {
    inline constexpr int MateScore = 100000;

    struct SearchResult {
        Move best_move{};
        int score{};
    };

    [[nodiscard]]
    SearchResult search(GameState& game, int depth);
}
