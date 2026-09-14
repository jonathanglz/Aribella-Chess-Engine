#pragma once

#include "../engine/game_state.h"

namespace Aribella {
    inline constexpr int MateScore = 100000;

    [[nodiscard]]
    int negamax(GameState &game, int depth);
}
