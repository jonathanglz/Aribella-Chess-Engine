#include <iostream>

#include "src/movegen/magic.h"
#include "src/position/position.h"

using namespace Aribella;

int main() {

    std::cout << "Aribella Chess Engine \nVersion 0.1\n";

    Position p;
    p.set_piece(Color::White, PieceType::Rook, Square::D4);

    print_bitboard(p.occupancy());

    p.clear();

    print_bitboard(p.occupancy());

    return 0;
}
