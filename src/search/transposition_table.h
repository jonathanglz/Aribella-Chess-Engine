#pragma once

#include <cstddef>
#include <vector>

#include "../core/types.h"
#include "../movegen/move.h"
#include "../position/zobrist.h"

namespace Aribella {
    enum class TTFlag : u8 {
        Exact,
        LowerBound,
        UpperBound
    };

    struct TTEntry {
        Zobrist::Key key{};
        int depth{};
        int score{};
        Move best_move{};
        TTFlag flag{TTFlag::Exact};
        bool valid{false};
    };

    class TranspositionTable {
    public:
        explicit TranspositionTable(std::size_t size);

        void clear();

        void store(Zobrist::Key key, int depth, int score, TTFlag flag, const Move &best_move);

        [[nodiscard]]
        const TTEntry *probe(Zobrist::Key key) const noexcept;

    private:
        std::vector<TTEntry> entries_;
    };
}
