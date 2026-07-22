#pragma once

#include <array>

#include "types.h"

namespace Aribella {

    // Little Endian Rank File (LERF)
    // See: Chessprogramming Wiki - Square_Mapping_Considerations
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
    inline constexpr Bitboard Rank3 = Rank1 << 16;
    inline constexpr Bitboard Rank4 = Rank1 << 24;
    inline constexpr Bitboard Rank5 = Rank1 << 32;
    inline constexpr Bitboard Rank6 = Rank1 << 40;
    inline constexpr Bitboard Rank7 = Rank1 << 48;
    inline constexpr Bitboard Rank8 = Rank1 << 56;

    inline constexpr std::array<Bitboard, to_index(File::Count)> FileMasks {
        FileA, FileB, FileC, FileD,
        FileE, FileF, FileG, FileH
    };

    inline constexpr std::array<Bitboard, to_index(Rank::Count)> RankMasks {
        Rank1, Rank2, Rank3, Rank4,
        Rank5, Rank6, Rank7, Rank8
    };

}