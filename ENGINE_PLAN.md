# Chess Engine Plan

Build a bitboard-based chess engine with correct legal move generation, perft validation, UCI support, and a decent alpha-beta search.

## Language

Use C++ for `thinmint`.

Recommended baseline:

- C++20
- CMake
- `ctest` for test execution
- `clang++` or `g++`

Do not introduce heavy dependencies early. Keep Sprint 01 close to:

- standard library
- a small test framework only if needed, or plain executable tests

## Engine architecture

Core modules:

- Board representation
- FEN parser
- Move encoding
- Bitboard attack generation
- Pseudo-legal move generation
- Legal move filtering
- Make/unmake move
- Perft testing
- UCI protocol
- Evaluation
- Search

## Board representation

Use 64-bit bitboards.

Maintain:

- one bitboard per piece type and color
- occupancy by color
- total occupancy
- side to move
- castling rights
- en passant square
- halfmove clock
- fullmove number
- zobrist hash eventually, but not in the first sprint unless needed

## Correctness gates

Move generation must be validated with known perft positions before search work begins.

Minimum perft positions:

- starting position
- Kiwipete
- positions covering castling
- positions covering en passant
- positions covering promotions
- positions where king is in check
- positions with pins and discovered checks

## Search target

Initial search:

- negamax
- alpha-beta pruning
- iterative deepening
- simple material + piece-square evaluation
- legal move generation only
- UCI `go depth N`

Later search improvements:

- quiescence
- transposition table
- move ordering
- killer moves
- history heuristic

## Sprint breakdown

The project should run in 4 sprints. Each sprint should be small enough for weak models to finish incrementally, but detailed enough that feature selection is obvious.

### Sprint 01: Bootstrap and Core Position Model

Goal: make `thinmint` buildable, testable, and capable of loading a position safely.

Scope:

- initialize C++20 and CMake toolchain
- create source, test, and scripts layout
- create build command
- create test command
- create verification script
- add minimal CLI entrypoint
- define square indexing
- define color, piece, and castling-rights enums
- add bitboard helper primitives
- define move encoding
- define board state
- parse FEN into board state
- validate board state invariants

Required tests:

- smoke test for buildable binary
- configure/build test via CMake
- unit tests for bitboard primitives
- unit tests for square mapping
- unit tests for move encoding flags
- FEN parsing tests for start position
- FEN parsing tests for castling rights
- FEN parsing tests for en passant fields
- malformed FEN rejection tests

Exit criteria:

- one-command verification works
- `cmake -S . -B build && cmake --build build` succeeds
- start position parses correctly
- board state is represented fully enough for move generation work
- the repo has its first stable checkpoint chain

Implementation guidance:

- prefer a small static library target for engine code plus one executable target
- keep headers and source files split by module
- avoid template-heavy abstractions in the early sprints
- prefer explicit integer types like `uint64_t`, `uint32_t`, and `int32_t`

### Sprint 02: Attack Generation and Legal Move Correctness

Goal: produce correct legal moves and pass foundational perft gates.

Scope:

- pawn attack generation
- knight attack generation
- king attack generation
- bishop attacks
- rook attacks
- queen attacks
- pseudo-legal move generation for all pieces
- capture generation
- quiet move generation
- promotion generation
- castling generation
- en passant generation
- king-in-check detection
- pin handling
- legal move filtering
- perft runner
- perft fixture support

Required tests:

- attack map unit tests for every piece type
- check detection tests
- pinned piece movement restriction tests
- castling legality tests
- en passant legality tests
- promotion move generation tests
- perft start position tests
- Kiwipete perft tests
- targeted perft positions for checks, pins, castling, and promotions

Exit criteria:

- all generated moves are legal
- start position perft passes to chosen depth
- Kiwipete passes
- targeted rule positions pass
- move legality is trusted enough to begin make/unmake-heavy search work

### Sprint 03: Make/Unmake, UCI, and Search Baseline

Goal: mutate state safely, expose the engine to GUIs, and make legal decisions.

Scope:

- make move for all move classes
- unmake move with undo stack
- state restoration for captures, promotions, castling, ep, and clocks
- board hash placeholder or initial zobrist if convenient
- move list container
- UCI handshake
- `isready`
- `ucinewgame`
- `position startpos`
- `position fen`
- `go depth N`
- legal move application from UCI moves
- negamax
- alpha-beta pruning
- iterative deepening
- basic material evaluation
- simple piece-square evaluation
- checkmate/stalemate terminal scoring

Required tests:

- make/unmake round-trip tests
- make/unmake tests for special moves
- repeated random legal move sequence round-trip tests
- UCI parser tests
- legal bestmove output tests for simple positions
- mate-in-one and obvious tactical search tests

Exit criteria:

- make/unmake is stable
- engine communicates with a UCI GUI
- engine returns legal moves for depth-limited search
- full games can be played without crashing

### Sprint 04: Search Quality, Speed, and Reliability

Goal: make `thinmint` respectable rather than merely functional.

Scope:

- quiescence search
- move ordering using captures and simple heuristics
- killer move heuristic
- history heuristic
- transposition table
- iterative deepening improvements
- aspiration windows if still stable
- repetition and fifty-move handling if not already complete
- benchmarking harness
- node counter reporting
- optional basic time management
- regression suite hardening

Required tests:

- search regression tests on fixed tactical positions
- no-regression perft suite reruns
- repetition handling tests
- draw detection tests
- transposition table consistency tests where practical

Exit criteria:

- engine is stronger at equal depth than the sprint-03 baseline
- correctness gates still pass
- engine has benchmarkable progress and a stable verification loop

## Sprint philosophy

- Sprint 01 creates stable structure.
- Sprint 02 proves the engine understands chess rules.
- Sprint 03 turns legality into a playable engine.
- Sprint 04 improves strength without losing trustworthiness.

## Non-negotiable guardrails

- Perft gates search work.
- Make/unmake must be validated before aggressive pruning work.
- UCI should come after legality is trustworthy.
- Performance tuning comes after correctness and test coverage.
