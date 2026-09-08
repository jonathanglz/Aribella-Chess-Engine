#include <iostream>

#include "src/movegen/movegen.h"
#include "src/position/position.h"
#include "src/movegen/move_list.h"
#include "src/movegen/perft.h"


using namespace Aribella;

int main() {
    std::cout << "Aribella Chess Engine \nVersion 0.1\n";

    Position position;
    position.set_start_position();

    pertf_divide(position, Color::White, 3);

    return 0;
}
