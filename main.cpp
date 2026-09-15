#include <iostream>

#include "src/engine/game_state.h"
#include "src/evaluation/evaluation.h"
#include "src/movegen/movegen.h"
#include "src/position/position.h"
#include "src/movegen/move_list.h"
#include "src/movegen/perft.h"
#include "src/search/search.h"
#include "src/search/transposition_table.h"

using namespace Aribella;

int main() {
    std::cout << "Aribella Chess Engine \nVersion 0.1\n";

    GameState game;

    assert(game.set_from_fen("4k3/8/8/8/8/8/4q3/4R1K1 w - - 0 1"));

    const SearchResult result = search(game, 1);
    
    std::cout
        << "Best move: "
        << static_cast<int>(
               to_index(result.best_move.from)
           )
        << " -> "
        << static_cast<int>(
               to_index(result.best_move.to)
           )
        << '\n';

    std::cout
        << "Score: "
        << result.score
        << '\n';

    assert(
        result.best_move.from
        == Square::E1
    );

    assert(
        result.best_move.to
        == Square::E2
    );

    assert(
        result.score == 500
    );

    return 0;
}
