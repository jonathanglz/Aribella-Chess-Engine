#include <chrono>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <string_view>

#include "engine/game_state.h"
#include "movegen/perft.h"

using namespace Aribella;

int main()
{
    constexpr std::string_view StartFEN =
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR "
        "w KQkq - 0 1";

    constexpr int Depth = 5;
    constexpr std::uint64_t ExpectedNodes = 4'865'609ULL;

    GameState game;

    if (!game.set_from_fen(StartFEN)) {
        std::cerr << "Failed to load starting position.\n";
        return 1;
    }

    const auto start =
        std::chrono::steady_clock::now();

    const std::uint64_t nodes =
        perft(game, Depth);

    const auto end =
        std::chrono::steady_clock::now();

    const std::chrono::duration<double> elapsed =
        end - start;

    const double seconds = elapsed.count();

    const double nps =
        seconds > 0.0
            ? static_cast<double>(nodes) / seconds
            : 0.0;

    std::cout
        << "Aribella Perft Benchmark\n"
        << "------------------------\n"
        << "Position: Starting position\n"
        << "Depth:    " << Depth << '\n'
        << "Nodes:    " << nodes << '\n'
        << std::fixed << std::setprecision(3)
        << "Time:     " << seconds << " s\n"
        << std::setprecision(0)
        << "NPS:      " << nps << '\n';

    if (nodes != ExpectedNodes) {
        std::cerr
            << "\nERROR: Expected "
            << ExpectedNodes
            << " nodes.\n";

        return 1;
    }

    return 0;
}