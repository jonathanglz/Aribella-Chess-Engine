#include <gtest/gtest.h>

#include "../../src/core/bitboard.h"

using namespace Aribella;

TEST(BitboardTest, SquareBitboard) {
    EXPECT_EQ(square_bb(Square::A1), 1ULL);
    EXPECT_EQ(square_bb(Square::B1), 2ULL);
    EXPECT_EQ(square_bb(Square::A2), 1ULL << 8);
    EXPECT_EQ(square_bb(Square::H8), 1ULL << 63);
}

TEST(BitboardTest, SetBit) {
    Bitboard board = 0ULL;

    set_bit(board, Square::A1);
    set_bit(board, Square::E4);

    EXPECT_NE(board &square_bb(Square::A1), 0ULL);
    EXPECT_NE(board &square_bb(Square::E4), 0ULL);

    EXPECT_EQ(popcount(board), 2);
}

TEST(BitboardTest, ClearBit) {
    Bitboard board = square_bb(Square::A1) | square_bb(Square::E4);

    clear_bit(board, Square::A1);

    EXPECT_EQ(board &square_bb(Square::A1), 0ULL);
    EXPECT_NE(board &square_bb(Square::E4), 0ULL);

    EXPECT_EQ(popcount(board), 1);
}

TEST(BitboardTest, Popcount) {
    EXPECT_EQ(popcount(0ULL), 0);
    EXPECT_EQ(popcount(square_bb(Square::A1)), 1);

    const Bitboard board = square_bb(Square::A1) | square_bb(Square::E4) | square_bb(Square::H8);

    EXPECT_EQ(popcount(board), 3);
    EXPECT_EQ(popcount(~Bitboard{0}), 64);
}

TEST(BitboardTest, LeastSignificantBit) {
    const Bitboard board = square_bb(Square::C3) | square_bb(Square::A1) | square_bb(Square::H8);

    EXPECT_EQ(lsb(board), Square::A1);
    EXPECT_EQ(lsb(square_bb(Square::H8)), Square::H8);
}

TEST(BitboardTest, PopLeastSignificantBit) {
    Bitboard board = square_bb(Square::H8) | square_bb(Square::C3) | square_bb(Square::A1);

    EXPECT_EQ(pop_lsb(board), Square::A1);
    EXPECT_EQ(popcount(board), 2);

    EXPECT_EQ(pop_lsb(board), Square::C3);
    EXPECT_EQ(popcount(board), 1);

    EXPECT_EQ(pop_lsb(board), Square::H8);
    EXPECT_EQ(popcount(board), 0ULL);
}

TEST(BitboardTest, NorthShift) {
    const Bitboard board = square_bb(Square::E4);

    const Bitboard result = shift(board, Direction::North);

    EXPECT_EQ(result, square_bb(Square::E5));
}

TEST(BitboardTest, SouthShift) {
    const Bitboard board = square_bb(Square::E4);

    const Bitboard result = shift(board, Direction::South);

    EXPECT_EQ(result, square_bb(Square::E3));
}

TEST(BitboardTest, EastShift) {
    const Bitboard board = square_bb(Square::E4);

    const Bitboard result = shift(board, Direction::East);

    EXPECT_EQ(result, square_bb(Square::F4));
}

TEST(BitboardTest, WestShift) {
    const Bitboard board = square_bb(Square::E4);

    const Bitboard result = shift(board, Direction::West);

    EXPECT_EQ(result, square_bb(Square::D4));
}

TEST(BitboardTest, NorthEastShift) {
    const Bitboard board = square_bb(Square::E4);

    const Bitboard result = shift(board, Direction::NorthEast);

    EXPECT_EQ(result, square_bb(Square::F5));
}

TEST(BitboardTest, NorthWestShift) {
    const Bitboard board = square_bb(Square::E4);

    const Bitboard result = shift(board, Direction::NorthWest);

    EXPECT_EQ(result, square_bb(Square::D5));
}

TEST(BitboardTest, SouthEastShift) {
    const Bitboard board = square_bb(Square::E4);

    const Bitboard result = shift(board, Direction::SouthEast);

    EXPECT_EQ(result, square_bb(Square::F3));
}

TEST(BitboardTest, SouthWestShift) {
    const Bitboard board = square_bb(Square::E4);

    const Bitboard result = shift(board, Direction::SouthWest);

    EXPECT_EQ(result, square_bb(Square::D3));
}

TEST(BitboardTest, ShiftDoesNotWrapAroundBoard) {
    const Bitboard a_file = square_bb(Square::A4);

    const Bitboard a_file_result = shift(a_file, Direction::West);

    EXPECT_EQ(a_file_result, 0ULL);

    const Bitboard h_file = square_bb(Square::H4);

    const Bitboard h_file_result = shift(h_file, Direction::East);

    EXPECT_EQ(h_file_result, 0ULL);
}

TEST(BitboardTest, ShiftEmptyBitboard) {
    constexpr Bitboard empty = 0ULL;

    EXPECT_EQ(shift(empty, Direction::North), 0ULL);
    EXPECT_EQ(shift(empty, Direction::South), 0ULL);
    EXPECT_EQ(shift(empty, Direction::East), 0ULL);
    EXPECT_EQ(shift(empty, Direction::West), 0ULL);

    EXPECT_EQ(shift(empty, Direction::NorthEast), 0ULL);
    EXPECT_EQ(shift(empty, Direction::NorthWest), 0ULL);
    EXPECT_EQ(shift(empty, Direction::SouthEast), 0ULL);
    EXPECT_EQ(shift(empty, Direction::SouthWest), 0ULL);
}

TEST(BitboardTest, ShiftMultiplePieces) {
    const Bitboard board = square_bb(Square::A2) | square_bb(Square::D4) | square_bb(Square::H7);

    const Bitboard expected = square_bb(Square::A3) | square_bb(Square::D5) | square_bb(Square::H8);

    const Bitboard result = shift(board, Direction::North);

    EXPECT_EQ(result, expected);
}