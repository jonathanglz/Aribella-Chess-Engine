#include "uci.h"

#include "../engine/engine.h"
#include "../engine/game_state.h"
#include "../movegen/move_format.h"
#include "../movegen/movegen.h"
#include "../search/search.h"
#include "../search/transposition_table.h"

#include <algorithm>
#include <iostream>
#include <memory>
#include <sstream>

namespace Aribella {
    namespace {
        constexpr std::string_view StartFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

        constexpr std::size_t TTSize = 100'000;

        struct UciSession {
            Engine engine{TTSize};

            UciSession() {
                engine.game().set_from_fen(StartFEN);
            }

            void new_game() {
                engine.new_game();
                engine.game().set_from_fen(StartFEN);
            }
        };

        bool apply_uci_move(GameState &game, std::string_view move_text) {
            Position position = game.position();

            const MoveList moves = generate_legal_moves(position, position.side_to_move());

            for (std::size_t i = 0; i < moves.size(); ++i) {
                const Move move = moves[i];

                if (move_to_string(move) == move_text) {
                    game.make_move(move);
                    return true;
                }
            }
            return false;
        }

        bool set_uci_position(UciSession &session, std::istringstream &input) {
            std::string token;

            if (!(input >> token)) {
                return false;
            }

            GameState next_game;

            if (token == "startpos") {
                if (!next_game.set_from_fen(StartFEN)) {
                    return false;
                }
            } else if (token == "fen") {
                std::string fen;
                std::string field;

                for (int i = 0; i < 6; ++i) {
                    if (!(input >> field)) {
                        return false;
                    }

                    if (!fen.empty()) {
                        fen += ' ';
                    }

                    fen += field;
                }

                if (!next_game.set_from_fen(fen)) {
                    return false;
                }
            } else {
                return false;
            }

            if (input >> token) {
                if (token != "moves") {
                    return false;
                }

                while (input >> token) {
                    if (!apply_uci_move(next_game, token)) {
                        return false;
                    }
                }
            }
            session.engine.set_game(std::move(next_game));

            return true;
        }

        void print_uci_score(int score) {
            if (!is_mate_score(score)) {
                std::cout << "score cp " << score;
                return;
            }

            if (score > 0) {
                const int plies = MateScore - score;
                const int mate_moves = (plies + 1) / 2;

                std::cout << "score mate " << mate_moves;
            } else {
                const int plies = MateScore + score;
                const int mate_moves = -(plies / 2);

                std::cout << "score mate " << mate_moves;
            }
        }
    }

    void run_uci() {
        UciSession session;

        std::string line;

        while (std::getline(std::cin, line)) {
            std::istringstream input(line);

            std::string command;
            input >> command;

            if (command == "uci") {
                std::cout
                        << "id name Aribella\n"
                        << "id author Jonathan Gonzalez\n"
                        << "uciok\n"
                        << std::flush;
            } else if (command == "isready") {
                std::cout
                        << "readyok\n"
                        << std::flush;
            } else if (command == "ucinewgame") {
                session.engine.cancel_search();
                session.engine.wait_for_search();

                session.new_game();
            } else if (command == "position") {
                session.engine.cancel_search();
                session.engine.wait_for_search();

                if (!set_uci_position(session, input)) {
                    std::cout
                            << "info string Invalid position command\n"
                            << std::flush;
                }
            } else if (command == "go") {
                std::string token;

                int depth = 64;

                int wtime = -1;
                int btime = -1;
                int winc = 0;
                int binc = 0;
                int movetime = -1;

                bool depth_specified = false;

                while (input >> token) {
                    if (token == "depth") {
                        if (!(input >> depth) || depth < 1) {
                            std::cout << "info string Invalid search depth\n"
                                    << std::flush;
                            depth = 0;
                            break;
                        }

                        depth_specified = true;
                    } else if (token == "wtime") {
                        input >> wtime;
                    } else if (token == "btime") {
                        input >> btime;
                    } else if (token == "winc") {
                        input >> winc;
                    } else if (token == "binc") {
                        input >> binc;
                    } else if (token == "movetime") {
                        input >> movetime;
                    }
                }

                if (depth == 0) {
                    continue;
                }

                session.engine.clear_search_deadline();

                if (movetime >= 0) {
                    session.engine.set_search_deadline(
                        SearchControl::Clock::now() +
                        std::chrono::milliseconds(movetime)
                    );
                } else if (!depth_specified) {
                    const Color side = session.engine.game().position().side_to_move();

                    const int remaining_time =
                            side == Color::White
                                ? wtime
                                : btime;

                    const int increment =
                            side == Color::White
                                ? winc
                                : binc;

                    if (remaining_time >= 0) {
                        int allocated_time =
                                remaining_time / 30 +
                                increment / 2;

                        allocated_time = std::min(allocated_time, std::max(1, remaining_time - 50));

                        session.engine.set_search_deadline(
                            SearchControl::Clock::now() +
                            std::chrono::milliseconds(allocated_time)
                        );
                    }
                }

                Position root_position = session.engine.game().position();

                MoveList root_moves =
                        generate_legal_moves(root_position, root_position.side_to_move());

                Move fallback_move{};

                if (!root_moves.empty()) {
                    fallback_move = root_moves[0];
                }

                session.engine.start_search(
                    depth,
                    true,
                    [fallback_move](const IterativeSearchResult &result, bool is_current) {
                        if (!is_current) {
                            return;
                        }

                        std::uint64_t total_nodes = 0;

                        // Print information for every completed iteration.
                        for (const IterationInfo &iteration: result.iterations) {
                            total_nodes += iteration.stats.nodes;

                            std::cout
                                    << "info depth "
                                    << iteration.depth
                                    << ' ';

                            print_uci_score(iteration.score);

                            std::cout
                                    << " nodes "
                                    << total_nodes;

                            if (!iteration.pv.empty()) {
                                std::cout << " pv";

                                for (const Move &move: iteration.pv) {
                                    std::cout
                                            << ' '
                                            << move_to_string(move);
                                }
                            }

                            std::cout << '\n';
                        }

                        // Print exactly one final bestmove.
                        Move best_move = fallback_move;

                        if (result.completed_depth > 0) {
                            best_move = result.result.best_move;
                        }

                        std::cout
                                << "bestmove "
                                << move_to_string(best_move)
                                << '\n'
                                << std::flush;
                    }
                );
            } else if (command == "stop") {
                session.engine.stop();
                session.engine.wait_for_search();
            } else if (command == "quit") {
                session.engine.cancel_search();
                session.engine.wait_for_search();
                break;
            }
        }
    }
}
