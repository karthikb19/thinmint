#!/usr/bin/env bash

set -euo pipefail

if [ $# -lt 2 ]; then
  echo "Usage: $0 <iterations> <sprint-file>"
  exit 1
fi

ITERATIONS="$1"
SPRINT_FILE="$2"

for ((i=1; i<=ITERATIONS; i++)); do
  echo "Iteration $i"
  echo "Using sprint file: $SPRINT_FILE"

  result=$(opencode run \
    -m qwen \
    "@OPENCODE.md @ENGINE_PLAN.md @$SPRINT_FILE @PROGRESS.md @CHECKPOINTS.md @LOGGING.md

Work one feature at a time.

1. Read the files.
2. Select the highest-priority incomplete feature with completed dependencies.
3. Implement only that feature.
4. Add or update tests.
5. Run the project verification command.
6. If tests pass, update the sprint file and append to PROGRESS.md.
7. Create a git checkpoint commit with message format checkpoint(<sprint>): <feature>.
8. If all features are complete, output <THINMINT_DONE>.

Never claim success without passing verification.
Prefer correctness over speed.")

  echo "$result"

  if [[ "$result" == *"<THINMINT_DONE>"* ]]; then
    echo "All sprint items complete."
    exit 0
  fi
done
