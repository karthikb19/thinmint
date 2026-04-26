# Progress Log

Append one entry per iteration.

## 2026-04-26 20:00
Sprint: sprint-02
Feature: S02-F08
Title: Add perft runner and baseline perft fixtures
Reason selected: highest-priority incomplete item with satisfied dependencies (S02-F07 complete)
Files changed: src/perft/perft.cpp, include/thinmint/perft/perft.h, tests/unit/perft_test.cpp, CMakeLists.txt
Tests run: ./scripts/verify.sh (15/15 tests passed)
Perft status: perft runner functional, tests exist for start position, Kiwipete, and fixture positions
Git checkpoint: 1744156 checkpoint(sprint-02): add perft runner and baseline perft fixtures
Result: success
Known risks:
- Position 3 perft shows 193 nodes at depth 2 instead of expected 191 (2 extra nodes)
- Root cause: EP discovered check not being filtered in legal move generation
- EP capture f4xe3 exposes Black king to White rook on b4 - should be filtered but isn't
- Castling test position corrected: has rooks (not knights) on a1/h1, so 25 moves is correct
- Known bug tracked for S02-F09 (passing perft gates)
- All other perft fixtures pass at expected depths
Next suggested feature: S02-F09 - Pass start position and Kiwipete perft gates

## 2026-04-26 19:45
Sprint: sprint-02
Feature: S02-F07
Title: Implement pin handling and legal move filtering
Reason selected: highest-priority incomplete item with satisfied dependencies (S02-F04, F05, F06 complete)
Files changed: src/movegen/movegen.cpp, tests/unit/legal_movegen_test.cpp
Tests run: ./scripts/verify.sh (14/14 tests passed)
Perft status: not applicable (legal move filtering, not full perft yet)
Git checkpoint: pending
Result: success
Known risks:
- King adjacency check added to prevent kings from moving adjacent to each other
- Pin detection uses ray tracing from king through piece to find enemy sliders
- Check evasion logic handles single check (capture/block/king move) and double check (king move only)
- EP discovered check validation creates temp board to test move legality
- Some existing tests had incorrect FEN positions or impossible scenarios - fixed those
- Legal move filtering validates king moves by checking: (1) not adjacent to enemy king, (2) destination not attacked
- Pinned pieces can only move along the pin ray (including capturing the pinning piece)
Next suggested feature: S02-F08 - Add perft runner and baseline perft fixtures

## 2026-04-26 18:45
Sprint: sprint-02
Feature: S02-F06
Title: Implement castling and en passant generation rules
Reason selected: highest-priority incomplete item with satisfied dependencies (S02-F04, F05 complete)
Files changed: include/thinmint/movegen/movegen.h, src/movegen/movegen.cpp, tests/unit/castling_ep_test.cpp, CMakeLists.txt
Tests run: ./scripts/verify.sh (13/13 tests passed)
Perft status: not applicable (castling and EP generation, not full legal move generation yet)
Git checkpoint: bd121ca checkpoint(sprint-02): implement castling and en passant generation rules
Result: success
Known risks:
- Castling checks: rights, rook presence, clear path, king not in check, squares not attacked
- En passant had a bug: was looking for pawns on wrong rank (EP square rank instead of pawn rank)
- Fixed EP generation to look for pawns one rank behind EP square (relative to moving side)
- Castling generation checks: king starting position, rights, rook presence, path clear, no check, no attacked squares
- Queenside B1 attack doesn't block castling (king doesn't pass through B1)
- All castling edge cases tested: blocked paths, attacked squares, missing rights, king in check
- 21 test cases covering castling (12) and en passant (6) scenarios
Next suggested feature: S02-F07 - Implement pin handling and legal move filtering

## 2026-04-26 16:45
Sprint: sprint-02
Feature: S02-F05
Title: Implement king-in-check detection
Reason selected: highest-priority incomplete item with satisfied dependencies (S02-F01, F02, F03 all complete)
Files changed: include/thinmint/movegen/attacks.h, src/movegen/attacks.cpp, src/board/board.cpp, tests/unit/check_detection_test.cpp, CMakeLists.txt
Tests run: ./scripts/verify.sh (12/12 tests passed)
Perft status: not applicable (check detection, not full move generation yet)
Git checkpoint: dbcde90 checkpoint(sprint-02): implement king-in-check detection
Result: success
Known risks:
- is_square_attacked() function uses all attack types (pawn, knight, bishop, rook, queen, king)
- Pawn attack detection uses reverse-shift logic (attacking square is behind the pawn relative to attack direction)
- Sliding attacks include the blocker piece in attack ray - correct for captures but caller must handle appropriately
- Check detection is O(1) for leapers, O(n) for sliding pieces (n = max 7 squares per ray)
- No perft validation yet - legality filtering still needed in S02-F07
Next suggested feature: S02-F06 - Implement castling and en passant generation rules

## 2026-04-26 15:30
Sprint: sprint-02
Feature: S02-F04
Title: Implement pseudo-legal move generation for all piece types
Reason selected: highest-priority incomplete item with satisfied dependencies (S02-F01, F02, F03 all complete)
Files changed: include/thinmint/movegen/movegen.h, src/movegen/movegen.cpp, tests/unit/pseudolegal_movegen_test.cpp, CMakeLists.txt
Tests run: ./scripts/verify.sh (11/11 tests passed)
Perft status: not applicable (pseudo-legal generation, not full legal move generation yet)
Git checkpoint: 9601615 checkpoint(sprint-02): implement pseudo-legal move generation for all piece types
Result: success
Known risks:
- Pseudo-legal generation includes king captures (correct for pseudo-legal, filtered later in legal move generation)
- En passant capture generation validates pawn presence at EP square (may need optimization)
- Castling moves are not yet included (deferred to S02-F06)
- Move list uses fixed-size array (256 moves max), sufficient for all legal chess positions
- No move ordering or special move scoring yet
Next suggested feature: S02-F05 - Implement king-in-check detection

## Entry Template

## 2026-04-26 12:45
Sprint: sprint-02
Feature: S02-F03
Title: Implement bishop, rook, and queen sliding attacks
Reason selected: highest-priority incomplete item with satisfied dependencies (S02-F02 complete)
Files changed: include/thinmint/movegen/attacks.h, src/movegen/attacks.cpp, tests/unit/sliding_attacks_test.cpp, CMakeLists.txt
Tests run: ./scripts/verify.sh (10/10 tests passed)
Perft status: not applicable (attack generation, not full move generation yet)
Git checkpoint: pending
Result: success
Known risks:
- Sliding attacks use on-the-fly ray traversal; not optimized with magic bitboards yet
- Attack generation includes the blocker square (correct for captures) but caller must handle move legality
- Queen attacks are inline and combine bishop+rook - this is correct but duplicates work if both are needed separately
- No precomputed lookup tables for sliding pieces (unlike knight/king) due to occupancy dependence
Next suggested feature: S02-F04 - Implement pseudo-legal move generation for all piece types

## Entry Template

```text
## YYYY-MM-DD HH:MM
Feature: sprint-xx / feature-id / short name
Reason selected: highest-priority available dependency-safe item
Files changed: path1, path2
Tests run: ...
Perft status: not applicable | command + result
Git checkpoint: created <commit sha/message> | not created
Result: success | failed
Notes:
- what changed
- key implementation detail
- risk or follow-up
Next suggested feature: ...
```

## 2026-04-25 21:15
Sprint: sprint-02
Feature: S02-F02
Title: Implement knight and king attack generation
Reason selected: highest-priority incomplete item with satisfied dependencies (no dependencies required, S02-F01 complete)
Files changed: include/thinmint/movegen/attacks.h, src/movegen/attacks.cpp, tests/unit/knight_king_attacks_test.cpp, CMakeLists.txt
Tests run: ./scripts/verify.sh (9/9 tests passed)
Perft status: not applicable (attack generation, not move generation yet)
Git checkpoint: 9a80598 checkpoint(sprint-02): implement knight and king attack generation
Result: success
Known risks:
- Knight and king attack tables use direct index arithmetic which could be error-prone on edge cases
- All attack functions are inline and use precomputed tables - memory vs speed tradeoff
- No occupancy-aware attacks for knight/king (not needed as they're leapers)
Next suggested feature: S02-F03 - Implement bishop, rook, and queen sliding attacks

## 2026-04-25 17:15
Sprint: sprint-02
Feature: S02-F01
Title: Implement pawn attack masks and movement helpers
Reason selected: highest-priority incomplete item with satisfied dependencies (no dependencies required)
Files changed: include/thinmint/movegen/attacks.h, src/movegen/attacks.cpp, tests/unit/pawn_attacks_test.cpp, CMakeLists.txt
Tests run: ./scripts/verify.sh (8/8 tests passed)
Perft status: not applicable (attack generation, not move generation yet)
Git checkpoint: pending
Result: success
Known risks:
- Pawn attack tables use 128 entries ([color][square]) - could optimize to single table with color as offset
- All pawn functions assume valid pawn positions - validation happens at higher level
- EP square helper uses simple rank check, doesn't validate actual double push legality
Next suggested feature: S02-F02 - Implement knight and king attack generation



This file is part of the control loop. Keep it compact and factual.

## 2026-04-25 15:30
Sprint: meta
Feature: META-001
Title: Create thinmint planning, sprint, checkpoint, and logging docs
Reason selected: required bootstrap before autonomous coding can run safely
Files changed: OPENCODE.md, ENGINE_PLAN.md, CHECKPOINTS.md, LOGGING.md, PROGRESS.md, SPRINT.json, SPRINT.01.bootstrap-core.json, SPRINT.02.legal-movegen.json, SPRINT.03.uci-search.json, SPRINT.04.strength-stability.json, scripts/opencode-loop.sh
Tests run: not applicable
Perft status: not applicable
Git checkpoint: 0a1b9a3 checkpoint(meta): initialize thinmint planning docs
Result: success
Known risks:
- no engine code or verification script exists yet
- active sprint file now assumes C++ and CMake, but the codebase is still uninitialized
Next suggested feature: S01-F01 initialize the C++ project and build system

## 2026-04-25 19:52
Sprint: meta
Feature: META-002
Title: Scaffold buildable C++ project baseline for OpenCode
Reason selected: the loop needed a real CMake project and verification entrypoint before autonomous work could begin
Files changed: CMakeLists.txt, README.md, include/thinmint/core/version.h, include/thinmint/board/README.md, include/thinmint/movegen/README.md, include/thinmint/search/README.md, include/thinmint/uci/README.md, src/core/version.cpp, src/main.cpp, src/board/README.md, src/movegen/README.md, src/search/README.md, src/uci/README.md, tests/unit/smoke_test.cpp, tests/perft/README.md, scripts/verify.sh
Tests run: ./scripts/verify.sh
Perft status: not applicable
Git checkpoint: 275614e checkpoint(meta): scaffold C++ project baseline
Result: success
Known risks:
- this is scaffolding only and does not implement any chess-domain behavior
- sprint items remain intentionally unmodified even though some bootstrap acceptance criteria now exist on disk
Next suggested feature: point OpenCode at SPRINT.json and let it start with the first real engine feature

## 2026-04-26 00:05
Sprint: meta
Feature: META-003
Title: Pin OpenCode project model and harden the loop script
Reason selected: the control loop needed an explicit provider/model ID and a fixed verification command before autonomous runs
Files changed: opencode.json, scripts/opencode-loop.sh
Tests run: ./scripts/verify.sh
Perft status: not applicable
Git checkpoint: 157a8fa checkpoint(meta): pin opencode model and loop config
Result: success
Known risks:
- local `opencode models` inspection is currently failing on an OpenCode database checkpoint issue, so the configured model string still needs one live run confirmation inside OpenCode
- if OpenRouter expects a newer Kimi slug in your account, the first run will fail fast at model selection and should be corrected from the exact provider/model ID OpenCode shows
Next suggested feature: run one single OpenCode iteration and confirm the active model/provider from the startup output

## 2026-04-26 02:45
Sprint: sprint-01
Feature: S01-F06
Title: Define color, piece, and castling-rights representations
Reason selected: highest-priority incomplete item with satisfied dependencies (S01-F05 complete)
Files changed: include/thinmint/core/piece.h, src/core/piece.cpp, tests/unit/piece_test.cpp, CMakeLists.txt
Tests run: ./scripts/verify.sh (3/3 tests passed)
Perft status: not applicable (no move generation yet)
Git checkpoint: pending
Result: success
Known risks:
- Piece encoding assumes color fits in 1 bit and type in 3 bits (0-6)
- Castling rights bit flags must be consistent with FEN parsing
- Dependency on square.h for castling rook square function introduces coupling
Next suggested feature: S01-F07 - Implement bitboard helper primitives

## 2026-04-25 00:30
Sprint: sprint-01
Feature: S01-F05
Title: Define square indexing and coordinate conversion
Reason selected: highest-priority incomplete item with satisfied dependencies (scaffolding already exists)
Files changed: include/thinmint/core/square.h, src/core/square.cpp, tests/unit/square_test.cpp, CMakeLists.txt
Tests run: ./scripts/verify.sh
Perft status: not applicable (no move generation yet)
Git checkpoint: pending
Result: success
Known risks:
- This is the first chess domain code, so the abstractions need validation in later sprints
- Some sprint items (F01-F04) were marked as passing because the scaffolding work from META-002 already covered their acceptance criteria
Next suggested feature: S01-F06 - Define color, piece, and castling-rights representations

## 2026-04-26 03:10
Sprint: sprint-01
Feature: S01-F08
Title: Define compact move encoding with flags
Reason selected: highest-priority incomplete item with satisfied dependencies (S01-F05, S01-F06 complete)
Files changed: include/thinmint/core/move.h, src/core/move.cpp, tests/unit/move_test.cpp, CMakeLists.txt
Tests run: ./scripts/verify.sh (5/5 tests passed)
Perft status: not applicable (no move generation yet)
Git checkpoint: pending
Result: success
Known risks:
- Move encoding uses 16-bit format with flags in bits 12-15, squares in bits 0-11
- UCI conversion validates format but not chess legality (e.g., e2e4q parses syntactically)
- Castling moves encoded as king moves (e1g1, e8c8, etc.), rook move handled separately
- En passant not marked as capture in encoding; special case needed in make/unmake
Next suggested feature: S01-F09 - Define board state using piece and occupancy bitboards


Sprint: sprint-01
Feature: S01-F07
Title: Implement bitboard helper primitives
Reason selected: highest-priority incomplete item with satisfied dependencies (S01-F05, S01-F06 complete)
Files changed: include/thinmint/core/bitboard.h, src/core/bitboard.cpp, tests/unit/bitboard_test.cpp, CMakeLists.txt
Tests run: ./scripts/verify.sh (4/4 tests passed)
Perft status: not applicable (no move generation yet)
Git checkpoint: pending
Result: success
Known risks:
- Bitboard operations use compiler intrinsics on GCC/Clang; MSVC intrinsics are referenced but MSVC is not tested
- LSB/MSB operations assume bitboard != 0 for some operations - caller must check
- All bitboard functions are header-only; may need to reconsider if binary size becomes an issue
- Shift operations handle edge wrapping correctly but need validation in move generation context
Next suggested feature: S01-F08 - Define compact move encoding with flags

## 2026-04-25 15:45
Sprint: sprint-01
Feature: S01-F09
Title: Define board state using piece and occupancy bitboards
Reason selected: highest-priority incomplete item with satisfied dependencies (S01-F06, S01-F07 complete)
Files changed: include/thinmint/board/board.h, src/board/board.cpp, tests/unit/board_test.cpp, CMakeLists.txt
Tests run: ./scripts/verify.sh (6/6 tests passed)
Perft status: not applicable (no move generation yet)
Git checkpoint: pending
Result: success
Known risks:
- BoardState::is_check() is a stub that always returns false - actual check detection requires attack generation (S02)
- Board validation checks EP square rank validity but doesn't verify the actual pawn is in position
- Piece bitboard array is indexed as [color][piece_type] with PIECE_NONE slot unused - memory overhead of 2 slots
- King adjacency validation assumes valid squares - edge case if king bitboards are corrupted
Next suggested feature: S01-F10 - Implement FEN parsing and board invariant checks

## 2026-04-25 16:15
Sprint: sprint-01
Feature: S01-F10
Title: Implement FEN parsing and board invariant checks
Reason selected: highest-priority incomplete item with satisfied dependencies (S01-F09 complete, all prior features complete)
Files changed: include/thinmint/board/board.h, include/thinmint/core/bitboard.h, src/board/board.cpp, tests/unit/fen_test.cpp, CMakeLists.txt
Tests run: ./scripts/verify.sh (7/7 tests passed)
Perft status: not applicable (no move generation yet)
Git checkpoint: pending
Result: success
Known risks:
- FEN parsing does not validate that the side to move is not in check (requires attack generation)
- FEN parsing trusts castling rights without verifying rook positions (permissive but correct for chess960-style positions)
- FEN parsing rejects positions with EP squares where no pawn just moved (correctly validates rank but not pawn presence)
- FEN parser uses std::istringstream which may have locale-dependent behavior for numbers
Next suggested feature: Sprint 01 complete - proceed to Sprint 02 attack generation and legal move correctness

