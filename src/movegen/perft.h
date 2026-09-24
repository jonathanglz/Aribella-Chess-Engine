#pragma once

#include "../engine/game_state.h"

#include <cstdint>


namespace Aribella {
    [[nodiscard]]
    std::uint64_t perft(GameState &game, int depth);

    void pertf_divide(GameState &game, int depth);
}