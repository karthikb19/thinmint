#!/usr/bin/env bash

set -euo pipefail

if [ $# -lt 2 ]; then
  echo "Usage: $0 <iterations> <sprint-file>"
  exit 1
fi

ITERATIONS="$1"
SPRINT_FILE="$2"
MODEL="${OPENCODE_MODEL:-openrouter/moonshotai/kimi-k2.6}"

if [ ! -f "$SPRINT_FILE" ]; then
  echo "Sprint file not found: $SPRINT_FILE"
  exit 1
fi

if [ ! -f "./scripts/verify.sh" ]; then
  echo "Missing verification script: ./scripts/verify.sh"
  exit 1
fi

if [ ! -x "./scripts/verify.sh" ]; then
  echo "Verification script is not executable: ./scripts/verify.sh"
  exit 1
fi

for ((i=1; i<=ITERATIONS; i++)); do
  echo "Iteration $i"
  echo "Using sprint file: $SPRINT_FILE"
  echo "Using model: $MODEL"

  result=$(opencode run \
    -m "$MODEL" \
    "@OPENCODE.md @ENGINE_PLAN.md @$SPRINT_FILE @PROGRESS.md @CHECKPOINTS.md @LOGGING.md

Work one feature at a time.

1. Read the files.
2. Select the highest-priority incomplete feature with completed dependencies.
3. Implement only that feature.
4. Add or update tests.
5. Run exactly this verification command: ./scripts/verify.sh
6. If tests pass, update the sprint file and append to PROGRESS.md.
7. Create a git checkpoint commit with message format checkpoint(<sprint>): <feature>.
8. If all features are complete, output <THINMINT_DONE>.

Never claim success without passing verification.
Never substitute a different verification command unless ./scripts/verify.sh is missing or broken, and if it is broken, log that fact in PROGRESS.md.
Prefer correctness over speed.")

  echo "$result"

  if [[ "$result" == *"<THINMINT_DONE>"* ]]; then
    echo "All sprint items complete."
    exit 0
  fi
done
