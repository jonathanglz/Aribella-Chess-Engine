#pragma once

#include "../position/position.h"

namespace Aribella {
    bool is_endgame(const Position &position) noexcept;

    [[nodiscard]]
    int evaluate(const Position &position) noexcept;
}
