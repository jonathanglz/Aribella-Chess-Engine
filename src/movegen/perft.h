#pragma once

#include <cstdint>
#include "../position/position.h"

namespace Aribella {
    [[nodiscard]]
    std::uint64_t perft(Position &position, Color side, int depth);

    void pertf_divide(Position &position, Color side, int depth);
}