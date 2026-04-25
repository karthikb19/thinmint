# thinmint Loop Instructions

`thinmint` is a Ralph-style loop for building a bitboard chess engine with constrained models.

Recommended model for this experiment:

- `qwen3-coder` or the strongest Qwen coding model available in OpenCode
- fallback: `DeepSeek-Coder`
- avoid the weakest model on legality-heavy tasks unless the sprint is trivial

Your job is to complete exactly one feature per iteration.

## Primary objective

Build a correct, deterministic chess engine with:

- bitboards
- validated legal move generation
- make/unmake
- perft coverage
- UCI support
- a decent alpha-beta search

## Hard rules

- Work on only one incomplete item from `SPRINT.json`.
- Respect feature dependencies.
- Do not rewrite large parts of the engine unless the selected task explicitly requires it.
- Prefer small, reviewable diffs.
- Every implementation change must include or update tests.
- If you touch move generation, legality, make/unmake, castling, en passant, promotion, check detection, or FEN parsing, you must run perft tests.
- Do not mark a feature as complete unless all relevant tests pass.
- Do not skip failing tests by deleting, weakening, or commenting them out.
- Do not introduce external dependencies unless necessary and justified in `PROGRESS.md`.
- Keep the engine deterministic.
- If you are unsure about chess correctness, stop optimizing and add validation first.

## Chess engine correctness priorities

Correctness order:

1. Legal move generation
2. Make/unmake consistency
3. Perft correctness
4. UCI compatibility
5. Search correctness
6. Evaluation strength
7. Performance

A faster wrong engine is worthless.

## Required behavior per iteration

1. Read `ENGINE_PLAN.md`, `SPRINT.json`, `PROGRESS.md`, and `CHECKPOINTS.md`.
2. Select the highest-priority incomplete feature whose dependencies are complete.
3. Restate the chosen feature in one short sentence before coding.
4. Implement only that feature.
5. Add or update tests.
6. Run the project verification command.
7. If you touched chess rules or move generation, run perft validation.
8. If verification passes:
   - mark the feature complete in `SPRINT.json`
   - append a detailed entry to `PROGRESS.md`
   - create a git checkpoint commit
9. If verification fails:
   - do not mark the feature complete
   - append the failure and diagnosis to `PROGRESS.md`
   - do not create a success checkpoint commit
10. Output `<THINMINT_DONE>` only if every feature in `SPRINT.json` is complete.

## Logging requirements

Each progress entry must include:

- Feature worked on
- Why it was selected
- Files changed
- Tests run
- Perft status, if relevant
- Git checkpoint status
- Known risks
- Next suggested feature

## Git checkpoint format

Use commit messages in this format:

- `checkpoint(sprint-01): add board bitboards`
- `checkpoint(sprint-01): implement FEN parser`
- `checkpoint(sprint-02): legal move filter for check evasions`

Do not bundle unrelated work into a checkpoint.

## Prompt shape for OpenCode

Use this prompt body for each loop iteration:

```text
@OPENCODE.md @ENGINE_PLAN.md @SPRINT.json @PROGRESS.md @CHECKPOINTS.md @LOGGING.md

Work one feature at a time.

1. Read the project context.
2. Pick the highest-priority incomplete feature whose dependencies are done.
3. Implement only that feature.
4. Add or update tests.
5. Run verification.
6. If verification passes, update SPRINT.json and append to PROGRESS.md.
7. Create a git checkpoint commit with the required format.
8. If every feature is complete, output <THINMINT_DONE>.

Never silently skip tests.
Never mark incomplete or failing work as done.
Prefer correctness over speed.
```

## Expected repository shape

Target the following structure early:

```text
src/
  board/
  movegen/
  search/
  uci/
tests/
  perft/
  unit/
scripts/
  verify.sh
  checkpoint.sh
SPRINT.json
PROGRESS.md
CHECKPOINTS.md
LOGGING.md
ENGINE_PLAN.md
OPENCODE.md
```

## Model operating constraints

- Assume context windows are limited and imperfect.
- Re-read only the files needed for the chosen feature.
- Prefer extending small modules over growing a giant file.
- When a feature feels too large, narrow the implementation to the minimum acceptance criteria.
- If a feature depends on missing structure, create only enough structure to satisfy that feature.

## Done criteria for any feature

A feature is done only if all of the following are true:

- acceptance criteria for that feature are met
- relevant tests pass
- perft passes if chess rules were touched
- sprint file is updated
- progress log entry is appended
- git checkpoint commit is created

## Style

- Prefer simple, explicit code.
- Avoid cleverness unless it clearly improves performance.
- Keep APIs small.
- Add comments only for non-obvious chess logic.
- When in doubt, make the code easier to test.
