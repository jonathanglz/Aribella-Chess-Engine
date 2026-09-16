#include "game_state.h"

namespace Aribella {
    bool GameState::set_from_fen(std::string_view fen) {
        if (!position_.set_from_fen(fen)) {
            return false;
        }

        position_history_.clear();

        position_history_.push_back(position_.key());

        return true;
    }

    UndoState GameState::make_move(const Move &move) {
        const UndoState undo =
                position_.make_move(move);

        position_history_.push_back(position_.key());

        return undo;
    }

    void GameState::unmake_move(const Move &move, const UndoState &undo) {
        assert(position_history_.size() > 1);

        position_history_.pop_back();

        position_.unmake_move(move, undo);

        assert(position_history_.back() == position_.key());
    }

    bool GameState::is_threefold_repetition() const noexcept {
        // repetition is impossible if empty
        if (position_history_.empty()) {
            return false;
        }

        // key to compare older history against
        const Zobrist::Key current_key =
                position_.key();

        // last entry as current index
        const std::size_t current_index =
                position_history_.size() - 1;

        // get relevant plies for repetition searching
        const std::size_t reversible_plies =
                position_.halfmove_clock();

        // determines how far back we need to search
        const std::size_t earliest_index =
                reversible_plies > current_index
                    ? 0
                    : current_index - reversible_plies;


        // count current position as occurrence
        int occurrences{1};

        // not enough history for repeated position
        if (current_index < 2) {
            return false;
        }

        // Skip every other position because only one side's move must match
        for (std::size_t index = current_index - 2;;) {
            if (index < earliest_index) {
                break;
            }

            // older zobrist key matches current key
            if (position_history_[index] == current_key) {
                ++occurrences;

                if (occurrences >= 3) {
                    return true;
                }
            }

            // prevent unsigned integer underflow
            if (index < earliest_index + 2) {
                break;
            }

            // move back to previous position with the same side
            index -= 2;
        }

        return false;
    }
}
