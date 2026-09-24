cmake_minimum_required(VERSION 4.2)
project(Aribella)

set(CMAKE_CXX_STANDARD 20)

add_executable(Aribella main.cpp
        src/core/bitboard.cpp
        src/core/bitboard.h
        src/core/types.h
        src/core/constants.h
        src/movegen/move.h
        src/position/position.cpp
        src/position/position.h
        src/position/zobrist.h
        src/movegen/movegen.cpp
        src/movegen/movegen.h
        src/movegen/attacks.h
        src/movegen/magic_constants.h
        src/movegen/move_list.h
        src/movegen/attacks.cpp
        src/position/undo.h
        src/movegen/perft.h
        src/movegen/perft.cpp
        src/core/random.h
        src/engine/game_state.h
        src/engine/game_state.cpp
        src/search/transposition_table.h
        src/search/transposition_table.cpp
        src/evaluation/evaluation.h
        src/evaluation/evaluation.cpp
        src/search/search.h
        src/search/search.cpp
        src/movegen/move_format.h
        src/movegen/move_format.cpp
        src/uci/uci.h
        src/uci/uci.cpp
)
