#pragma once

#include <cstdint>
#include "../position/position.h"

namespace Aribella {
    [[nodiscard]]
    std::uint64_t perft(Position &position, int depth);

    void pertf_divide(Position &position, int depth);
}