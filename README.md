# thinmint

`thinmint` is a bitboard chess engine project scaffolded for a Ralph-style
OpenCode loop.

## Current state

This repository currently contains bootstrap scaffolding only:

- C++20 project via CMake
- `thinmint` executable target
- `thinmint_core` library target
- a minimal smoke test
- `scripts/verify.sh` for the control loop

No chess logic has been implemented yet.

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

## OpenCode loop inputs

The autonomous loop should always read:

- `OPENCODE.md`
- `ENGINE_PLAN.md`
- `SPRINT.json`
- `LOGGING.md`
- `PROGRESS.md`
- `CHECKPOINTS.md`

These files define how work is selected, validated, logged, and committed.
