#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "magic.h"
#include "magic_constants.h"


namespace Aribella {
    [[nodiscard]]
    constexpr std::size_t magic_table_size(Bitboard mask) noexcept {
        const auto relevant_bits = static_cast<std::size_t>(popcount(mask));

        return std::size_t{1} << relevant_bits;
    }

    [[nodiscard]]
    constexpr std::size_t calculate_total_rook_attack_entries() noexcept {
        std::size_t total{0};

        for (std::size_t index = 0; index < to_index(Square::Count); ++index) {
            const auto sq = static_cast<Square>(index);

            const Bitboard mask = generate_rook_relevant_mask(sq);

            total += magic_table_size(mask);
        }

        return total;
    }

    [[nodiscard]]
    constexpr std::size_t calculate_total_bishop_attack_entries() noexcept {
        std::size_t total{0};

        for (std::size_t index = 0; index < to_index(Square::Count); ++index) {
            const auto sq = static_cast<Square>(index);

            const Bitboard mask = generate_bishop_relevant_mask(sq);

            total += magic_table_size(mask);
        }

        return total;
    }

    inline constexpr std::size_t TotalRookAttackEntries = calculate_total_rook_attack_entries();
    inline constexpr std::size_t TotalBishopAttackEntries = calculate_total_bishop_attack_entries();

    [[nodiscard]]
    constexpr std::array<SMagic, to_index(Square::Count)> make_rook_magics() noexcept {
        std::array<SMagic, to_index(Square::Count)> magics{};

        std::size_t current_offset{0};

        for (std::size_t index = 0; index < magics.size(); ++index) {
            const auto sq = static_cast<Square>(index);
            const Bitboard mask = generate_rook_relevant_mask(sq);

            const auto relevant_bits = static_cast<std::uint8_t>(popcount(mask));

            magics[index] = {
                .mask = mask,
                .magic = RookMagicNumbers[index],
                .shift = static_cast<std::uint8_t>(64 - relevant_bits),
                .offset = current_offset
            };

            current_offset += magic_table_size(mask);
        }
        return magics;
    }

    [[nodiscard]]
    constexpr std::array<SMagic, to_index(Square::Count)> make_bishop_magics() noexcept {
        std::array<SMagic, to_index(Square::Count)> magics{};

        std::size_t current_offset{0};

        for (std::size_t index = 0; index < magics.size(); ++index) {
            const auto sq = static_cast<Square>(index);
            const Bitboard mask = generate_bishop_relevant_mask(sq);

            const auto relevant_bits = static_cast<std::uint8_t>(popcount(mask));

            magics[index] = {
                .mask = mask,
                .magic = BishopMagicNumbers[index],
                .shift = static_cast<std::uint8_t>(64 - relevant_bits),
                .offset = current_offset
            };

            current_offset += magic_table_size(mask);
        }
        return magics;
    }

    inline constexpr auto RookMagics = make_rook_magics();

    inline constexpr auto BishopMagics = make_bishop_magics();

    using RookAttackTable = std::array<Bitboard, TotalRookAttackEntries>;
    using BishopAttackTable = std::array<Bitboard, TotalBishopAttackEntries>;

    [[nodiscard]]
    constexpr RookAttackTable make_rook_attacks_table() noexcept {
        RookAttackTable table{};

        for (std::size_t square_index = 0; square_index < to_index(Square::Count); ++square_index) {
            const auto sq = static_cast<Square>(square_index);

            const auto reference_table = generate_rook_occupancy_attack_table(sq);

            const SMagic &magic = RookMagics[square_index];

            for (std::size_t entry_index = 0; entry_index < reference_table.count; ++entry_index) {
                const auto &entry = reference_table.entries[entry_index];

                const std::size_t index = magic_index(entry.occupancy, magic);

                table[magic.offset + index] = entry.attacks;
            }
        }
        return table;
    }

    [[nodiscard]]
    constexpr BishopAttackTable make_bishop_attacks_table() noexcept {
        BishopAttackTable table{};

        for (std::size_t square_index = 0; square_index < to_index(Square::Count); ++square_index) {
            const auto sq = static_cast<Square>(square_index);

            const auto reference_table = generate_bishop_occupancy_attack_table(sq);

            const SMagic &magic = BishopMagics[square_index];

            for (std::size_t entry_index = 0; entry_index < reference_table.count; ++entry_index) {
                const auto &entry = reference_table.entries[entry_index];

                const std::size_t index = magic_index(entry.occupancy, magic);

                table[magic.offset + index] = entry.attacks;
            }
        }
        return table;
    }

    inline const auto RookAttackLookup = make_rook_attacks_table();

    inline const auto BishopAttackLookup = make_bishop_attacks_table();

    [[nodiscard]]
    inline Bitboard rook_attacks(Square sq, Bitboard occupancy) noexcept {
        assert(is_valid(sq));

        const std::size_t square_index = to_index(sq);

        const SMagic &magic = RookMagics[square_index];

        const std::size_t index = magic_index(occupancy, magic);

        return RookAttackLookup[magic.offset + index];
    }

    [[nodiscard]]
    inline Bitboard bishop_attacks(Square sq, Bitboard occupancy) noexcept {
        assert(is_valid(sq));

        const std::size_t square_index = to_index(sq);

        const SMagic &magic = BishopMagics[square_index];

        const std::size_t index = magic_index(occupancy, magic);

        return BishopAttackLookup[magic.offset + index];
    }

    [[nodiscard]]
    inline Bitboard queen_attacks(Square sq, Bitboard occupancy) noexcept {
        assert(is_valid(sq));

        return rook_attacks(sq, occupancy) | bishop_attacks(sq, occupancy);
    }

}
