#pragma once

#include <cassert>
#include <cstdint>

namespace Aribella {
    class Xorshift64Star {
    private:
        std::uint64_t state;

    public:
        explicit constexpr Xorshift64Star(std::uint64_t seed) noexcept : state{seed} {
        }

        [[nodiscard]]
        constexpr std::uint64_t next() noexcept {
            std::uint64_t value = state;

            value ^= value >> 12;
            value ^= value << 25;
            value ^= value >> 27;

            state = value;

            // Sebastian Vignas 0x2545F4914F6CDD1DULL hexadecimal number
            return value * 0x2545F4914F6CDD1DULL;
        }
    };
}
