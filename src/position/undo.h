#pragma once

#include "../core/types.h"

namespace Aribella {

    struct UndoState {
        Piece captured_piece{Piece::None};

        CastlingRights castling_rights{CastlingRights::None};

        Square en_passant_square{Square::None};
    };
}