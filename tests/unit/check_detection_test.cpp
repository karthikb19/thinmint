#include <cassert>
#include <cstdio>

#include "thinmint/board/board.h"
#include "thinmint/movegen/attacks.h"

using namespace thinmint::core;
using namespace thinmint::board;
using namespace thinmint::movegen;

// Test that starting position is not in check
void test_start_position_not_in_check() {
    BoardState board;
    board.reset_to_start_position();

    // Neither side is in check at the start
    assert(!board.is_check(COLOR_WHITE));
    assert(!board.is_check(COLOR_BLACK));

    printf("  PASS: Start position not in check\n");
}

// Test simple check by queen
void test_queen_check() {
    // White king on e1, Black queen on e8 giving check
    BoardState board = board_from_fen("4k3/4q3/8/8/8/8/8/4K3 w - - 0 1");
    assert(board.is_valid());

    // White king is in check from queen on e8
    assert(board.is_check(COLOR_WHITE));
    // Black king is not in check
    assert(!board.is_check(COLOR_BLACK));

    printf("  PASS: Queen check detected\n");
}

// Test check by rook
void test_rook_check() {
    // White king on e1, Black rook on e8 giving check
    BoardState board = board_from_fen("4k3/4r3/8/8/8/8/8/4K3 w - - 0 1");
    assert(board.is_valid());

    assert(board.is_check(COLOR_WHITE));
    assert(!board.is_check(COLOR_BLACK));

    printf("  PASS: Rook check detected\n");
}

// Test check by bishop
void test_bishop_check() {
    // White king on e1, Black bishop on a5 giving diagonal check
    // a5 -> b4 -> c3 -> d2 -> e1 is the diagonal
    BoardState board = board_from_fen("4k3/8/8/b7/8/8/8/4K3 w - - 0 1");
    assert(board.is_valid());

    assert(board.is_check(COLOR_WHITE));
    assert(!board.is_check(COLOR_BLACK));

    printf("  PASS: Bishop check detected\n");
}

// Test check by knight
void test_knight_check() {
    // White king on e1, Black knight on d3 or f3 giving check
    BoardState board = board_from_fen("4k3/8/8/8/8/3n4/8/4K3 w - - 0 1");
    assert(board.is_valid());

    assert(board.is_check(COLOR_WHITE));
    assert(!board.is_check(COLOR_BLACK));

    printf("  PASS: Knight check detected\n");
}

// Test check by pawn
void test_pawn_check() {
    // White king on e3, Black pawn on d4 or f4 giving check
    BoardState board = board_from_fen("4k3/8/8/8/3p4/4K3/8/8 w - - 0 1");
    assert(board.is_valid());

    // King on e3 is attacked by pawn on d4 (pawn attacks diagonally)
    assert(board.is_check(COLOR_WHITE));
    assert(!board.is_check(COLOR_BLACK));

    printf("  PASS: Pawn check detected\n");
}

// Test discovered check (piece moves away, revealing check)
// Note: This tests that check detection works after a move would be made
void test_is_square_attacked_function() {
    BoardState board;
    board.reset_to_start_position();

    // In start position, e2 square is not attacked by Black
    Square e2 = make_square(FILE_E, RANK_2);
    assert(!is_square_attacked(board, e2, COLOR_BLACK));

    // e4 square is attacked by pawn on d5 if we place one
    BoardState board2 = board_from_fen("rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 2");
    assert(board2.is_valid());

    // After 1. e4 d5, e4 is attacked by pawn on d5
    Square e4 = make_square(FILE_E, RANK_4);
    assert(is_square_attacked(board2, e4, COLOR_BLACK));

    printf("  PASS: is_square_attacked works correctly\n");
}

// Test check blocked by piece
void test_check_blocked() {
    // White king on e1, Black queen on e8, but pawn on e2 blocking
    BoardState board = board_from_fen("4k3/4q3/8/8/8/8/4P3/4K3 w - - 0 1");
    assert(board.is_valid());

    // Queen's attack is blocked by pawn on e2
    assert(!board.is_check(COLOR_WHITE));

    printf("  PASS: Blocked check correctly not detected\n");
}

// Test double check
void test_double_check() {
    // White king on e1, attacked by both queen on e8 and bishop on a5
    // But bishop is blocked... Let's try another position
    // King on e1, knight on d3 and queen on e8 (but queen is blocked)
    // Actually let's use two knights
    BoardState board = board_from_fen("4k3/8/8/8/8/3n1n2/8/4K3 w - - 0 1");
    assert(board.is_valid());

    // Both knights attack e1
    assert(board.is_check(COLOR_WHITE));

    printf("  PASS: Double check detected\n");
}

// Test check from king (adjacent kings is illegal, but we test attack detection)
void test_king_attack_detection() {
    // Test is_square_attacked for king attacks
    BoardState board;
    board.reset_to_start_position();

    // Squares adjacent to black king on e8
    Square d7 = make_square(FILE_D, RANK_7);
    Square e7 = make_square(FILE_E, RANK_7);
    Square f7 = make_square(FILE_F, RANK_7);

    // These should be attacked by the black king
    assert(is_square_attacked(board, d7, COLOR_BLACK));
    assert(is_square_attacked(board, e7, COLOR_BLACK));
    assert(is_square_attacked(board, f7, COLOR_BLACK));

    // e1 is not attacked by black king (too far)
    Square e1 = make_square(FILE_E, RANK_1);
    assert(!is_square_attacked(board, e1, COLOR_BLACK));

    printf("  PASS: King attack detection works\n");
}

// Test discovered check scenario
void test_discovered_check() {
    // Position where sliding piece attacks along a ray with a blocker
    // If blocker moves, the check is discovered
    BoardState board = board_from_fen("4k3/8/8/8/3r4/8/4P3/4K3 w - - 0 1");
    assert(board.is_valid());

    // Rook on d4 attacks along the rank, but e2 pawn blocks
    // King on e1 is not in check
    assert(!board.is_check(COLOR_WHITE));

    // But e2 IS attacked by the rook (if the pawn weren't there)
    // Actually the pawn IS there, so e2 is not attacked
    // Let's verify the rook attacks work properly
    Square e2 = make_square(FILE_E, RANK_2);
    assert(!is_square_attacked(board, e2, COLOR_BLACK));  // Blocked by pawn

    Square d2 = make_square(FILE_D, RANK_2);
    assert(is_square_attacked(board, d2, COLOR_BLACK));   // Not blocked

    printf("  PASS: Discovered check scenario handled\n");
}

// Test edge cases - king on edge of board
void test_check_on_board_edges() {
    // King on a1, rook on a8 giving check
    BoardState board = board_from_fen("4k2r/8/8/8/8/8/8/K7 w - - 0 1");
    assert(board.is_valid());

    // Note: This position might have the rook attacking a1... let's check
    // Actually the rook is on h8 in this FEN, not a8
    // Let me fix: ra8 is rook on a8
    BoardState board2 = board_from_fen("r3k3/8/8/8/8/8/8/K7 w - - 0 1");
    assert(board2.is_valid());

    // Rook on a8 attacks down the a-file to a1
    assert(board2.is_check(COLOR_WHITE));

    printf("  PASS: Check detection works on board edges\n");
}

// Test that FEN parsing doesn't reject positions where side to move is in check
void test_check_position_parses() {
    // This position has White in check and it's White's move
    BoardState board = board_from_fen("4k3/4r3/8/8/8/8/8/4K3 w - - 0 1");
    assert(board.is_valid());
    assert(board.is_check(COLOR_WHITE));

    printf("  PASS: Positions with side-to-move in check parse correctly\n");
}

int main() {
    printf("Running check detection tests...\n\n");

    // Initialize attack tables
    init_all_attacks();

    test_start_position_not_in_check();
    test_queen_check();
    test_rook_check();
    test_bishop_check();
    test_knight_check();
    test_pawn_check();
    test_is_square_attacked_function();
    test_check_blocked();
    test_double_check();
    test_king_attack_detection();
    test_discovered_check();
    test_check_on_board_edges();
    test_check_position_parses();

    printf("\nAll check detection tests passed!\n");
    return 0;
}
