#include <iostream>

#include "src/uci/uci.h"

using namespace Aribella;

int main() {
    std::cout << "Aribella Chess Engine v1.0\n";

    Aribella::run_uci();

    return 0;
}
