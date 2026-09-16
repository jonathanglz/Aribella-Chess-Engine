#include "transposition_table.h"

namespace Aribella {
    TranspositionTable::TranspositionTable(std::size_t size) : entries_(size) {
        assert(size > 0);
    }


    void TranspositionTable::clear() {
        std::fill(entries_.begin(),
                  entries_.end(),
                  TTEntry{});
    }


    void TranspositionTable::store(Zobrist::Key key,
                                   int depth,
                                   int score,
                                   TTFlag flag,
                                   const Move &best_move) {
        const auto index =
                static_cast<std::size_t>(key % entries_.size());

        TTEntry &entry =
                entries_[index];

        // Empty slot: store
        if (!entry.valid) {
            entry = TTEntry{
                .key = key,
                .depth = depth,
                .score = score,
                .best_move = best_move,
                .flag = flag,
                .valid = true
            };

            return;
        }

        // Same Zobrist key: replace if new depth >= old depth
        if (entry.key == key) {
            if (depth >= entry.depth) {
                entry = TTEntry{
                    .key = key,
                    .depth = depth,
                    .score = score,
                    .best_move = best_move,
                    .flag = flag,
                    .valid = true
                };
            }
            return;
        }

        // Different key collision: replace only if new depth > old depth
        if (depth > entry.depth) {
            entry = TTEntry{
                .key = key,
                .depth = depth,
                .score = score,
                .best_move = best_move,
                .flag = flag,
                .valid = true
            };
        }
    }


    const TTEntry *TranspositionTable::probe(Zobrist::Key key) const noexcept {
        const auto index =
                static_cast<std::size_t>(key % entries_.size());

        const TTEntry &entry =
                entries_[index];

        if (!entry.valid) {
            return nullptr;
        }

        if (entry.key != key) {
            return nullptr;
        }

        return &entry;
    }
}
