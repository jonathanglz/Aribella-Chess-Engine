#pragma once

#include <bit>
#include <cassert>
#include "constants.h"


namespace Aribella {
    void print_bitboard(Aribella::Bitboard bb);

    constexpr Bitboard square_bb(Square sq) noexcept {
        assert(is_valid(sq));
        return Bitboard{1} << to_index(sq);
    }

    constexpr int popcount(Bitboard bb) noexcept {
        return std::popcount(bb);
    }

    constexpr Square lsb(Bitboard bb) noexcept {
        assert(bb);
        return static_cast<Square>(std::countr_zero(bb));
    }

    constexpr Square pop_lsb(Bitboard &bb) noexcept {
        assert(bb);
        const Square sq = lsb(bb);
        bb &= bb - 1;

        return sq;
    }

    constexpr bool is_set(Bitboard bb, Square sq) noexcept {
        return (bb & square_bb(sq)) != 0;
    }

    constexpr void set_bit(Bitboard &bb, Square sq) noexcept {
        bb |= square_bb(sq);
    }

    constexpr void clear_bit(Bitboard &bb, Square sq) noexcept {
        bb &= ~square_bb(sq);
    }

    constexpr Bitboard file_mask(File file) noexcept {
        assert(is_valid(file));
        return FileMasks[to_index(file)];
    }

    constexpr Bitboard rank_mask(Rank rank) noexcept {
        assert(is_valid(rank));
        return RankMasks[to_index(rank)];
    }

    constexpr Bitboard shift_north(Bitboard bb) noexcept {
        return bb << offset(Direction::North);
    }

    constexpr Bitboard shift_south(Bitboard bb) noexcept {
        return bb >> -offset(Direction::South);
    }

    constexpr Bitboard shift_east(Bitboard bb) noexcept {
        return (bb & ~FileH) << offset(Direction::East);
    }

    constexpr Bitboard shift_west(Bitboard bb) noexcept {
        return (bb & ~FileA) >> -offset(Direction::West);
    }

    constexpr Bitboard shift_northeast(Bitboard bb) noexcept {
        return (bb & ~FileH) << offset(Direction::NorthEast);
    }

    constexpr Bitboard shift_northwest(Bitboard bb) noexcept {
        return (bb & ~FileA) << offset(Direction::NorthWest);
    }

    constexpr Bitboard shift_southeast(Bitboard bb) noexcept {
        return (bb & ~FileH) >> -offset(Direction::SouthEast);
    }

    constexpr Bitboard shift_southwest(Bitboard bb) noexcept {
        return (bb & ~FileA) >> -offset(Direction::SouthWest);
    }

    // See ChessProgramming Wiki - Pawn Attacks(Bitboards)
    constexpr Bitboard pawn_attacks(Color c, Bitboard pawns) noexcept {
        assert(is_valid(c));

        return c == Color::White
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
            const auto sq = static_cast<Square>(i);
            attacksTable[i] = generate_knight_attacks(sq);
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
            const auto sq = static_cast<Square>(i);
            attacksTable[i] = generate_king_attacks(sq);
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

    constexpr Bitboard shift(Bitboard bb, Direction direction) noexcept {
        switch (direction) {
            case Direction::North:
                return shift_north(bb);
            case Direction::South:
                return shift_south(bb);
            case Direction::East:
                return shift_east(bb);
            case Direction::West:
                return shift_west(bb);
            case Direction::NorthEast:
                return shift_northeast(bb);
            case Direction::NorthWest:
                return shift_northwest(bb);
            case Direction::SouthEast:
                return shift_southeast(bb);
            case Direction::SouthWest:
                return shift_southwest(bb);
        }
        return Bitboard{0};
    }

    constexpr Bitboard generate_attacks_in_direction(Square sq, Direction direction, Bitboard occupancy) noexcept {
        assert(is_valid(sq));

        Bitboard attacks{0};
        Bitboard current = square_bb(sq);

        while (true) {
            current = shift(current, direction);

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
    constexpr Bitboard
    generate_relevant_mask_in_direction(Square sq, Direction direction) noexcept {
        assert(is_valid(sq));

        Bitboard mask{0};
        Bitboard current = square_bb(sq);

        while (true) {
            current = shift(current, direction);

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


    constexpr Bitboard set_occupancy(std::size_t index, Bitboard attack_mask) noexcept {
        Bitboard occupancy{0};

        const int bit_count = popcount(attack_mask);

        for (int bit = 0; bit < bit_count; ++bit) {
            const Square square = pop_lsb(attack_mask);

            if (index & (std::size_t{1} << bit)) {
                occupancy |= square_bb(square);
            }
        }

        return occupancy;
    }

    // One entry pair
    struct MagicEntry {
        Bitboard occupancy{};
        Bitboard attacks{};
    };

    // Set of entry pairs
    template<std::size_t Capacity>
    struct OccupancyAttackTable {
        std::array<MagicEntry, Capacity> entries{};
        std::size_t count{};
    };

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
}
