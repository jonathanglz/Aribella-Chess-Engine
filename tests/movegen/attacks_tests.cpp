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
    
}
