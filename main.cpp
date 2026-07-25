#include "src/core/bitboard.h"

#include <iostream>

using namespace Aribella;

int main() {

    std::cout << "Aribella Chess Engine \nVersion 0.1\n";

    print_bitboard(knight_attacks(Square::A3));

    return 0;
}
