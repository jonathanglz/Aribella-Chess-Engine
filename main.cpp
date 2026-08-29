#include <iostream>

#include "src/movegen/movegen.h"
#include "src/position/position.h"
#include "src/movegen/move_list.h"

using namespace Aribella;

int main() {

    std::cout << "Aribella Chess Engine \nVersion 0.1\n";

    for (std::size_t square_index = 0; square_index < to_index(Square::Count); ++square_index) {
        const auto sq = static_cast<Square>(square_index);

        const auto [entries, count] = generate_rook_occupancy_attack_table(sq);

        for (std::size_t entry_index = 0; entry_index < count; ++entry_index) {
            const auto&[occupancy, attacks] = entries[entry_index];

            const Bitboard lookup_attacks = rook_attacks(sq, occupancy);

            assert(lookup_attacks == attacks);
        }
    }

    for (std::size_t square_index = 0; square_index < to_index(Square::Count); ++square_index) {
        const auto sq = static_cast<Square>(square_index);

        const auto [entries, count] = generate_bishop_occupancy_attack_table(sq);

        for (std::size_t entry_index = 0; entry_index < count; ++entry_index) {
            const auto&[occupancy, attacks] = entries[entry_index];

            const Bitboard lookup_attacks = bishop_attacks(sq, occupancy);

            assert(lookup_attacks == attacks);
        }
    }

    return 0;
}
