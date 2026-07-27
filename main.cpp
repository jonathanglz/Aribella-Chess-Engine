#include "src/core/bitboard.h"

#include <iostream>

using namespace Aribella;

int main() {

    std::cout << "Aribella Chess Engine \nVersion 0.1\n";

    print_bitboard(generate_queen_attacks(Square::D4, Bitboard{0}));

    return 0;
}
