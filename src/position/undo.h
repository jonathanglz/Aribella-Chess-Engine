#pragma once

#include "../core/types.h"

namespace Aribella {

    struct UndoState {
        Piece captured_piece{Piece::None};

        CastlingRights castling_rights{CastlingRights::None};

        Square en_passant_square{Square::None};

        std::uint16_t halfmove_clock{};

        std::uint16_t fullmove_number{};

        Color side_to_move{Color::White};
    };
}