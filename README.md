# thinmint

`thinmint` is a bitboard chess engine project scaffolded for a Ralph-style
OpenCode loop.

## Current state

This repository currently contains a working bitboard engine baseline:

- C++20 project via CMake
- `thinmint` executable target
- `thinmint_core` library target
- legal move generation, make/unmake, perft, UCI parsing, evaluation, and search tests
- a repeatable search benchmark command
- `scripts/verify.sh` for the control loop

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Test

```bash
ctest --test-dir build --output-on-failure
```

## Verify

```bash
./scripts/verify.sh
```

This is the no-regression gate. It configures, builds, and runs the complete
CTest suite, including perft validation and tactical/search regression tests.

## Benchmark

Run a fixed search benchmark after building:

```bash
build/thinmint bench
```

The optional depth argument is clamped to `1..6`:

```bash
build/thinmint bench 4
```

The benchmark prints per-position `nodes` plus total nodes, elapsed milliseconds,
and nodes per second.

## OpenCode loop inputs

The autonomous loop should always read:

- `OPENCODE.md`
- `ENGINE_PLAN.md`
- `SPRINT.json`
- `LOGGING.md`
- `PROGRESS.md`
- `CHECKPOINTS.md`

These files define how work is selected, validated, logged, and committed.
