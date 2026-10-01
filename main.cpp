#include <iostream>

#include "src/engine/game_state.h"
#include "src/evaluation/evaluation.h"
#include "src/movegen/movegen.h"
#include "src/movegen/move_format.h"
#include "src/position/position.h"
#include "src/movegen/move_list.h"
#include "src/movegen/perft.h"
#include "src/search/search.h"
#include "src/search/transposition_table.h"
#include "src/uci/uci.h"


using namespace Aribella;

int main() {
    std::cout << "Aribella Chess Engine \nVersion 0.1\n";

    // Aribella::run_uci();

    Position position;
    position.set_from_fen("4k3/8/2b5/8/8/8/8/4K3 w - - 0 1");

    std::cout << evaluate(position) << std::endl;

    return 0;
}
