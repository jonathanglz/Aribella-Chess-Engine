#include "src/core/bitboard.h"

#include <iostream>

using namespace Aribella;

int main() {

    std::cout << "Aribella Chess Engine \nVersion 0.1\n";

    Bitboard board = generate_rook_relevant_mask(Square::D4);

    std::cout << popcount(board);

    return 0;
}
