#include "src/core/bitboard.h"

#include <iostream>

using namespace Aribella;

int main() {

    std::cout << "Aribella Chess Engine \nVersion 0.1\n";

    print_bitboard(generate_attacks_in_direction(Square::E4, Direction::South, square_bb(Square::E7)));

    return 0;
}
