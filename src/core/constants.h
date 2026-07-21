#pragma once

#include "types.h"

namespace Aribella {

    // Little Endian Rank File (LERF) constants from
    // https://www.chessprogramming.org/Square_Mapping_Considerations
    inline constexpr Bitboard FileA{0x0101010101010101ULL};
    inline constexpr Bitboard FileB = FileA << 1;
    inline constexpr Bitboard FileC = FileA << 2;
    inline constexpr Bitboard FileD = FileA << 3;
    inline constexpr Bitboard FileE = FileA << 4;
    inline constexpr Bitboard FileF = FileA << 5;
    inline constexpr Bitboard FileG = FileA << 6;
    inline constexpr Bitboard FileH = FileA << 7;

    inline constexpr Bitboard Rank1{0x00000000000000FFULL};
    inline constexpr Bitboard Rank2 = Rank1 << 8;
    inline constexpr Bitboard Rank3 = Rank2 << 16;
    inline constexpr Bitboard Rank4 = Rank3 << 24;
    inline constexpr Bitboard Rank5 = Rank4 << 32;
    inline constexpr Bitboard Rank6 = Rank5 << 40;
    inline constexpr Bitboard Rank7 = Rank6 << 48;
    inline constexpr Bitboard Rank8 = Rank7 << 56;

}