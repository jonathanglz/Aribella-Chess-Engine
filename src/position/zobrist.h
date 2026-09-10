#pragma once

#include <array>
#include <cstdint>

#include "../core/constants.h"
#include "../core/random.h"
#include "../core/types.h"

namespace Aribella::Zobrist {
    using Key = std::uint64_t;

    inline constexpr std::size_t PieceCount{12};
    inline constexpr std::size_t SquareCount{to_index(Square::Count)};

    struct Tables {
        std::array<std::array<Key, SquareCount>, PieceCount> piece_square{};

        Key side_to_move{};

        std::array<Key, 16> castling{};

        std::array<Key, 8> en_passant_file{};
    };

    [[nodiscard]]
    constexpr Tables make_tables() noexcept {
        Xorshift64Star RandomGenerator{ZobristSeed};

        Tables tables{};

        for (auto& piece_table : tables.piece_square) {

            for (Key& key : piece_table) {
                key = RandomGenerator.next();
            }
        }

        tables.side_to_move = RandomGenerator.next();

        for (Key& key : tables.castling) {
            key = RandomGenerator.next();
        }

        for (Key& key : tables.en_passant_file) {
            key = RandomGenerator.next();
        }

        return tables;
    }

    inline constexpr Tables ZobristTables = make_tables();
}