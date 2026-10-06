# Aribella

Aribella is a chess engine written from scratch in modern C++20.

The project focuses on chess-engine architecture, efficient move generation, position representation, search algorithms, evaluation, automated testing, performance benchmarking, and communication through the Universal Chess Interface (UCI).

Aribella was developed as a systems programming project to explore performance-oriented C++ and the algorithms behind computer chess.

---

## Features

### Board Representation

- 64-bit bitboards
- Piece-centric board representation
- LERF square mapping
- Precomputed king and knight attacks
- Sliding-piece attack generation
- Zobrist hashing

### Move Generation

- Pseudo-legal move generation
- Legal move generation
- Castling
- En passant
- Pawn promotion
- Capture promotion
- Check and pin handling
- Reversible make/unmake move system

### Search

- Alpha-beta search
- Iterative deepening
- Quiescence search
- Transposition table
- Principal variation tracking
- Move ordering
- Mate-score handling
- Search cancellation
- Search deadlines
- Asynchronous search

### Evaluation

- Material evaluation
- Piece-square tables
- Game-phase-aware evaluation

### Engine

- Universal Chess Interface (UCI)
- Asynchronous search using C++ threading
- Search deadlines and time management
- Game-state tracking
- Threefold repetition detection
- Fifty-move rule handling
- Checkmate and stalemate detection

---

## Project Structure

```text
Aribella/
├── benchmarks/
│   └── perft_benchmark.cpp
│
├── src/
│   ├── core/
│   ├── engine/
│   ├── evaluation/
│   ├── movegen/
│   ├── position/
│   ├── search/
│   ├── uci/
│   └── main.cpp
│
├── tests/
│   ├── core/
│   ├── engine/
│   ├── evaluation/
│   ├── movegen/
│   ├── position/
│   ├── search/
│   └── uci/
│
├── CMakeLists.txt
├── LICENSE
└── README.md
```

---

## Requirements

Building Aribella requires:

- A C++20-compatible compiler
- CMake
- Git

The project has been developed and tested using GCC/MinGW-w64 on Windows.

---

## Building

Clone the repository:

```bash
git clone <repository-url>
cd Aribella
```

### Release Build

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

### Debug Build

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug
```

---

## Universal Chess Interface

Aribella supports the Universal Chess Interface (UCI) and can be used with compatible chess GUIs and tournament managers.

Start the engine and send:

```text
uci
```

Aribella responds with:

```text
id name Aribella
id author Jonathan Gonzalez
uciok
```

Check whether the engine is ready:

```text
isready
```

Response:

```text
readyok
```

Set the standard starting position and begin a depth-limited search:

```text
position startpos
go depth 5
```

During iterative deepening, Aribella reports search information:

```text
info depth 1 score cp ... nodes ... pv ...
info depth 2 score cp ... nodes ... pv ...
info depth 3 score cp ... nodes ... pv ...
info depth 4 score cp ... nodes ... pv ...
info depth 5 score cp ... nodes ... pv ...
bestmove e2e4
```

The exact evaluation, principal variation, and best move depend on the position.

Aribella also supports UCI commands for:

- `uci`
- `isready`
- `ucinewgame`
- `position startpos`
- `position fen`
- Move sequences
- `go depth`
- Time controls
- `movetime`
- `stop`
- `quit`

---

## Testing

Aribella uses GoogleTest for automated testing.

The test suite covers:

Bitboard operations
- Directional bitboard shifts
- Attack generation
- Move-list behavior
- Pseudo-legal move generation
- Legal move generation
- Check and pin handling
- Castling
- En passant
- Promotions
- Capture promotions
- Position representation
- FEN parsing
- Make/unmake correctness
- Zobrist hashing
- Perft reference positions
- Transposition-table behavior
- Search behavior
- Search cancellation
- Game-state detection
- Move formatting
- UCI communication
- Engine lifecycle
- Asynchronous search

After building the project, run the test executable directly.

### Windows

```powershell
.\build\aribella_tests.exe
```

### Linux / macOS

```bash
./build/aribella_tests
```

---

## Perft Validation

Aribella's move generator is validated against standard perft reference positions and special-move scenarios.

Testing includes:

- Starting position
- Kiwipete
- Standard perft test positions
- Castling
- En passant
- Illegal en passant exposing the king
- Promotions
- Capture promotions
- Position restoration after make/unmake

Starting-position reference values:

| Depth | Nodes |
|------:|------:|
| 1 | 20 |
| 2 | 400 |
| 3 | 8,902 |
| 4 | 197,281 |
| 5 | 4,865,609 |

---

## Performance Benchmark

Aribella includes a Release-mode perft benchmark.

The benchmark measures move-generation and make/unmake performance from the standard starting position at depth 5.

### Results

| Run | Nodes | Time | Nodes/Second |
|---:|---:|---:|---:|
| 1 | 4,865,609 | 0.177 s | 27,488,043 |
| 2 | 4,865,609 | 0.161 s | 30,298,558 |
| 3 | 4,865,609 | 0.200 s | 24,341,640 |
| **Average** | **4,865,609** | **0.179 s** | **27,376,080** |

Average performance:

**~27.4 million nodes per second**

All three benchmark runs produced the correct reference node count of **4,865,609**.

Performance results are hardware-dependent and are provided as a development benchmark rather than a cross-system comparison.

---

## UCI Stability Validation

Aribella was tested through Cute Chess in a 10-game UCI match against Stockfish.

The purpose of the match was to validate protocol integration and engine stability rather than compare playing strength.

### Match Configuration

```text
Games:      10
Colors:     Alternating
Aribella:   5 White / 5 Black
Time:       1 minute per 40 moves
Increment:  1 second per move
```

### Results

```text
Games completed: 10 / 10

Aribella as White:
0 wins
5 losses
0 draws

Aribella as Black:
0 wins
5 losses
0 draws
```

All 10 games ended normally by checkmate.

```text
Illegal moves:  0
Crashes:        0
Disconnects:    0
Stalls:         0
```

Stockfish won all 10 games, which is expected for a developing engine tested against a state-of-the-art chess engine.

The important result of this test was that Aribella successfully completed all 10 games through UCI while playing both colors without protocol or engine failures.

---

## Technologies and Concepts

Aribella uses or demonstrates:

- C++20
- CMake
- GoogleTest
- CTest
- Git
- UCI
- Bitboards
- Bit manipulation
- Zobrist hashing
- Alpha-beta search
- Iterative deepening
- Quiescence search
- Transposition tables
- Piece-square tables
- Multithreading
- Atomic synchronization
- Asynchronous search
- Recursive algorithms
- Automated testing
- Performance benchmarking

---

## Development Goals

Aribella was built to develop practical experience with:

- Modern C++ software design
- Performance-oriented programming
- Data structures
- Bit manipulation
- Recursive search algorithms
- State management
- Reversible operations
- Hash tables and hashing
- Multithreaded application design
- Protocol-based software integration
- Automated testing
- Debugging
- Benchmarking
- Software architecture

---

## Future Work

Potential future improvements include:

- Killer-move heuristic
- History heuristic
- Improved move ordering
- Null-move pruning
- Late move reductions
- Aspiration windows
- More advanced evaluation
- Pawn-structure evaluation
- King-safety evaluation
- Mobility evaluation
- Opening-book support
- Endgame improvements
- Search-performance optimization
- Additional UCI options
- Playing-strength benchmarking

---

## Author

**Jonathan Gonzalez**

Aribella was designed and implemented as a from-scratch C++ chess-engine project.

---

## License

Aribella is released under the MIT License.

Copyright © 2026 Jonathan Gonzalez.

See the [`LICENSE`](LICENSE) file for the full license text.

## SOURCES:

- chessprogramming.org
- https://analog-hors.github.io/site/magic-bitboards/
- Chess Programming by Code Monkey
- https://en.wikipedia.org/wiki/Xorshift
- Stockfish https://github.com/official-stockfish/stockfish
- DogeyStamp https://www.dogeystamp.com/chess0/
