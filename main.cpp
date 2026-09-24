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

    Aribella::run_uci();

    return 0;
}
