#pragma once

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>

#include "../core/bitboard.h"
#include "../core/constants.h"
#include "../core/types.h"
#include "attacks.h"

namespace Aribella {
    // One entry pair
    struct OccupancyAttackEntry {
        Bitboard occupancy{};
        Bitboard attacks{};
    };

    // Set of entry pairs
    template<std::size_t Capacity>
    struct OccupancyAttackTable {
        std::array<OccupancyAttackEntry, Capacity> entries{};
        std::size_t count{};
    };

    [[nodiscard]]
    constexpr Bitboard set_occupancy(std::size_t index, Bitboard attack_mask) noexcept {
        Bitboard occupancy{0};

        const std::size_t bit_count = popcount(attack_mask);

        for (std::size_t bit = 0; bit < bit_count; ++bit) {
            const Square square = pop_lsb(attack_mask);

            if (index & (std::size_t{1} << bit)) {
                occupancy |= square_bb(square);
            }
        }

        return occupancy;
    }

    [[nodiscard]]
    constexpr OccupancyAttackTable<MaxRookOccupancies>
    generate_rook_occupancy_attack_table(Square sq) noexcept {
        assert(is_valid(sq));

        OccupancyAttackTable<MaxRookOccupancies> result{};

        const Bitboard mask = generate_rook_relevant_mask(sq);
        const auto relevant_bits = static_cast<std::size_t>(popcount(mask));

        result.count = std::size_t{1} << relevant_bits;

        assert(result.count <= result.entries.size());

        for (std::size_t index = 0; index < result.count; ++index) {
            const Bitboard occupancy = set_occupancy(index, mask);

            result.entries[index] = {
                .occupancy = occupancy, .attacks = generate_rook_attacks(sq, occupancy)
            };
        }
        return result;
    }

    [[nodiscard]]
    constexpr OccupancyAttackTable<MaxBishopOccupancies>
    generate_bishop_occupancy_attack_table(Square sq) noexcept {
        assert(is_valid(sq));

        OccupancyAttackTable<MaxBishopOccupancies> result{};

        const Bitboard mask = generate_bishop_relevant_mask(sq);
        const auto relevant_bits = static_cast<std::size_t>(popcount(mask));

        result.count = std::size_t{1} << relevant_bits;

        assert(result.count <= result.entries.size());

        for (std::size_t index = 0; index < result.count; ++index) {
            const Bitboard occupancy = set_occupancy(index, mask);

            result.entries[index] = {
                .occupancy = occupancy, .attacks = generate_bishop_attacks(sq, occupancy)
            };
        }
        return result;
    }

    struct SMagic {
        Bitboard mask{}; // relevant blocker squares
        Bitboard magic{}; // hash multiplier
        std::uint8_t shift{}; // number of low product bits discarded
        std::size_t offset{}; // starting position in the attack table
    };

    [[nodiscard]]
    constexpr std::size_t magic_index(Bitboard occupancy, const SMagic &entry) noexcept {
        assert(entry.shift < 64);

        occupancy &= entry.mask;
        occupancy *= entry.magic;
        occupancy >>= entry.shift;

        return static_cast<std::size_t>(occupancy);
    }

    [[nodiscard]]
    SMagic find_rook_magic(Square sq);

    [[nodiscard]]
    SMagic find_bishop_magic(Square sq);
}
