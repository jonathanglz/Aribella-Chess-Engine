#include "engine.h"

#include <utility>

namespace Aribella {
    Engine::Engine(std::size_t tt_size) : tt_size_(tt_size), game_{},
                                          tt_(std::make_unique<TranspositionTable>(tt_size_)) {
    }

    GameState &Engine::game() noexcept {
        return game_;
    }

    const GameState &Engine::game() const noexcept {
        return game_;
    }

    TranspositionTable &Engine::tt() noexcept {
        return *tt_;
    }

    void Engine::new_game() {
        game_ = GameState{};

        tt_ = std::make_unique<TranspositionTable>(tt_size_);
    }

    void Engine::set_game(GameState game) {
        game_ = std::move(game);
    }

    IterativeSearchResult Engine::search(int depth, bool use_move_ordering) {
        search_control_.stopped.store(false, std::memory_order_relaxed);

        return iterative_deepening(
            game_,
            *tt_,
            depth,
            use_move_ordering,
            search_control_
        );
    }

    void Engine::start_search(int depth, bool use_move_ordering, SearchFinishedCallback on_finished) {
        const std::uint64_t my_search_id = search_id_.fetch_add(1, std::memory_order_relaxed) + 1;

        stop();
        wait_for_search();

        search_thread_ = std::jthread([
                this,
                depth,
                use_move_ordering,
                my_search_id,
                on_finished = std::move(on_finished)
            ]() {
                const IterativeSearchResult result =
                        search(depth, use_move_ordering);

                const bool is_current = search_id_.load(std::memory_order_relaxed) == my_search_id;

                on_finished(result, is_current);
            }
        );
    }

    void Engine::stop() noexcept {
        search_control_.stopped.store(true, std::memory_order_relaxed);
    }

    void Engine::wait_for_search() {
        if (search_thread_.joinable()) {
            search_thread_.join();
        }
    }

    void Engine::cancel_search() noexcept {
        search_id_.fetch_add(1, std::memory_order_relaxed);

        stop();
    }

    void Engine::set_search_deadline(SearchControl::Clock::time_point deadline) noexcept {
        search_control_.deadline = deadline;
    }

    void Engine::clear_search_deadline() noexcept {
        search_control_.deadline = SearchControl::Clock::time_point::max();
    }
}
