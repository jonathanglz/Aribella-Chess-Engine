#include <gtest/gtest.h>
#include <cstdint>
#include <utility>
#include <array>

#include "../../src/engine/game_state.h"
#include "../../src/movegen/perft.h"
#include "../../src/movegen/movegen.h"

using namespace Aribella;

namespace {
    class StartingPositionPerftTest : public ::testing::TestWithParam<std::pair<int, std::uint64_t> > {
    };

    class KiwipetePerftTest : public ::testing::TestWithParam<std::pair<int, std::uint64_t> > {
    };

    struct CastlingTestCase {
        const char *fen;
        Color side;
        Move move;

        Square rook_from;
        Square rook_to;

        CastlingRights kingside_right;
        CastlingRights queenside_right;
    };

    class CastlingTest : public ::testing::TestWithParam<CastlingTestCase> {
    };
}

TEST_P(StartingPositionPerftTest, CorrectNodeCount) {
    const auto [depth, expected_nodes] = GetParam();

    GameState game;

    ASSERT_TRUE(game.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"));

    EXPECT_EQ(perft(game, depth), expected_nodes);
}

INSTANTIATE_TEST_SUITE_P(StandardDepths, StartingPositionPerftTest, ::testing::Values(
                             std::pair<int, std::uint64_t>{1, 20},
                             std::pair<int, std::uint64_t>{2, 400},
                             std::pair<int, std::uint64_t>{3, 8902},
                             std::pair<int, std::uint64_t>{4, 197281}
                         ));

TEST_P(KiwipetePerftTest, CorrectNodeCOunt) {
    const auto [depth, expected_nodes] = GetParam();

    GameState game;

    ASSERT_TRUE(game.set_from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"));

    EXPECT_EQ(perft(game, depth), expected_nodes);
}

INSTANTIATE_TEST_SUITE_P(StandardDepths, KiwipetePerftTest, ::testing::Values(
                             std::pair<int, std::uint64_t>{1, 48},
                             std::pair<int, std::uint64_t>{2, 2039},
                             std::pair<int, std::uint64_t>{3, 97862}
                         ));

TEST(PerftTest, EnPassantAvailable) {
    GameState game;

    ASSERT_TRUE(game.set_from_fen("4k3/8/8/3pP3/8/8/8/4K3 w - d6 0 1"));

    EXPECT_EQ(perft(game, 1), 7ULL);
}

TEST(PerfTest, EnPassantCannotExposeKing) {
    GameState game;

    ASSERT_TRUE(game.set_from_fen("k3r3/8/8/3pP3/8/8/8/4K3 w - d6 0 1"));

    EXPECT_EQ(perft(game, 1), 6ULL);
}

TEST(PerftTest, GeneratesAllPromotionMoves) {
    GameState game;

    ASSERT_TRUE(game.set_from_fen("7k/P7/8/8/8/8/8/4K3 w - - 0 1"));

    EXPECT_EQ(perft(game, 1), 9ULL);
}

TEST(PerftTest, GeneratesAllCapturePromotions) {
    GameState game;

    ASSERT_TRUE(game.set_from_fen("1r5k/P7/8/8/8/8/8/4K3 w - - 0 1"));

    EXPECT_EQ(perft(game, 1), 13ULL);
}

TEST(MoveTest, CapturePromotionsRestorePositon) {
    Position position;

    ASSERT_TRUE(position.set_from_fen("1r5k/P7/8/8/8/8/8/4K3 w - - 0 1"));

    const Position original = position;
    const auto original_key = position.key();

    const std::array promotions{
        PieceType::Queen,
        PieceType::Rook,
        PieceType::Bishop,
        PieceType::Knight
    };

    for (const PieceType promotion: promotions) {
        const Move move{
            Square::A7,
            Square::B8,
            MoveFlag::PromotionCapture,
            promotion
        };

        // Verify the move was generated as legal.
        const MoveList legal_moves = generate_legal_moves(position, Color::White);

        bool found = false;

        for (const Move &legal_move: legal_moves) {
            if (legal_move == move) {
                found = true;
                break;
            }
        }

        ASSERT_TRUE(found);

        // Make the capture promotion.
        const UndoState undo = position.make_move(move);

        // The pawn should disappear from A7.
        EXPECT_FALSE(position.is_occupied(Square::A7));

        // B8 should contain the promoted white piece.
        EXPECT_NE(position.pieces(Color::White, promotion) & square_bb(Square::B8), 0ULL);

        // The captured black rook should be removed.
        EXPECT_EQ(position.pieces(Color::Black, PieceType::Rook) & square_bb(Square::B8), 0ULL);

        // Restore the original position.
        position.unmake_move(move, undo);

        EXPECT_EQ(position, original);
        EXPECT_EQ(position.key(), original_key);
    }
}

TEST(MoveTest, EnPassantRestoresPosition) {
    Position position;

    ASSERT_TRUE(position.set_from_fen("4k3/8/8/3pP3/8/8/8/4K3 w - d6 0 1"));

    const Position original = position;
    const auto original_key = position.key();

    const Move move{
        Square::E5,
        Square::D6,
        MoveFlag::EnPassant,
        PieceType::None
    };

    // Confirm that en passant is a legal move.
    const MoveList legal_moves =
            generate_legal_moves(position, Color::White);

    bool found = false;

    for (const Move &legal_move: legal_moves) {
        if (legal_move == move) {
            found = true;
            break;
        }
    }

    ASSERT_TRUE(found);

    // Execute en passant.
    const UndoState undo = position.make_move(move);

    // White's pawn must leave E5.
    EXPECT_FALSE(position.is_occupied(Square::E5));

    // White's pawn must arrive on D6.
    EXPECT_NE(position.pieces(Color::White, PieceType::Pawn)& square_bb(Square::D6), 0ULL);

    // Black's captured pawn must disappear from D5.
    EXPECT_FALSE(position.is_occupied(Square::D5));

    // The en passant target must be cleared.
    EXPECT_EQ(position.en_passant_square(), Square::None);

    // Restore the original position.
    position.unmake_move(move, undo);

    EXPECT_EQ(position, original);
    EXPECT_EQ(position.key(), original_key);
}

TEST(MoveTest, KingsideCastlingRestoresPosition) {
    Position position;

    ASSERT_TRUE(position.set_from_fen("4k3/8/8/8/8/8/8/R3K2R w KQ - 0 1"));

    const Position original = position;
    const auto original_key = position.key();

    const Move move{
        Square::E1,
        Square::G1,
        MoveFlag::KingCastle,
        PieceType::None
    };

    // Verify that kingside castling is legal.
    const MoveList legal_moves = generate_legal_moves(position, Color::White);

    bool found = false;

    for (const Move &legal_move: legal_moves) {
        if (legal_move == move) {
            found = true;
            break;
        }
    }

    ASSERT_TRUE(found);

    // Execute castling.
    const UndoState undo = position.make_move(move);

    // Verify the king moved from E1 to G1.
    EXPECT_FALSE(position.is_occupied(Square::E1));

    EXPECT_NE(position.pieces(Color::White, PieceType::King)& square_bb(Square::G1), 0ULL);

    // Verify the rook moved from H1 to F1.
    EXPECT_FALSE(position.is_occupied(Square::H1));

    EXPECT_NE(position.pieces(Color::White, PieceType::Rook) & square_bb(Square::F1), 0ULL);

    // Castling rights should be removed.
    EXPECT_FALSE(position.can_castle(CastlingRights::WhiteKingSide));

    EXPECT_FALSE(position.can_castle(CastlingRights::WhiteQueenSide));

    // Restore the original position.
    position.unmake_move(move, undo);

    EXPECT_EQ(position, original);
    EXPECT_EQ(position.key(), original_key);
}

TEST_P(CastlingTest, MakeUnmakeRestoresPosition) {
    const auto &test = GetParam();

    Position position;
    ASSERT_TRUE(position.set_from_fen(test.fen));

    const Position original = position;

    // Confirm castling is legal.
    const MoveList legal_moves = generate_legal_moves(position, test.side);

    bool found = false;

    for (const Move &legal_move: legal_moves) {
        if (legal_move == test.move) {
            found = true;
            break;
        }
    }

    ASSERT_TRUE(found);

    // Execute castling.
    const UndoState undo = position.make_move(test.move);

    // Verify the king's destination.
    EXPECT_FALSE(position.is_occupied(test.move.from));

    EXPECT_NE(position.pieces(test.side, PieceType::King) & square_bb(test.move.to), 0ULL);

    // Verify the rook's destination.
    EXPECT_FALSE(position.is_occupied(test.rook_from));

    EXPECT_NE(position.pieces(test.side, PieceType::Rook) & square_bb(test.rook_to), 0ULL);

    // Castling must remove both rights for the moving side.
    EXPECT_FALSE(position.can_castle(test.kingside_right));
    EXPECT_FALSE(position.can_castle(test.queenside_right));

    // Undo castling.
    position.unmake_move(test.move, undo);

    EXPECT_EQ(position, original);
    EXPECT_EQ(position.key(), original.key());
}

INSTANTIATE_TEST_SUITE_P(AllCastlingDirections, CastlingTest, ::testing::Values(
                             // White kingside
                             CastlingTestCase{
                             "4k3/8/8/8/8/8/8/R3K2R w KQ - 0 1",
                             Color::White,
                             {
                             Square::E1,
                             Square::G1,
                             MoveFlag::KingCastle,
                             PieceType::None
                             },
                             Square::H1,
                             Square::F1,
                             CastlingRights::WhiteKingSide,
                             CastlingRights::WhiteQueenSide
                             },

                             // White queenside
                             CastlingTestCase{
                             "4k3/8/8/8/8/8/8/R3K2R w KQ - 0 1",
                             Color::White,
                             {
                             Square::E1,
                             Square::C1,
                             MoveFlag::QueenCastle,
                             PieceType::None
                             },
                             Square::A1,
                             Square::D1,
                             CastlingRights::WhiteKingSide,
                             CastlingRights::WhiteQueenSide
                             },

                             // Black kingside
                             CastlingTestCase{
                             "r3k2r/8/8/8/8/8/8/4K3 b kq - 0 1",
                             Color::Black,
                             {
                             Square::E8,
                             Square::G8,
                             MoveFlag::KingCastle,
                             PieceType::None
                             },
                             Square::H8,
                             Square::F8,
                             CastlingRights::BlackKingSide,
                             CastlingRights::BlackQueenSide
                             },

                             // Black queenside
                             CastlingTestCase{
                             "r3k2r/8/8/8/8/8/8/4K3 b kq - 0 1",
                             Color::Black,
                             {
                             Square::E8,
                             Square::C8,
                             MoveFlag::QueenCastle,
                             PieceType::None
                             },
                             Square::A8,
                             Square::D8,
                             CastlingRights::BlackKingSide,
                             CastlingRights::BlackQueenSide
                             }
                         )
);
