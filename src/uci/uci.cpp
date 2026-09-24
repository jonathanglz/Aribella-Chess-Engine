#include "uci.h"

#include "../engine/game_state.h"
#include "../movegen/move_format.h"
#include "../movegen/movegen.h"
#include "../search/search.h"
#include "../search/transposition_table.h"

#include <algorithm>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>

namespace Aribella {
    namespace {
        constexpr std::string_view StartFen = "rnbqkbnr/pppppppp/8/8/8/8/"
                "PPPPPPPP/RNBQKBNR w KQkq - 0 1";

        constexpr std::size_t TTSize = 100'000;

        struct UciSession {
            GameState game{};

            std::unique_ptr<TranspositionTable> tt =
                    std::make_unique<TranspositionTable>(TTSize);

            UciSession() {
                game.set_from_fen(StartFen);
            }

            void new_game() {
                game.set_from_fen(StartFen);

                tt = std::make_unique<TranspositionTable>(TTSize);
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
                if (!next_game.set_from_fen(StartFen)) {
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
            session.game = std::move(next_game);
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
                        << "id author Aribella\n"
                        << "uciok\n"
                        << std::flush;
            } else if (command == "isready") {
                std::cout
                        << "readyok\n"
                        << std::flush;
            } else if (command == "ucinewgame") {
                session.new_game();
            } else if (command == "position") {
                if (!set_uci_position(session, input)) {
                    std::cout
                            << "info string Invalid position command\n"
                            << std::flush;
                }
            } else if (command == "go") {
                std::string token;

                // Temporary GUI support: use depth 4 when no depth is specified.
                int depth = 4;

                while (input >> token)
                {
                    if (token == "depth")
                    {
                        if (!(input >> depth) || depth < 1)
                        {
                            std::cout << "info string Invalid search depth\n"
                                      << std::flush;
                            depth = 0;
                            break;
                        }
                    }
                }

                if (depth == 0)
                {
                    continue;
                }

                const IterativeSearchResult result =
                        iterative_deepening(
                            session.game,
                            *session.tt,
                            depth,
                            true
                        );

                std::uint64_t total_nodes = 0;

                for (const IterationInfo &iteration:
                     result.iterations) {
                    total_nodes += iteration.stats.nodes;

                    std::cout
                            << "info depth " << iteration.depth
                            << ' ';

                    print_uci_score(iteration.score);

                    std::cout << " nodes " << total_nodes << '\n';
                }

                std::cout
                        << "bestmove "
                        << move_to_string(result.result.best_move)
                        << '\n'
                        << std::flush;
            } else if (command == "quit") {
                break;
            }
        }
    }
}
