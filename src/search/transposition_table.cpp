#include "transposition_table.h"

namespace Aribella {
    TranspositionTable::TranspositionTable(std::size_t size) : entries_(size) {
        assert(size > 0);
    }


    void TranspositionTable::clear() {
        std::fill(entries_.begin(), entries_.end(), TTEntry{});
    }


    void TranspositionTable::store(Zobrist::Key key, int depth, int score, TTFlag flag, const Move &best_move) {
        const auto index = static_cast<std::size_t>(key % entries_.size());

        entries_[index] = TTEntry{
            .key = key,
            .depth = depth,
            .score = score,
            .best_move = best_move,
            .flag = flag
        };
    }


    const TTEntry *TranspositionTable::probe(Zobrist::Key key) const noexcept {
        const auto index = static_cast<std::size_t>(key % entries_.size());

        const TTEntry &entry = entries_[index];

        if (entry.key != key) {
            return nullptr;
        }

        return &entry;
    }
}
