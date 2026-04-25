# thinmint Logging Spec

The logging exists to make weak-model autonomous development auditable.

## Files

- `PROGRESS.md`: human-readable append-only iteration log
- `SPRINT.json`: machine-readable task state
- git history: authoritative code checkpoints

## Required per-iteration log flow

1. Select one feature.
2. State which feature is being attempted.
3. Implement code and tests.
4. Run verification.
5. Record outcome in `PROGRESS.md`.
6. If successful, commit with checkpoint format.
7. Update the sprint item to `passes: true` only after step 4 succeeds.

## Required progress entry fields

- timestamp
- sprint
- feature id
- feature title
- reason selected
- files changed
- tests run
- perft command and result when relevant
- commit hash and message when successful
- result status
- known risks
- next suggested feature

## Failure logging rules

- A failed attempt must still be logged.
- Failures should include the command that failed and the suspected cause.
- Do not erase failed attempts from `PROGRESS.md`.
- Do not mark the feature complete after partial progress.

## Compact example

```text
## 2026-04-25 15:10
Sprint: sprint-01
Feature: S01-F04
Title: Implement FEN parsing into board state
Reason selected: highest-priority incomplete item with satisfied dependencies
Files changed: src/board/fen.rs, tests/fen_tests.rs
Tests run: ./scripts/verify.sh
Perft status: not applicable
Git checkpoint: 1234abcd checkpoint(sprint-01): implement FEN parsing into board
Result: success
Known risks:
- no FEN serialization yet
- malformed halfmove parsing still lightly tested
Next suggested feature: S02-F01 pawn and knight attack generation
```

## Why this matters

- `SPRINT.json` tells the model what remains.
- `PROGRESS.md` tells you what happened.
- git tells you what actually changed.

If those three disagree, trust git first, then fix the other two.
