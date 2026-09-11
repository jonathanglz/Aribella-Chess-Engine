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
        "4k3/8/8/8/8/8/4P3/4K3 "
        "w - - 0 1"
    ));

    const Move move{
        .from = Square::E2,
        .to = Square::E4,
        .flag = MoveFlag::DoublePawnPush,
        .promotion = PieceType::None
    };

    const UndoState undo =
        position.make_move(move);

    assert(
        position.en_passant_square()
        == Square::E3
    );

    const auto incremental_key =
        position.key();

    position.recompute_key();

    assert(
        position.key()
        == incremental_key
    );

    position.unmake_move(
        move,
        undo
    );

    assert(
        position.key()
        == undo.key
    );

    return 0;
}
