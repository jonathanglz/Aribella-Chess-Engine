#pragma once

#include "../core/types.h"

namespace Aribella {
    enum class MoveFlag : u8 {
        Quiet = 0,
        Capture,
        DoublePawnPush,
        KingCastle,
        QueenCastle,
        EnPassant,
        Promotion,
        PromotionCapture
    };

    struct Move {
        Square from{Square::None};
        Square to{Square::None};
        MoveFlag flag{MoveFlag::Quiet};
        PieceType promotion{PieceType::None};
    };

    [[nodiscard]]
    constexpr bool is_captured(const Move &move) noexcept {
        return move.flag == MoveFlag::Capture
               || move.flag == MoveFlag::PromotionCapture
               || move.flag == MoveFlag::EnPassant;
    }

    [[nodiscard]]
    constexpr bool is_promotion(const Move &move) noexcept {
        return move.flag == MoveFlag::Promotion
               || move.flag == MoveFlag::PromotionCapture;
    }

}
