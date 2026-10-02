cmake_minimum_required(VERSION 3.20)

project(Aribella)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

add_library(aribella_engine STATIC
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
        src/engine/engine.h
        src/engine/engine.cpp
)

target_include_directories(aribella_engine
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(aribella_engine
        PUBLIC
            Threads::Threads
)

add_executable(Aribella
        main.cpp
)

target_link_libraries(Aribella
        PRIVATE
            aribella_engine
)

include(FetchContent)

FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(aribella_tests
        tests/evaluation/evaluation_tests.cpp
        tests/movegen/perft_tests.cpp
)

target_link_libraries(aribella_tests
        PRIVATE
            GTest::gtest_main
            aribella_engine
)

include(GoogleTest)

gtest_discover_tests(aribella_tests)