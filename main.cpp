#include <iostream>

#include "src/movegen/movegen.h"
#include "src/position/position.h"
#include "src/movegen/move_list.h"

using namespace Aribella;

int main() {
    std::cout << "Aribella Chess Engine \nVersion 0.1\n";


    print_bitboard(Bitboard{0});

    return 0;
}
