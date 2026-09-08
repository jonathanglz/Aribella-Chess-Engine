#include <iostream>

#include "src/movegen/movegen.h"
#include "src/position/position.h"
#include "src/movegen/move_list.h"

using namespace Aribella;

int main() {
    std::cout << "Aribella Chess Engine \nVersion 0.1\n";

    print_bitboard(Bitboard{0});

    Position position;

    position.set_piece(Color::White, PieceType::King, Square::E1);
    position.set_piece(Color::White, PieceType::Rook, Square::E2);
    position.set_piece(Color::Black, PieceType::Rook, Square::E8);
    position.set_piece(Color::Black, PieceType::King, Square::A8);

    MoveList legal = generate_legal_moves(position, Color::White);

    for (std::size_t i = 0; i < legal.size(); ++i) {
        assert(!(legal[i].from == Square::E2 && legal[i].to == Square::D2));
    }

    return 0;
}
