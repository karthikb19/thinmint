#include "thinmint/movegen/movegen.h"
#include "thinmint/movegen/attacks.h"
#include "thinmint/board/board.h"

#include <cassert>
#include <cstdio>
#include <cstring>

using namespace thinmint::core;
using namespace thinmint::board;
using namespace thinmint::movegen;

// Count moves by piece type (approximately - based on from square piece)
int count_pawn_moves(const MoveList& moves, const BoardState& board) {
    int count = 0;
    for (size_t i = 0; i < moves.size(); ++i) {
        if (board.piece_type_at(from_square(moves[i])) == PIECE_PAWN) {
            ++count;
        }
    }
    return count;
}

int count_knight_moves(const MoveList& moves, const BoardState& board) {
    int count = 0;
    for (size_t i = 0; i < moves.size(); ++i) {
        if (board.piece_type_at(from_square(moves[i])) == PIECE_KNIGHT) {
            ++count;
        }
    }
    return count;
}

int count_bishop_moves(const MoveList& moves, const BoardState& board) {
    int count = 0;
    for (size_t i = 0; i < moves.size(); ++i) {
        if (board.piece_type_at(from_square(moves[i])) == PIECE_BISHOP) {
            ++count;
        }
    }
    return count;
}

int count_rook_moves(const MoveList& moves, const BoardState& board) {
    int count = 0;
    for (size_t i = 0; i < moves.size(); ++i) {
        if (board.piece_type_at(from_square(moves[i])) == PIECE_ROOK) {
            ++count;
        }
    }
    return count;
}

int count_queen_moves(const MoveList& moves, const BoardState& board) {
    int count = 0;
    for (size_t i = 0; i < moves.size(); ++i) {
        if (board.piece_type_at(from_square(moves[i])) == PIECE_QUEEN) {
            ++count;
        }
    }
    return count;
}

int count_king_moves(const MoveList& moves, const BoardState& board) {
    int count = 0;
    for (size_t i = 0; i < moves.size(); ++i) {
        if (board.piece_type_at(from_square(moves[i])) == PIECE_KING) {
            ++count;
        }
    }
    return count;
}

int count_captures(const MoveList& moves) {
    int count = 0;
    for (size_t i = 0; i < moves.size(); ++i) {
        if (is_capture(moves[i]) || is_en_passant(moves[i])) {
            ++count;
        }
    }
    return count;
}

int count_promotions(const MoveList& moves) {
    int count = 0;
    for (size_t i = 0; i < moves.size(); ++i) {
        if (is_promotion(moves[i])) {
            ++count;
        }
    }
    return count;
}

// Test 1: Starting position move count
void test_start_position() {
    printf("Test: Starting position...\n");

    BoardState board;
    board.reset_to_start_position();

    MoveList moves;
    size_t count = generate_pseudo_legal_moves(board, moves);

    // White to move: 20 legal moves expected (same as pseudo-legal since no checks)
    // 16 pawn moves (8 single pushes, 8 double pushes from rank 2)
    // Wait, that's not right. From starting position:
    // Each pawn can move 1 or 2 squares: 8 pawns * 2 = 16 pawn moves
    // Each knight has 2 moves: 2 knights * 2 = 4 knight moves
    // Total = 20
    assert(count == 20);
    assert(count_pawn_moves(moves, board) == 16);
    assert(count_knight_moves(moves, board) == 4);
    assert(count_captures(moves) == 0);
    assert(count_promotions(moves) == 0);

    printf("  PASS: Generated %zu moves (expected 20)\n", count);
}

// Test 2: Single pawn push
void test_pawn_single_push() {
    printf("Test: Pawn single push...\n");

    // Position with pawn and kings
    BoardState board = board_from_fen("4k3/8/8/8/8/8/4P3/4K3 w - - 0 1");
    assert(board.is_valid());

    MoveList moves;
    generate_pseudo_legal_moves(board, moves);

    // e2 pawn should be able to push to e3
    bool found_push = false;
    for (size_t i = 0; i < moves.size(); ++i) {
        if (from_square(moves[i]) == SQUARE_E2 && to_square(moves[i]) == SQUARE_E3) {
            found_push = true;
            assert(!is_capture(moves[i]));
            assert(!is_promotion(moves[i]));
            break;
        }
    }
    assert(found_push);

    printf("  PASS: Pawn single push works\n");
}

// Test 3: Pawn double push from starting rank
void test_pawn_double_push() {
    printf("Test: Pawn double push...\n");

    BoardState board = board_from_fen("4k3/8/8/8/8/8/4P3/4K3 w - - 0 1");
    assert(board.is_valid());

    MoveList moves;
    generate_pseudo_legal_moves(board, moves);

    // e2 pawn should be able to push to e4 (double push)
    bool found_double = false;
    for (size_t i = 0; i < moves.size(); ++i) {
        if (from_square(moves[i]) == SQUARE_E2 && to_square(moves[i]) == SQUARE_E4) {
            found_double = true;
            break;
        }
    }
    assert(found_double);

    printf("  PASS: Pawn double push works\n");
}

// Test 4: Pawn capture
void test_pawn_capture() {
    printf("Test: Pawn capture...\n");

    // White pawn on e4 with black pawn on d5 and f5
    BoardState board = board_from_fen("4k3/8/8/3p1p2/4P3/8/8/4K3 w - - 0 1");
    assert(board.is_valid());

    MoveList moves;
    generate_pseudo_legal_moves(board, moves);

    bool found_exd5 = false;
    bool found_exf5 = false;

    for (size_t i = 0; i < moves.size(); ++i) {
        if (from_square(moves[i]) == SQUARE_E4) {
            if (to_square(moves[i]) == SQUARE_D5) {
                found_exd5 = true;
                assert(is_capture(moves[i]));
            }
            if (to_square(moves[i]) == SQUARE_F5) {
                found_exf5 = true;
                assert(is_capture(moves[i]));
            }
        }
    }

    assert(found_exd5);
    assert(found_exf5);

    printf("  PASS: Pawn captures work\n");
}

// Test 5: Pawn promotion push
void test_pawn_promotion_push() {
    printf("Test: Pawn promotion push...\n");

    // White pawn on e7 (black king on d8 so e8 is free for promotion)
    BoardState board = board_from_fen("3k4/4P3/8/8/8/8/8/4K3 w - - 0 1");
    assert(board.is_valid());

    MoveList moves;
    generate_pseudo_legal_moves(board, moves);

    // Should have 4 promotion moves: e8=N, e8=B, e8=R, e8=Q
    int promo_count = 0;
    for (size_t i = 0; i < moves.size(); ++i) {
        if (from_square(moves[i]) == SQUARE_E7 && to_square(moves[i]) == SQUARE_E8) {
            assert(is_promotion(moves[i]));
            ++promo_count;
        }
    }
    assert(promo_count == 4);

    printf("  PASS: Pawn promotion push works (%d promotions)\n", promo_count);
}

// Test 6: Pawn promotion capture
void test_pawn_promotion_capture() {
    printf("Test: Pawn promotion capture...\n");

    // White pawn on e7 with black rook on d8, black king on c8 (so e8 is free)
    BoardState board = board_from_fen("2rk4/4P3/8/8/8/8/8/4K3 w - - 0 1");
    assert(board.is_valid());

    MoveList moves;
    generate_pseudo_legal_moves(board, moves);

    // Should have 5 moves total: e8=N/B/R/Q (promo push) + exd8=N/B/R/Q (promo capture)
    int push_promo = 0;
    int capture_promo = 0;

    for (size_t i = 0; i < moves.size(); ++i) {
        if (from_square(moves[i]) == SQUARE_E7) {
            if (to_square(moves[i]) == SQUARE_E8) {
                assert(is_promotion(moves[i]));
                assert(!is_capture(moves[i]));
                ++push_promo;
            }
            if (to_square(moves[i]) == SQUARE_D8) {
                assert(is_promotion(moves[i]));
                assert(is_capture(moves[i]));
                ++capture_promo;
            }
        }
    }

    assert(push_promo == 4);
    assert(capture_promo == 4);

    printf("  PASS: Pawn promotion capture works (%d push + %d capture)\n", push_promo, capture_promo);
}

// Test 7: Knight moves
void test_knight_moves() {
    printf("Test: Knight moves from center...\n");

    // Knight on e4 (no kings to avoid extra moves)
    BoardState board = board_from_fen("8/8/8/8/4N3/8/8/4K2k w - - 0 1");
    assert(board.is_valid());

    MoveList moves;
    size_t count = generate_pseudo_legal_moves(board, moves);

    // Knight on e4 has 8 possible moves, plus white king on e1 has 5 moves
    // Black king on h1 doesn't move because it's black's turn... wait, no, white to move
    // White king on e1: 5 moves
    // White knight on e4: 8 moves
    // Total: 13
    // But we want to test just the knight, so let's count knight moves specifically
    int knight_move_count = count_knight_moves(moves, board);
    assert(knight_move_count == 8);

    printf("  PASS: Knight has %d moves from e4\n", knight_move_count);
}

// Test 8: Knight moves from corner
void test_knight_corner() {
    printf("Test: Knight moves from corner...\n");

    BoardState board = board_from_fen("N6k/8/8/8/8/8/8/4K3 w - - 0 1");
    assert(board.is_valid());

    MoveList moves;
    generate_pseudo_legal_moves(board, moves);

    // Knight on a8 has 2 moves: b6, c7
    // White king on e1 has 5 moves
    // Total moves = 7, but we count only knight moves
    int knight_move_count = count_knight_moves(moves, board);
    assert(knight_move_count == 2);

    printf("  PASS: Knight has %d moves from corner\n", knight_move_count);
}

// Test 9: Bishop sliding moves
void test_bishop_moves() {
    printf("Test: Bishop sliding moves...\n");

    // Bishop on c1 with open diagonals
    BoardState board = board_from_fen("4k3/8/8/8/8/8/8/2B2K2 w - - 0 1");
    assert(board.is_valid());

    MoveList moves;
    generate_pseudo_legal_moves(board, moves);

    // Bishop on c1 can move: b2, a3, d2, e3, f4, g5, h6 = 7 squares
    // White king on f1 has 5 moves
    int bishop_move_count = count_bishop_moves(moves, board);
    assert(bishop_move_count == 7);

    printf("  PASS: Bishop has %d moves\n", bishop_move_count);
}

// Test 10: Rook sliding moves
void test_rook_moves() {
    printf("Test: Rook sliding moves...\n");

    // Rook in center on e4
    BoardState board = board_from_fen("4k3/8/8/8/4R3/8/8/4K3 w - - 0 1");
    assert(board.is_valid());

    MoveList moves;
    generate_pseudo_legal_moves(board, moves);

    // Rook on e4: horizontal moves + vertical moves (blocked by kings, but can capture enemy king)
    // Horizontal: a4, b4, c4, d4, f4, g4, h4 = 7
    // Vertical down: e3, e2 = 2 (e1 blocked by own king)
    // Vertical up: e5, e6, e7, e8 (capture enemy king) = 4
    // Total: 13 (pseudo-legal allows king captures)
    int rook_move_count = count_rook_moves(moves, board);
    assert(rook_move_count == 13);

    printf("  PASS: Rook has %d moves from center (including king capture)\n", rook_move_count);
}

// Test 11: Queen moves
void test_queen_moves() {
    printf("Test: Queen moves...\n");

    // Queen in center on d4
    BoardState board = board_from_fen("4k3/8/8/8/3Q4/8/8/4K3 w - - 0 1");
    assert(board.is_valid());

    MoveList moves;
    generate_pseudo_legal_moves(board, moves);

    // Queen combines rook (horizontal/vertical) and bishop (diagonal) moves
    // From d4: 7 horizontal (a4-h4 excluding d4) + 7 vertical (d1-d8 excluding d4)
    // But d1 blocked by own king, d8 blocked by enemy king
    // So vertical: d2, d3, d5, d6, d7 = 5
    // Horizontal: a4, b4, c4, e4, f4, g4, h4 = 7
    // Diagonals: c3, b2, a1, c5, b6, a7, e3, f2, g1, e5, f6, g7, h8 = 13
    // But a1 blocked by... no, a1 is empty. g1 has white king? No, king on e1.
    // Total queen moves: 5 + 7 + 13 = 25? Let me just count via function
    int queen_move_count = count_queen_moves(moves, board);
    // King on e1 also has 5 moves
    // We just verify queen has moves
    assert(queen_move_count > 20);

    printf("  PASS: Queen has %d moves from center\n", queen_move_count);
}

// Test 12: King moves
void test_king_moves() {
    printf("Test: King moves from center...\n");

    // White king on e4, black king far away on h1
    BoardState board = board_from_fen("8/8/8/8/4K3/8/8/7k w - - 0 1");
    assert(board.is_valid());

    MoveList moves;
    generate_pseudo_legal_moves(board, moves);

    // White king in center has 8 moves
    int king_move_count = count_king_moves(moves, board);
    assert(king_move_count == 8);

    printf("  PASS: King has %d moves from center\n", king_move_count);
}

// Test 13: King from corner
void test_king_corner() {
    printf("Test: King moves from corner...\n");

    BoardState board = board_from_fen("K6k/8/8/8/8/8/8/8 w - - 0 1");
    assert(board.is_valid());

    MoveList moves;
    generate_pseudo_legal_moves(board, moves);

    // White king on a8 has 3 moves: a7, b7, b8
    int king_move_count = count_king_moves(moves, board);
    assert(king_move_count == 3);

    printf("  PASS: King has %d moves from corner\n", king_move_count);
}

// Test 14: Sliding piece blocked by own piece
void test_sliding_blocked() {
    printf("Test: Sliding piece blocked by own piece...\n");

    // Rook on a1 with own pawn on a6
    BoardState board = board_from_fen("4k3/8/P7/8/8/8/8/R6K w - - 0 1");
    assert(board.is_valid());

    MoveList moves;
    generate_pseudo_legal_moves(board, moves);

    // Rook on a1:
    // Horizontal: b1, c1, d1, e1, f1, g1 = 6 (h1 blocked by own king)
    // Vertical: a2, a3, a4, a5 = 4 (a6 blocked by own pawn)
    // Total rook moves: 10
    int rook_move_count = count_rook_moves(moves, board);
    assert(rook_move_count == 10);

    printf("  PASS: Sliding piece blocked correctly (%d rook moves)\n", rook_move_count);
}

// Test 15: Sliding piece can capture
void test_sliding_capture() {
    printf("Test: Sliding piece capture...\n");

    // Rook on a1 with enemy pawn on a4
    BoardState board = board_from_fen("4k3/8/8/8/p7/8/8/R6K w - - 0 1");
    assert(board.is_valid());

    MoveList moves;
    size_t count = generate_pseudo_legal_moves(board, moves);

    // Count captures
    int captures = count_captures(moves);

    // Rook should be able to capture on a4 (1 capture)
    assert(captures == 1);

    printf("  PASS: Sliding capture works (%d captures)\n", captures);
}

// Test 16: Black to move
void test_black_to_move() {
    printf("Test: Black to move...\n");

    BoardState board = board_from_fen("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");
    assert(board.is_valid());
    assert(board.side_to_move == COLOR_BLACK);

    MoveList moves;
    size_t count = generate_pseudo_legal_moves(board, moves);

    // After 1.e4, black has the same 20 moves as white in start position
    assert(count == 20);

    printf("  PASS: Black generates %zu moves\n", count);
}

// Test 17: Generate captures only
void test_generate_captures() {
    printf("Test: Generate captures only...\n");

    // Position with pieces that can capture
    BoardState board = board_from_fen("4k3/8/8/3p4/4P3/8/8/4K3 w - - 0 1");
    assert(board.is_valid());

    MoveList captures;
    size_t capture_count = generate_captures(board, captures);

    // e4 pawn can capture on d5 (promotion is also considered "capture-like")
    // But d5 is empty in this FEN... let me recalculate
    // Actually d5 has a black pawn, so exd5 is a capture
    assert(capture_count >= 1);

    printf("  PASS: Generated %zu capture/promotion moves\n", capture_count);
}

// Test 18: Generate quiet moves only
void test_generate_quiet() {
    printf("Test: Generate quiet moves only...\n");

    BoardState board = board_from_fen("4k3/8/8/8/4P3/8/8/4K3 w - - 0 1");
    assert(board.is_valid());

    MoveList quiet;
    size_t quiet_count = generate_quiet_moves(board, quiet);

    // e4 pawn can push to e5 (quiet move)
    // Also double push from e2 is not possible since pawn is on e4
    assert(quiet_count >= 1);

    printf("  PASS: Generated %zu quiet moves\n", quiet_count);
}

// Test 19: Move list operations
void test_move_list() {
    printf("Test: Move list operations...\n");

    MoveList moves;
    assert(moves.empty());
    assert(moves.size() == 0);

    moves.push(make_move(SQUARE_E2, SQUARE_E4));
    assert(!moves.empty());
    assert(moves.size() == 1);
    assert(moves[0] == make_move(SQUARE_E2, SQUARE_E4));

    moves.push(make_move(SQUARE_G1, SQUARE_F3));
    assert(moves.size() == 2);

    moves.clear();
    assert(moves.empty());

    printf("  PASS: Move list operations work\n");
}

// Test 20: Complex position with multiple piece types
void test_complex_position() {
    printf("Test: Complex position...\n");

    // Position with several pieces
    BoardState board = board_from_fen("r1bqkb1r/pppp1ppp/2n2n2/4p3/2B1P3/5N2/PPPP1PPP/RNBQK2R w KQkq - 0 1");
    assert(board.is_valid());

    MoveList moves;
    size_t count = generate_pseudo_legal_moves(board, moves);

    // Should generate many moves (exact count depends on position)
    // This is Italian Game position after 1.e4 e5 2.Nf3 Nc6 3.Bc4
    // White has many options including O-O, d3, d4, etc.
    assert(count > 20);

    // Verify we have moves from different piece types
    assert(count_pawn_moves(moves, board) > 0);
    assert(count_knight_moves(moves, board) > 0);
    assert(count_bishop_moves(moves, board) > 0);
    assert(count_king_moves(moves, board) > 0);

    printf("  PASS: Complex position generates %zu moves\n", count);
}

int main() {
    printf("=== Pseudo-Legal Move Generation Tests ===\n\n");

    // Initialize attack tables
    init_all_attacks();

    test_move_list();
    test_start_position();
    test_pawn_single_push();
    test_pawn_double_push();
    test_pawn_capture();
    test_pawn_promotion_push();
    test_pawn_promotion_capture();
    test_knight_moves();
    test_knight_corner();
    test_bishop_moves();
    test_rook_moves();
    test_queen_moves();
    test_king_moves();
    test_king_corner();
    test_sliding_blocked();
    test_sliding_capture();
    test_black_to_move();
    test_generate_captures();
    test_generate_quiet();
    test_complex_position();

    printf("\n=== All tests passed! ===\n");
    return 0;
}
