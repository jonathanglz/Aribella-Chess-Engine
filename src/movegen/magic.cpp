#include "magic.h"

#include <cassert>
#include <cstdint>

namespace Aribella {
    namespace {
        class Xorshift64Star {
        private:
            std::uint64_t state;

        public:
            explicit Xorshift64Star(std::uint64_t seed) noexcept : state{seed} { assert(seed != 0); }

            [[nodiscard]]
            std::uint64_t next() noexcept {
                std::uint64_t value = state;

                value ^= value >> 12;
                value ^= value << 25;
                value ^= value >> 27;

                state = value;

                // Sebastian Vignas 0x2545F4914F6CDD1DULL hexadecimal number
                return value * 0x2545F4914F6CDD1DULL;
            }
        };

        // Golden ratio 0x9E3779B97F4A7C15ULL hexadecimal number
        Xorshift64Star RandomGenerator{0x9E3779B97F4A7C15ULL};

        [[nodiscard]]
        Bitboard random_bitboard() noexcept {
            return RandomGenerator.next();
        }

        [[nodiscard]]
        Bitboard random_sparse_bitboard() noexcept {
            return random_bitboard()
                   & random_bitboard()
                   & random_bitboard();
        }

        template<std::size_t Capacity>
        [[nodiscard]]
        Bitboard find_magic(Bitboard relevant_mask, const OccupancyAttackTable<Capacity> &reference_table,
                            std::uint8_t shift) {
            assert(reference_table.count <= Capacity);
            assert(shift < 64);

            // to remember which attack bitboard was stored at each used index
            std::array<Bitboard, Capacity> used_attacks{};
            // to remember if magic index has already been reached
            // used_indices[10] = true
            std::array<bool, Capacity> used_indices{};

            while (true) {
                // random sparse 64-bit number that becomes candidate magic number
                const Bitboard candidate = random_sparse_bitboard();

                // Reject candidates that do no create enough variation in high bytes
                if (popcount((relevant_mask * candidate) & 0xFF00000000000000ULL) < 6) {
                    continue;
                }

                // every candidate must be tested from clean state
                used_attacks.fill(Bitboard{0});
                used_indices.fill(false);

                // temporary SMagic object for candidate currently being tested
                const SMagic magic{
                    .mask = relevant_mask,
                    .magic = candidate,
                    .shift = shift
                };

                // Assume that it is a valid candidate. If bad collision is found set to false.
                bool valid = true;

                // Test every occupancy/attack pair for selected square
                for (std::size_t i = 0; i < reference_table.count; i++) {
                    // retrieve one known-correct pair(entry.occupancy, entry.attacks)
                    const OccupancyAttackEntry &entry = reference_table.entries[i];

                    // calculate the magic index by calling magic_index helper
                    const std::size_t index = magic_index(entry.occupancy, magic);

                    if (index >= Capacity) {
                        valid = false;
                        break;
                    }

                    // if current index has no previous occupancy, then claim current index
                    // else if this index maps to a different attack then set to false/break.
                    if (!used_indices[index]) {
                        used_indices[index] = true;
                        used_attacks[index] = entry.attacks;
                    } else if (used_indices[index] != entry.attacks) {
                        valid = false;
                        break;
                    }
                }

                // return candidate magic number
                if (valid) {
                    return candidate;
                }
            }
        }
    }

    SMagic find_rook_magic(Square sq) {
        assert(is_valid(sq));

        // find all blocker positions that can affect rook on sq
        const Bitboard mask = generate_rook_relevant_mask(sq);

        // generate all reference occupancies/attacks pair
        const auto reference_table = generate_rook_occupancy_attack_table(sq);

        // counts the number of squares in the relevant blocker mask
        const auto relevant_bits = static_cast<std::uint8_t>(popcount(mask));

        // calculate the shift
        const auto shift = static_cast<std::uint8_t>(64 - relevant_bits);

        // search for the rook magic number
        const Bitboard magic_number = find_magic(mask, reference_table, shift);

        return SMagic{
            .mask = mask,
            .magic = magic_number,
            .shift = shift
        };
    }

    SMagic find_bishop_magic(Square sq) {
        assert(is_valid(sq));

        // find all blocker positions that can affect bishop on sq
        const Bitboard mask = generate_bishop_relevant_mask(sq);

        // generate all reference occupancies/attacks pair
        const auto reference_table = generate_bishop_occupancy_attack_table(sq);

        // counts the number of squares in the relevant blocker mask
        const auto relevant_bits = static_cast<std::uint8_t>(popcount(mask));

        // calculate the shift
        const auto shift = static_cast<std::uint8_t>(64 - relevant_bits);

        // search for the bishop magic number
        const Bitboard magic_number = find_magic(mask, reference_table, shift);

        return SMagic{
            .mask = mask,
            .magic = magic_number,
            .shift = shift
        };
    }
}
