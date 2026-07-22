cmake_minimum_required(VERSION 4.2)
project(Aribella)

set(CMAKE_CXX_STANDARD 20)

add_executable(Aribella main.cpp
        src/core/bitboard.cpp
        src/core/bitboard.h
        src/core/types.h
        src/core/constants.h
        src/engine/engine.cpp
        src/engine/engine.h
        src/position/move.cpp
        src/position/move.h
        src/position/fen.cpp
        src/position/fen.h
        src/position/position.cpp
        src/position/position.h
        src/position/zobrist.h
        src/movegen/movegen.cpp
        src/movegen/movegen.h
        src/movegen/attacks.cpp
        src/movegen/attacks.h
        tests/bitboard_tests.cpp)
