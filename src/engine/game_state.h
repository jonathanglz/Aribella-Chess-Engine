#pragma once
#include <vector>

#include "../position/zobrist.h"
#include "../position/position.h"

namespace Aribella {
    class GameState {
    public:
        [[nodiscard]]
        const Position &position() const noexcept {
            return position_;
        }

        [[nodiscard]]
        bool set_from_fen(std::string_view fen);

        [[nodiscard]]
        UndoState make_move(const Move &move);

        void unmake_move(const Move &move, const UndoState &undo);

        [[nodiscard]]
        bool is_threefold_repetition() const noexcept;

    private:
        Position position_{};

        std::vector<Zobrist::Key> position_history_{};
    };
}
