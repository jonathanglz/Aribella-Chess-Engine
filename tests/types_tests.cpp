#include "../src/core/types.h"

using namespace Aribella;

// to_index
static_assert(to_index(Color::Count) == 2);
static_assert(to_index(PieceType::Count) == 6);
static_assert(to_index(Piece::Count) == 12);
static_assert(to_index(File::Count) == 8);
static_assert(to_index(Rank::Count) == 8);
static_assert(to_index(Square::Count) == 64);

// is_valid
static_assert(is_valid(Color::White) == true);
static_assert(is_valid(Color::Black) == true);
static_assert(is_valid(PieceType::Pawn) == true);
static_assert(is_valid(PieceType::King) == true);
static_assert(is_valid(Piece::WhitePawn) == true);
static_assert(is_valid(Piece::BlackPawn) == true);
static_assert(is_valid(Piece::WhiteKing) == true);
static_assert(is_valid(Piece::BlackKing) == true);
static_assert(is_valid(File::A) == true);
static_assert(is_valid(File::H) == true);
static_assert(is_valid(Rank::R1) == true);
static_assert(is_valid(Rank::R8) == true);
static_assert(is_valid(Square::A1) == true);
static_assert(is_valid(Square::H8) == true);

// ~operator
static_assert(~Color::White == Color::Black);
static_assert(~Color::Black == Color::White);

// color_of
static_assert(color_of(Piece::WhitePawn) == Color::White);
static_assert(color_of(Piece::BlackPawn) == Color::Black);
static_assert(color_of(Piece::WhiteKing) == Color::White);
static_assert(color_of(Piece::BlackKing) == Color::Black);

// type_of
static_assert(type_of(Piece::WhitePawn) == PieceType::Pawn);
static_assert(type_of(Piece::WhiteKing) == PieceType::King);
static_assert(type_of(Piece::BlackPawn) == PieceType::Pawn);
static_assert(type_of(Piece::BlackKing) == PieceType::King);

// make_piece
static_assert(make_piece(Color::White, PieceType::Pawn) == Piece::WhitePawn);
static_assert(make_piece(Color::White, PieceType::King) == Piece::WhiteKing);
static_assert(make_piece(Color::Black, PieceType::Pawn) == Piece::BlackPawn);
static_assert(make_piece(Color::Black, PieceType::King) == Piece::BlackKing);

// file_of
static_assert(file_of(Square::A1) == File::A);
static_assert(file_of(Square::H8) == File::H);

// rank_of
static_assert(rank_of(Square::A1) == Rank::R1);
static_assert(rank_of(Square::H8) == Rank::R8);

// make_square
static_assert(make_square(File::A, Rank::R1) == Square::A1);
static_assert(make_square(File::H, Rank::R8) == Square::H8);

// offset
static_assert(offset(Direction::North) == 8);
static_assert(offset(Direction::South) == -8);
static_assert(offset(Direction::East) == 1);
static_assert(offset(Direction::West) == -1);
static_assert(offset(Direction::NorthEast) == 9);
static_assert(offset(Direction::NorthWest) == 7);
static_assert(offset(Direction::SouthEast) == -7);
static_assert(offset(Direction::SouthWest) == -9);