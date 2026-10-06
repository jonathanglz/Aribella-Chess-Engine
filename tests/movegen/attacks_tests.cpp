#include <gtest/gtest.h>

#include "../../src/core/bitboard.h"
#include "../../src/core/types.h"
#include "../../src/movegen/attacks.h"

using namespace Aribella;

namespace {
    constexpr Bitboard squares(std::initializer_list<Square> list) {
        Bitboard result = 0;

        for (const Square sq: list) {
            result |= square_bb(sq);
        }

        return result;
    }
}

TEST(AttackTest, PawnAttacksWhite) {
    const Bitboard pawns = square_bb(Square::E4);

    const Bitboard expected = squares({Square::D5, Square::F5});

    EXPECT_EQ(pawn_attacks(Color::White, pawns), expected);
}

TEST(AttackTest, PawnAttacksBlack) {
    const Bitboard pawns = square_bb(Square::E5);

    const Bitboard expected = squares({Square::D4, Square::F4});

    EXPECT_EQ(pawn_attacks(Color::Black, pawns), expected);
}

TEST(AttackTest, PawnAttackAtBoardEdges) {
    EXPECT_EQ(pawn_attacks(Color::White, square_bb(Square::A4)), square_bb(Square::B5));

    EXPECT_EQ(pawn_attacks(Color::White, square_bb(Square::H4)), square_bb(Square::G5));

    EXPECT_EQ(pawn_attacks(Color::Black, square_bb(Square::A5)), square_bb(Square::B4));

    EXPECT_EQ(pawn_attacks(Color::Black, square_bb(Square::H5)), square_bb(Square::G4));
}

TEST(AttackTest, KnightAttacksFromCenter) {
    const Bitboard expected = squares({
        Square::C3,
        Square::E3,
        Square::B4,
        Square::F4,
        Square::B6,
        Square::F6,
        Square::C7,
        Square::E7
    });

    EXPECT_EQ(knight_attacks(Square::D5), expected);
}

TEST(AttackTest, KnigtAttacksFromCorner) {
    const Bitboard expected = squares({
        Square::B3,
        Square::C2
    });

    EXPECT_EQ(knight_attacks(Square::A1), expected);
}

TEST(AttacksTest, KnightAttacksAtBoardEdges) {
    const Bitboard expected = squares({
        Square::A3,
        Square::C3,
        Square::D2
    });

    EXPECT_EQ(knight_attacks(Square::B1), expected);
}

TEST(AttacksTest, KingAttacksFromCenter) {
    const Bitboard expected = squares({
        Square::D3,
        Square::E3,
        Square::F3,
        Square::D4,
        Square::F4,
        Square::D5,
        Square::E5,
        Square::F5
    });

    EXPECT_EQ(king_attacks(Square::E4), expected);
}

TEST(AttacksTest, KingAttacksFromCorner) {
    const Bitboard expected = squares({
        Square::A2,
        Square::B1,
        Square::B2
    });

    EXPECT_EQ(king_attacks(Square::A1), expected);
}

TEST(AttacksTest, KingAttacksAtBoardEdges) {
    const Bitboard expected = squares({
        Square::A1,
        Square::C1,
        Square::A2,
        Square::B2,
        Square::C2
    });

    EXPECT_EQ(king_attacks(Square::B1), expected);
}

TEST(AttacksTest, BishopAttacksOnEmptyBoard) {
    const Bitboard expected = squares({
        Square::C3,
        Square::B2,
        Square::A1,

        Square::E5,
        Square::F6,
        Square::G7,
        Square::H8,

        Square::C5,
        Square::B6,
        Square::A7,

        Square::E3,
        Square::F2,
        Square::G1
    });

    EXPECT_EQ(bishop_attacks(Square::D4, 0ULL), expected);
}

TEST(AttacksTest, BishopAttacksWithBlockers) {
    const Bitboard occupancy = squares({
        Square::F6,
        Square::B6,
        Square::F2
    });

    const Bitboard expected = squares({
        Square::C3,
        Square::B2,
        Square::A1,

        Square::E5,
        Square::F6,

        Square::C5,
        Square::B6,

        Square::E3,
        Square::F2
    });

    EXPECT_EQ(bishop_attacks(Square::D4, occupancy), expected);
}

TEST(AttacksTest, RookAttacksOnEmptyBoard) {
    const Bitboard expected = squares({
        Square::D1,
        Square::D2,
        Square::D3,
        Square::D5,
        Square::D6,
        Square::D7,
        Square::D8,

        Square::A4,
        Square::B4,
        Square::C4,
        Square::E4,
        Square::F4,
        Square::G4,
        Square::H4
    });

    EXPECT_EQ(rook_attacks(Square::D4, 0ULL), expected);
}

TEST(AttacksTest, RookAttacksWithBlockers) {
    const Bitboard occupancy = squares({
        Square::D6,
        Square::D2,
        Square::B4,
        Square::F4
    });

    const Bitboard expected = squares({
        Square::D3,
        Square::D2,
        Square::D5,
        Square::D6,

        Square::C4,
        Square::B4,
        Square::E4,
        Square::F4
    });

    EXPECT_EQ(rook_attacks(Square::D4, occupancy), expected);
}

TEST(AttacksTest, QueenAttacksOnEmptyBoard) {
    const Bitboard expected =
            rook_attacks(Square::D4, 0ULL) |
            bishop_attacks(Square::D4, 0ULL);

    EXPECT_EQ(queen_attacks(Square::D4, 0ULL), expected);

    EXPECT_EQ(popcount(expected), 27);
}

TEST(AttacksTest, QueenAttacksWithBlockers) {
    const Bitboard occupancy = squares({
        Square::D6,
        Square::B4,
        Square::F6,
        Square::B2
    });

    const Bitboard expected = squares({
        // Rook directions
        Square::D1,
        Square::D2,
        Square::D3,
        Square::D5,
        Square::D6,
        Square::C4,
        Square::B4,
        Square::E4,
        Square::F4,
        Square::G4,
        Square::H4,

        // Bishop directions
        Square::C3,
        Square::B2,
        Square::E5,
        Square::F6,
        Square::C5,
        Square::B6,
        Square::A7,
        Square::E3,
        Square::F2,
        Square::G1
    });

    EXPECT_EQ(queen_attacks(Square::D4, occupancy), expected);
}
