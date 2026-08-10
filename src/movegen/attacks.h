#pragma once


#include "../core/bitboard.h"

namespace Aribella {
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

}
