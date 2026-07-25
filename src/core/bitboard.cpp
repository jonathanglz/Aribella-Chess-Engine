#include "bitboard.h"

#include <iostream>

namespace Aribella {
    void print_bitboard(Bitboard bb) {
        std::cout << '\n';

        for (int rank = 7; rank >= 0; --rank) {
            std::cout << rank + 1 << "  ";

            for (int file = 0; file < 8; ++file) {
                const auto sq = make_square(
                    static_cast<File>(file),
                    static_cast<Rank>(rank)
                );

                std::cout << (is_set(bb, sq) ? 'X' : '.') << ' ';
            }

            std::cout << '\n';
        }

        std::cout << "\n   A B C D E F G H\n\n";
    }
}
