#include <gtest/gtest.h>

#include <iostream>
#include <sstream>
#include <string>

#include "uci/uci.h"

using namespace Aribella;

namespace {
    std::string run_uci_commands(const std::string &commands) {
        std::istringstream input(commands);
        std::ostringstream output;

        std::streambuf *oldCin = std::cin.rdbuf(input.rdbuf());
        std::streambuf *oldCout = std::cout.rdbuf(output.rdbuf());

        run_uci();

        std::cin.rdbuf(oldCin);
        std::cout.rdbuf(oldCout);

        return output.str();
    }
}

TEST(UciTest, RespondsToUciCommand) {
    const std::string output =
            run_uci_commands(
                "uci\n"
                "quit\n"
            );

    EXPECT_NE(
        output.find("id name Aribella"),
        std::string::npos
    );

    EXPECT_NE(
        output.find("id author Jonathan Gonzalez"),
        std::string::npos
    );

    EXPECT_NE(
        output.find("uciok"),
        std::string::npos
    );
}

TEST(UciTest, RespondsToIsReady) {
    const std::string output =
            run_uci_commands(
                "isready\n"
                "quit\n"
            );

    EXPECT_NE(
        output.find("readyok"),
        std::string::npos
    );
}

TEST(UciTest, SetsStartingPosition) {
    const std::string output =
            run_uci_commands(
                "position startpos\n"
                "go depth 1\n"
                "quit\n"
            );

    EXPECT_EQ(
        output.find("Invalid position command"),
        std::string::npos
    );
}

TEST(UciTest, LoadsFenPosition) {
    const std::string output =
            run_uci_commands(
                "position fen "
                "7k/5Q2/6K1/8/8/8/8/8 "
                "b - - 0 1\n"
                "go depth 1\n"
                "quit\n"
            );

    EXPECT_EQ(
        output.find("Invalid position command"),
        std::string::npos
    );
}

TEST(UciTest, AppliesMoveSequence) {
    const std::string output =
            run_uci_commands(
                "position startpos moves "
                "e2e4 e7e5 g1f3\n"
                "go depth 1\n"
                "quit\n"
            );

    EXPECT_EQ(
        output.find("Invalid position command"),
        std::string::npos
    );
}

TEST(UciTest, StartsSearch) {
    const std::string output =
            run_uci_commands(
                "position startpos\n"
                "go depth 1\n"
                "stop\n"
                "quit\n"
            );

    EXPECT_NE(
        output.find("bestmove "),
        std::string::npos
    );
}

TEST(UciTest, StopsSearch)
{
    const std::string output =
        run_uci_commands(
            "position startpos\n"
            "go depth 10\n"
            "stop\n"
            "quit\n"
        );

    EXPECT_NE(
        output.find("bestmove "),
        std::string::npos
    );
}

TEST(UciTest, NewGameAllowsNewSearch)
{
    const std::string output =
        run_uci_commands(
            "position startpos moves e2e4 e7e5 g1f3\n"
            "ucinewgame\n"
            "go depth 1\n"
            "stop\n"
            "quit\n"
        );

    EXPECT_NE(
        output.find("bestmove "),
        std::string::npos
    );
}
