#pragma once

#include <cassert>
#include "../core/bitboard.h"
#include "magic_constants.h"

namespace Aribella {
    // See ChessProgramming Wiki - Pawn Attacks(Bitboards)
    constexpr Bitboard pawn_attacks(Color c,
                                    Bitboard pawns) noexcept {
        assert(is_valid(c));

        return
                c == Color::White
                    ? shift_northeast(pawns) | shift_northwest(pawns)
                    : shift_southeast(pawns) | shift_southwest(pawns);
    }

    // Calculates one mask manually
    // See ChessProgramming Wiki - Knight_Pattern(Knight Attacks)
    constexpr Bitboard generate_knight_attacks(Square sq) noexcept {
        assert(is_valid(sq));

        const Bitboard bb = square_bb(sq);

        return
                shift_north(shift_northeast(bb)) |
                shift_north(shift_northwest(bb)) |
                shift_south(shift_southeast(bb)) |
                shift_south(shift_southwest(bb)) |
                shift_east(shift_northeast(bb)) |
                shift_east(shift_southeast(bb)) |
                shift_west(shift_northwest(bb)) |
                shift_west(shift_southwest(bb));
    }

    // Builds table for all 64 masks
    constexpr std::array<Bitboard, to_index(Square::Count)> make_knight_attack_table() noexcept {
        std::array<Bitboard, to_index(Square::Count)> attacksTable{};

        for (std::size_t i = 0; i < attacksTable.size(); ++i) {
            const auto sq =
                    static_cast<Square>(i);
            attacksTable[i] =
                    generate_knight_attacks(sq);
        }

        return attacksTable;
    }

    // Stores all 64 masks in array
    inline constexpr auto KnightAttackTable = make_knight_attack_table();

    // Retrieves one of the stored mask.
    constexpr Bitboard knight_attacks(Square sq) noexcept {
        assert(is_valid(sq));
        return KnightAttackTable[to_index(sq)];
    }

    // Generates only one mask manually
    constexpr Bitboard generate_king_attacks(Square sq) noexcept {
        assert(is_valid(sq));

        const Bitboard bb = square_bb(sq);

        return
                shift_north(bb) |
                shift_northeast(bb) |
                shift_northwest(bb) |
                shift_east(bb) |
                shift_west(bb) |
                shift_south(bb) |
                shift_southeast(bb) |
                shift_southwest(bb);
    }

    // Builds table for all 64 masks
    constexpr std::array<Bitboard, to_index(Square::Count)> make_king_attack_table() noexcept {
        std::array<Bitboard, to_index(Square::Count)> attacksTable{};

        for (std::size_t i = 0; i < attacksTable.size(); ++i) {
            const auto sq =
                    static_cast<Square>(i);
            attacksTable[i] =
                    generate_king_attacks(sq);
        }

        return attacksTable;
    }

    // Stores all 64 masks in array
    inline constexpr auto KingAttackTable = make_king_attack_table();

    // Retrieves on of the stored mask
    constexpr Bitboard king_attacks(Square sq) noexcept {
        assert(is_valid(sq));

        return KingAttackTable[to_index(sq)];
    }

    constexpr Bitboard generate_attacks_in_direction(Square sq, Direction direction, Bitboard occupancy) noexcept {
        assert(is_valid(sq));

        Bitboard attacks{0};
        Bitboard current =
                square_bb(sq);

        while (true) {
            current =
                    shift(current,
                          direction);

            if (current == 0) {
                break;
            }

            attacks |= current;

            if (current & occupancy) {
                break;
            }
        }
        return attacks;
    }

    constexpr Bitboard generate_rook_attacks(Square sq, Bitboard occupancy) noexcept {
        return
                generate_attacks_in_direction(sq, Direction::North, occupancy) |
                generate_attacks_in_direction(sq, Direction::South, occupancy) |
                generate_attacks_in_direction(sq, Direction::East, occupancy) |
                generate_attacks_in_direction(sq, Direction::West, occupancy);
    }

    constexpr Bitboard generate_bishop_attacks(Square sq, Bitboard occupancy) noexcept {
        return
                generate_attacks_in_direction(sq, Direction::NorthEast, occupancy) |
                generate_attacks_in_direction(sq, Direction::NorthWest, occupancy) |
                generate_attacks_in_direction(sq, Direction::SouthEast, occupancy) |
                generate_attacks_in_direction(sq, Direction::SouthWest, occupancy);
    }

    constexpr Bitboard generate_queen_attacks(Square sq, Bitboard occupancy) noexcept {
        return
                generate_rook_attacks(sq, occupancy) |
                generate_bishop_attacks(sq, occupancy);
    }

    // See ChessProgramming.org - Magic Bitboards
    constexpr Bitboard generate_relevant_mask_in_direction(Square sq, Direction direction) noexcept {
        assert(is_valid(sq));

        Bitboard mask{0};
        Bitboard current = square_bb(sq);

        while (true) {
            current =
                    shift(current,
                          direction);

            if (current == 0) {
                break;
            }

            if (const Bitboard next = shift(current, direction); next == 0) {
                break;
            }

            mask |= current;
        }

        return mask;
    }

    constexpr Bitboard generate_rook_relevant_mask(Square sq) noexcept {
        return
                generate_relevant_mask_in_direction(sq, Direction::North) |
                generate_relevant_mask_in_direction(sq, Direction::South) |
                generate_relevant_mask_in_direction(sq, Direction::East) |
                generate_relevant_mask_in_direction(sq, Direction::West);
    }

    constexpr Bitboard generate_bishop_relevant_mask(Square sq) noexcept {
        return
                generate_relevant_mask_in_direction(sq, Direction::NorthEast) |
                generate_relevant_mask_in_direction(sq, Direction::NorthWest) |
                generate_relevant_mask_in_direction(sq, Direction::SouthEast) |
                generate_relevant_mask_in_direction(sq, Direction::SouthWest);
    }

    // Converts integer index into one subset of attack_mask
    [[nodiscard]]
    constexpr Bitboard set_occupancy(std::size_t index, Bitboard attack_mask) noexcept {
        Bitboard occupancy{0};

        const int bit_count =
                popcount(attack_mask);

        for (std::size_t bit = 0; bit < bit_count; ++bit) {
            const Square square =
                    pop_lsb(attack_mask);

            if (index & (std::size_t{1} << bit)) {
                occupancy |= square_bb(square);
            }
        }

        return occupancy;
    }

    // Known blocker occupancy -> sliding-piece attacks
    struct OccupancyAttackEntry {
        Bitboard occupancy{};
        Bitboard attacks{};
    };

    // Holds every possible occupancy/attack pair for one square
    template<std::size_t Capacity>
    struct OccupancyAttackTable {
        std::array<OccupancyAttackEntry, Capacity> entries{};
        std::size_t count{};
    };

    // Generates all rook blocker occupancies and their correct attacks
    [[nodiscard]]
    constexpr OccupancyAttackTable<MaxRookOccupancies> generate_rook_occupancy_attack_table(Square sq) noexcept {
        assert(is_valid(sq));

        OccupancyAttackTable<MaxRookOccupancies> result{};

        const Bitboard mask =
                generate_rook_relevant_mask(sq);
        const int relevant_bits =
                popcount(mask);

        result.count =
                std::size_t{1} << relevant_bits;

        assert(result.count <= result.entries.size());

        for (std::size_t index = 0; index < result.count; ++index) {
            const Bitboard occupancy =
                    set_occupancy(index,
                                  mask);

            result.entries[index] = {
                .occupancy = occupancy,
                .attacks = generate_rook_attacks(sq, occupancy)
            };
        }
        return result;
    }

    // Generates all bishop blocker occupancies and their correct attacks
    [[nodiscard]]
    constexpr OccupancyAttackTable<MaxBishopOccupancies> generate_bishop_occupancy_attack_table(Square sq) noexcept {
        assert(is_valid(sq));

        OccupancyAttackTable<MaxBishopOccupancies> result{};

        const Bitboard mask =
                generate_bishop_relevant_mask(sq);
        const int relevant_bits =
                popcount(mask);

        result.count =
                std::size_t{1} << relevant_bits;

        assert(result.count <= result.entries.size());

        for (std::size_t index = 0; index < result.count; ++index) {
            const Bitboard occupancy =
                    set_occupancy(index, mask);

            result.entries[index] = {
                .occupancy = occupancy,
                .attacks = generate_bishop_attacks(sq, occupancy)
            };
        }
        return result;
    }

    // Information needed to hash occupancies for one square.
    struct Magic {
        Bitboard mask{}; // relevant blocker squares
        Bitboard magic_number{}; // hash multiplier
        std::uint8_t shift{}; // number of low product bits discarded
        std::size_t offset{}; // starting position in the attack table
    };

    // Magic search algorithm
    //
    // For one square:
    // Generate the relevant mask.
    // Generate every occupancy.
    // Generate every correct attack.
    // Repeatedly generate candidate magic numbers.
    // Test whether every occupancy maps to a unique index (or the same index only when the attacks are identical).
    // Keep the first successful magic.

    [[nodiscard]]
    constexpr std::size_t magic_index(Bitboard occupancy, const Magic &magic) noexcept {
        assert(magic.shift < 64);

        occupancy &= magic.mask;
        occupancy *= magic.magic_number;
        occupancy >>= magic.shift;

        return static_cast<std::size_t>(occupancy);
    }

    [[nodiscard]]
    Magic find_rook_magic(Square sq);

    [[nodiscard]]
    Magic find_bishop_magic(Square sq);

    // Calculate one square's table size
    [[nodiscard]]
    constexpr std::size_t magic_table_size(Bitboard mask) noexcept {
        const auto relevant_bits =
                static_cast<std::size_t>(popcount(mask));

        return std::size_t{1} << relevant_bits;
    }

    // Calculate total packed rook-table size
    [[nodiscard]]
    constexpr std::size_t calculate_total_rook_attack_entries() noexcept {
        std::size_t total{0};

        for (std::size_t index = 0; index < to_index(Square::Count); ++index) {
            const auto sq =
                    static_cast<Square>(index);

            const Bitboard mask =
                    generate_rook_relevant_mask(sq);

            total += magic_table_size(mask);
        }

        return total;
    }

    // Calculate total packed bishop-table size
    [[nodiscard]]
    constexpr std::size_t calculate_total_bishop_attack_entries() noexcept {
        std::size_t total{0};

        for (std::size_t index = 0; index < to_index(Square::Count); ++index) {
            const auto sq =
                    static_cast<Square>(index);

            const Bitboard mask =
                    generate_bishop_relevant_mask(sq);

            total += magic_table_size(mask);
        }

        return total;
    }

    // Total of the sum of each square stored
    inline constexpr std::size_t TotalRookAttackEntries = calculate_total_rook_attack_entries();
    inline constexpr std::size_t TotalBishopAttackEntries = calculate_total_bishop_attack_entries();


    [[nodiscard]]
    constexpr std::array<Magic, to_index(Square::Count)> make_rook_magics() noexcept {
        std::array<Magic, to_index(Square::Count)> magics{};

        std::size_t current_offset{0};

        for (std::size_t index = 0; index < magics.size(); ++index) {
            const auto sq =
                    static_cast<Square>(index);
            const Bitboard mask =
                    generate_rook_relevant_mask(sq);

            const auto relevant_bits =
                    static_cast<std::uint8_t>(popcount(mask));

            magics[index] = {
                .mask = mask,
                .magic_number = RookMagicNumbers[index],
                .shift = static_cast<std::uint8_t>(64 - relevant_bits),
                .offset = current_offset
            };

            current_offset += magic_table_size(mask);
        }
        return magics;
    }

    [[nodiscard]]
    constexpr std::array<Magic, to_index(Square::Count)> make_bishop_magics() noexcept {
        std::array<Magic, to_index(Square::Count)> magics{};

        std::size_t current_offset{0};

        for (std::size_t index = 0; index < magics.size(); ++index) {
            const auto sq =
                    static_cast<Square>(index);
            const Bitboard mask =
                    generate_bishop_relevant_mask(sq);

            const auto relevant_bits =
                    static_cast<std::uint8_t>(popcount(mask));

            magics[index] = {
                .mask = mask,
                .magic_number = BishopMagicNumbers[index],
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
    constexpr RookAttackTable make_rook_attack_table() noexcept {
        RookAttackTable table{};

        for (std::size_t square_index = 0; square_index < to_index(Square::Count); ++square_index) {
            const auto sq = static_cast<Square>(square_index);

            const auto reference_table =
                    generate_rook_occupancy_attack_table(sq);

            const Magic &magic =
                    RookMagics[square_index];

            for (std::size_t entry_index = 0; entry_index < reference_table.count; ++entry_index) {
                const auto &entry =
                        reference_table.entries[entry_index];

                const std::size_t index =
                        magic_index(entry.occupancy, magic);

                table[magic.offset + index] = entry.attacks;
            }
        }
        return table;
    }

    [[nodiscard]]
    constexpr BishopAttackTable make_bishop_attack_table() noexcept {
        BishopAttackTable table{};

        for (std::size_t square_index = 0; square_index < to_index(Square::Count); ++square_index) {
            const auto sq = static_cast<Square>(square_index);

            const auto reference_table =
                    generate_bishop_occupancy_attack_table(sq);

            const Magic &magic =
                    BishopMagics[square_index];

            for (std::size_t entry_index = 0; entry_index < reference_table.count; ++entry_index) {
                const auto &entry =
                        reference_table.entries[entry_index];

                const std::size_t index =
                        magic_index(entry.occupancy, magic);

                table[magic.offset + index] =
                        entry.attacks;
            }
        }
        return table;
    }

    inline const auto RookAttackLookup = make_rook_attack_table();

    inline const auto BishopAttackLookup = make_bishop_attack_table();

    [[nodiscard]]
    inline Bitboard rook_attacks(Square sq, Bitboard occupancy) noexcept {
        assert(is_valid(sq));

        const std::size_t square_index =
                to_index(sq);

        const Magic &magic =
                RookMagics[square_index];

        const std::size_t index =
                magic_index(occupancy, magic);

        return
                RookAttackLookup[magic.offset + index];
    }

    [[nodiscard]]
    inline Bitboard bishop_attacks(Square sq, Bitboard occupancy) noexcept {
        assert(is_valid(sq));

        const std::size_t square_index =
                to_index(sq);

        const Magic &magic =
                BishopMagics[square_index];

        const std::size_t index =
                magic_index(occupancy, magic);

        return
                BishopAttackLookup[magic.offset + index];
    }

    [[nodiscard]]
    inline Bitboard queen_attacks(Square sq, Bitboard occupancy) noexcept {
        assert(is_valid(sq));

        return
                rook_attacks(sq, occupancy) |
                bishop_attacks(sq, occupancy);
    }
}
