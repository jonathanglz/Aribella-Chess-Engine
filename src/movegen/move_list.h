#pragma once

#include <array>
#include <cassert>
#include <cstddef>
#include <ranges>

#include "move.h"

namespace Aribella {

    inline constexpr std::size_t MaxMoves{256};

    class MoveList {
    public:
        [[nodiscard]]
        constexpr std::size_t size() const noexcept {
            return size_;
        }

        [[nodiscard]]
        constexpr bool empty() const noexcept {
            return size_ == 0;
        }

        constexpr void push_back(const Move& move) noexcept {
            assert(size_ < moves_.size());

            moves_[size_] = move;
            ++size_;
        }

        [[nodiscard]]
        constexpr const Move& operator[](const std::size_t index) const noexcept {
            assert(index < size_);

            return moves_[index];
        }

        constexpr void clear() noexcept {
            size_ = 0;
        }

    private:
        std::array<Move, MaxMoves> moves_{};
        std::size_t size_{0};
    };
}