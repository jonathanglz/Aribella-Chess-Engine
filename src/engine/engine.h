#pragma once

#include <cstddef>
#include <memory>
#include <thread>
#include <functional>
#include <cstdint>
#include <atomic>

#include "game_state.h"
#include "../search/transposition_table.h"
#include "../search/search.h"

namespace Aribella {
    class Engine {
    public:
        using SearchFinishedCallback = std::function<void(const IterativeSearchResult&, bool)>;

        explicit Engine(std::size_t tt_size);

        [[nodiscard]]
        GameState &game() noexcept;

        [[nodiscard]]
        const GameState &game() const noexcept;

        [[nodiscard]]
        TranspositionTable &tt() noexcept;

        void new_game();
        void set_game(GameState game);

        [[nodiscard]]
        IterativeSearchResult search(int depth, bool use_move_ordering);

        void start_search(int depth, bool use_move_ordering, SearchFinishedCallback on_finished);

        void stop() noexcept;
        void wait_for_search();
        void cancel_search() noexcept;

        void set_search_deadline(SearchControl::Clock::time_point deadline) noexcept;

        void clear_search_deadline() noexcept;


    private:
        std::size_t tt_size_;

        GameState game_;

        std::unique_ptr<TranspositionTable> tt_;

        SearchControl search_control_;

        std::jthread search_thread_;

        std::atomic<std::uint64_t> search_id_{0};
    };
}
