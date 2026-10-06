#include <gtest/gtest.h>

#include "movegen/move.h"
#include "movegen/move_format.h"

using namespace Aribella;

TEST(MoveFormatTest, FormatQuietMove) {
    const Move move{
        Square::G1,
        Square::F3,
        MoveFlag::Quiet,
        PieceType::None
    };

    EXPECT_EQ(move_to_string(move), "g1f3");
}

TEST(MoveFormatTest, FormatCapture) {
    const Move move{
        Square::E4,
        Square::D5,
        MoveFlag::Capture,
        PieceType::None
    };

    EXPECT_EQ(move_to_string(move), "e4d5");
}


TEST(MoveFormatTest, FormatPromotion) {
    EXPECT_EQ(
        move_to_string({
            Square::A7,
            Square::A8,
            MoveFlag::Promotion,
            PieceType::Queen
            }),
        "a7a8q"
    );

    EXPECT_EQ(
        move_to_string({
            Square::A7,
            Square::A8,
            MoveFlag::Promotion,
            PieceType::Rook
            }),
        "a7a8r"
    );

    EXPECT_EQ(
        move_to_string({
            Square::A7,
            Square::A8,
            MoveFlag::Promotion,
            PieceType::Bishop
            }),
        "a7a8b"
    );

    EXPECT_EQ(
        move_to_string({
            Square::A7,
            Square::A8,
            MoveFlag::Promotion,
            PieceType::Knight
            }),
        "a7a8n"
    );
}


TEST(MoveFormatTest, FormatCapturePromotion) {
    const Move move{
        Square::A7,
        Square::B8,
        MoveFlag::PromotionCapture,
        PieceType::Queen
    };

    EXPECT_EQ(move_to_string(move), "a7b8q");
}


TEST(MoveFormatTest, FormatCastling) {
    const Move kingSide{
        Square::E1,
        Square::G1,
        MoveFlag::KingCastle,
        PieceType::None
    };

    const Move queenSide{
        Square::E1,
        Square::C1,
        MoveFlag::QueenCastle,
        PieceType::None
    };

    EXPECT_EQ(move_to_string(kingSide), "e1g1");
    EXPECT_EQ(move_to_string(queenSide), "e1c1");
}


TEST(MoveFormatTest, FormatNullMove) {
    const Move move{};

    EXPECT_EQ(move_to_string(move), "0000");
}
