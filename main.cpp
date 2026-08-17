#include <iostream>

#include "src/movegen/magic.h"
#include "src/position/position.h"

using namespace Aribella;

int main() {

    std::cout << "Aribella Chess Engine \nVersion 0.1\n";

    Position p;
    p.set_piece(Color::White, PieceType::Rook, Square::D4);
    p.set_piece(Color::Black, PieceType::Queen, Square::E8);


   Color c = p.color_at(Square::E8);

    std::cout << to_index(c) << std::endl;

    return 0;
}
