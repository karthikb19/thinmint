# Git Checkpoints

Use git aggressively. Weak models drift. Commits are the rollback system.

## Rules

- Commit after every successful feature.
- Commit the documentation scaffolding before the first autonomous coding run.
- Do not commit failed verification states as if they are complete.
- Keep one feature per checkpoint.
- If a feature required manual cleanup, still commit once the tree is correct.
- Tag especially stable milestones if the sprint boundary is meaningful.

## Commit format

- `checkpoint(sprint-00): bootstrap project skeleton`
- `checkpoint(sprint-01): add bitboard board state`
- `checkpoint(sprint-01): parse FEN into board`
- `checkpoint(sprint-02): implement legal move generation`
- `checkpoint(sprint-03): add iterative deepening search`
- `checkpoint(meta): initialize thinmint planning docs`

## Branching

- Keep work on `main` if this is a solo experiment and you want the simplest loop.
- If you want safer inspection, create one branch per sprint and fast-forward merge only when the sprint exit criteria are met.
- Do not let the model create long-lived topic branches per feature unless you are explicitly supervising merges.

## Suggested milestone tags

- `v0-perft-startpos`
- `v0-perft-suite`
- `v0-uci-basic`
- `v0-search-playable`

## Suggested cadence

- Feature checkpoint: every passing feature
- Sprint checkpoint: after all items in a sprint are complete
- Milestone tag: after a major behavior gate passes
- Manual rescue checkpoint: before any risky refactor or search optimization

## Suggested helper commands

- `git add -A`
- `git commit -m "checkpoint(sprint-01): parse FEN into board"`
- `git tag v0-perft-startpos`

## Anti-patterns

- giant multi-feature commits
- commits before tests run
- rewriting the sprint file without changing code
- marking work complete based on intention instead of behavior
- optimizing search before move legality is trusted

## Human review points

Pause and inspect the repo manually after:

- first working FEN parser
- first correct start position perft
- first Kiwipete pass
- first legal make/unmake implementation
- first UCI `go depth` result
- first search win against a trivial baseline
