#include <iostream>

#include "src/movegen/movegen.h"
#include "src/position/position.h"
#include "src/movegen/move_list.h"
#include "src/movegen/perft.h"

using namespace Aribella;

int main() {
    std::cout << "Aribella Chess Engine \nVersion 0.1\n";

    Position position;

    assert(position.set_from_fen(
        "4k3/8/8/8/8/8/8/4K1N1 "
        "w - - 0 1"
    ));

    print_bitboard(position.occupancy());

    const Move move{
        .from = Square::G1,
        .to = Square::F3,
        .flag = MoveFlag::Quiet
    };

    const UndoState undo =
        position.make_move(move);

    const auto incremental_key =
        position.key();

    position.recompute_key();

    assert(
        position.key()
        == incremental_key
    );

    position.unmake_move(move, undo);

    assert(
        position.key()
        == undo.key
    );

    return 0;
}
