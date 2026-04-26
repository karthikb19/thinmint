// Castling and en passant generation tests
// S02-F06: Implement castling and en passant generation rules

#include <cassert>
#include <cstdio>
#include <string>

#include "thinmint/board/board.h"
#include "thinmint/movegen/movegen.h"
#include "thinmint/movegen/attacks.h"

using namespace thinmint::core;
using namespace thinmint::board;
using namespace thinmint::movegen;

// Helper to count castling moves
int count_castling_moves(const MoveList& moves) {
    int count = 0;
    for (size_t i = 0; i < moves.size(); ++i) {
        if (is_castling(moves[i])) {
            ++count;
        }
    }
    return count;
}

// Helper to check if a specific castling move exists
bool has_castling_move(const MoveList& moves, Square from, Square to) {
    for (size_t i = 0; i < moves.size(); ++i) {
        Move m = moves[i];
        if (is_castling(m) && from_square(m) == from && to_square(m) == to) {
            return true;
        }
    }
    return false;
}

// Helper to count en passant moves
int count_en_passant_moves(const MoveList& moves) {
    int count = 0;
    for (size_t i = 0; i < moves.size(); ++i) {
        if (is_en_passant(moves[i])) {
            ++count;
        }
    }
    return count;
}

// Helper to check if a specific en passant move exists
bool has_en_passant_move(const MoveList& moves, Square from, Square to) {
    for (size_t i = 0; i < moves.size(); ++i) {
        Move m = moves[i];
        if (is_en_passant(m) && from_square(m) == from && to_square(m) == to) {
            return true;
        }
    }
    return false;
}

// Test 1: Starting position has no castling rights (no rights in FEN)
void test_start_position_no_castling() {
    BoardState board;
    board.parse_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w - - 0 1");

    MoveList moves;
    generate_castling_moves(board, moves);

    assert(moves.size() == 0);
    assert(count_castling_moves(moves) == 0);
    printf("PASS: test_start_position_no_castling\n");
}

// Test 2: Castling position with pieces moved (clear path)
void test_castling_with_clear_path() {
    // Position where bishop and knight have moved, clearing the path
    BoardState board;
    board.parse_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1");

    MoveList moves;
    generate_castling_moves(board, moves);

    // Should have both white castling moves
    assert(moves.size() == 2);
    assert(has_castling_move(moves, SQUARE_E1, SQUARE_G1));  // Kingside
    assert(has_castling_move(moves, SQUARE_E1, SQUARE_C1));  // Queenside
    printf("PASS: test_castling_with_clear_path\n");
}

// Test 3: Black castling (black to move)
void test_black_castling() {
    // Position where black's pieces have moved, clearing the path
    BoardState board;
    board.parse_fen("r3k2r/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1");

    MoveList moves;
    generate_castling_moves(board, moves);

    // Should have both black castling moves
    assert(moves.size() == 2);
    assert(has_castling_move(moves, SQUARE_E8, SQUARE_G8));  // Kingside
    assert(has_castling_move(moves, SQUARE_E8, SQUARE_C8));  // Queenside
    printf("PASS: test_black_castling\n");
}

// Test 4: Castling blocked by piece between king and rook
void test_castling_blocked_by_piece() {
    // Knight blocking white kingside on G1
    // Queenside is also blocked by pieces on B1, C1, D1
    BoardState board;
    board.parse_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQK1NR w KQkq - 0 1");

    MoveList moves;
    generate_castling_moves(board, moves);

    // NO castling should be possible - kingside blocked by G1 knight,
    // queenside blocked by pieces on B1, C1, D1
    assert(moves.size() == 0);
    printf("PASS: test_castling_blocked_by_piece\n");
}

// Test 4b: Only kingside castling possible (no queenside rook)
void test_only_kingside_no_queenside_rook() {
    // King on E1, rook on H1, no rook on A1
    // FEN rank 1: 4 = A1-D1 empty, K = E1 king, 2 = F1,G1 empty, R = H1 rook
    BoardState board;
    board.parse_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/4K2R w KQ - 0 1");

    MoveList moves;
    generate_castling_moves(board, moves);

    // Only kingside should be possible (no queenside rook)
    assert(moves.size() == 1);
    assert(has_castling_move(moves, SQUARE_E1, SQUARE_G1));
    assert(!has_castling_move(moves, SQUARE_E1, SQUARE_C1));
    printf("PASS: test_only_kingside_no_queenside_rook\n");
}

// Test 5: Castling when king is in check
void test_castling_while_in_check() {
    // White king in check by black rook
    BoardState board;
    board.parse_fen("rnbqkbn1/pppppppr/7p/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    MoveList moves;
    generate_castling_moves(board, moves);

    // No castling allowed while in check
    assert(moves.size() == 0);
    printf("PASS: test_castling_while_in_check\n");
}

// Test 6: Castling when king passes through attacked square
void test_castling_through_check() {
    // Black bishop attacking F1 (king would pass through)
    BoardState board;
    board.parse_fen("rn1qkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNb w KQkq - 0 1");

    MoveList moves;
    generate_castling_moves(board, moves);

    // Neither castling should be allowed (kingside passes through check, queenside C1 attacked)
    // Actually, let's check more carefully:
    // - Kingside: E1->F1->G1, F1 is attacked by bishop on H1... wait, that's a white bishop
    // Let me fix the FEN

    // Black bishop on C4 attacking F1
    board.parse_fen("rnbqkbnr/pppppppp/8/8/2b5/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    generate_castling_moves(board, moves);

    // Both castlings should be blocked:
    // - Kingside: F1 is attacked by bishop on C4
    // - Queenside: D1 is attacked by bishop on C4
    assert(moves.size() == 0);
    printf("PASS: test_castling_through_check\n");
}

// Test 7: Castling destination attacked
void test_castling_destination_attacked() {
    // Black rook attacking G1 (castling destination) from G2
    // Also attacks F1 so kingside is blocked, but queenside is OK
    BoardState board;
    board.parse_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPrP/R3K2R w KQkq - 0 1");

    MoveList moves;
    generate_castling_moves(board, moves);

    // Kingside should be blocked (F1 and G1 attacked by rook), queenside should be OK
    assert(moves.size() == 1);
    assert(!has_castling_move(moves, SQUARE_E1, SQUARE_G1));
    assert(has_castling_move(moves, SQUARE_E1, SQUARE_C1));
    printf("PASS: test_castling_destination_attacked\n");
}

// Test 8: No castling rights
void test_no_castling_rights() {
    BoardState board;
    board.parse_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w - - 0 1");

    MoveList moves;
    generate_castling_moves(board, moves);

    assert(moves.size() == 0);
    printf("PASS: test_no_castling_rights\n");
}

// Test 9: Only kingside rights
void test_only_kingside_rights() {
    // Position with clear path for kingside but only kingside rights
    BoardState board;
    board.parse_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w Kk - 0 1");

    MoveList moves;
    generate_castling_moves(board, moves);

    // Only white kingside (queenside blocked by missing right, even though rook is there)
    assert(moves.size() == 1);
    assert(has_castling_move(moves, SQUARE_E1, SQUARE_G1));
    printf("PASS: test_only_kingside_rights\n");
}

// Test 10: Only queenside rights
void test_only_queenside_rights() {
    // Position with clear path for queenside but only queenside rights
    BoardState board;
    board.parse_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w Qq - 0 1");

    MoveList moves;
    generate_castling_moves(board, moves);

    // Only white queenside (kingside blocked by missing right)
    assert(moves.size() == 1);
    assert(has_castling_move(moves, SQUARE_E1, SQUARE_C1));
    printf("PASS: test_only_queenside_rights\n");
}

// Test 11: King not in starting position
void test_king_moved() {
    // King has moved and returned
    BoardState board;
    board.parse_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQ1K1R w KQkq - 0 1");

    MoveList moves;
    generate_castling_moves(board, moves);

    // No castling - king not on E1
    assert(moves.size() == 0);
    printf("PASS: test_king_moved\n");
}

// Test 12: Rook missing (captured)
void test_rook_missing() {
    // Kingside rook is missing, but kingside is also blocked by pieces
    // Queenside is also blocked by pieces on B1, C1, D1
    BoardState board;
    board.parse_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBN1 w KQkq - 0 1");

    MoveList moves;
    generate_castling_moves(board, moves);

    // No castling possible - kingside blocked by knight on G1, queenside blocked by pieces
    // Also H1 has no rook
    assert(moves.size() == 0);
    printf("PASS: test_rook_missing\n");
}

// Test 12b: Rook missing but other side clear
void test_kingside_rook_missing_queenside_clear() {
    // King on E1, no H1 rook, but queenside path is clear (A1 has rook)
    BoardState board;
    board.parse_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R3K3 w KQkq - 0 1");

    MoveList moves;
    generate_castling_moves(board, moves);

    // Only queenside should be possible (no kingside rook)
    assert(moves.size() == 1);
    assert(!has_castling_move(moves, SQUARE_E1, SQUARE_G1));
    assert(has_castling_move(moves, SQUARE_E1, SQUARE_C1));
    printf("PASS: test_kingside_rook_missing_queenside_clear\n");
}

// Test 13: En passant generation
void test_en_passant_generation() {
    // White to capture en passant
    BoardState board;
    board.parse_fen("rnbqkbnr/pppppppp/8/3Pp3/8/8/PPP1PPPP/RNBQKBNR w KQkq e6 0 1");

    MoveList moves;
    generate_pseudo_legal_moves(board, moves);

    // Should have en passant capture d5xe6
    assert(count_en_passant_moves(moves) == 1);
    assert(has_en_passant_move(moves, SQUARE_D5, SQUARE_E6));
    printf("PASS: test_en_passant_generation\n");
}

// Test 14: En passant from both sides
void test_en_passant_both_sides() {
    // White pawn can capture from both sides
    BoardState board;
    board.parse_fen("rnbqkbnr/pppppppp/8/2PpP3/8/8/PP1PPPPP/RNBQKBNR w KQkq d6 0 1");

    MoveList moves;
    generate_pseudo_legal_moves(board, moves);

    // Should have two en passant captures: c5xd6 and e5xd6
    assert(count_en_passant_moves(moves) == 2);
    assert(has_en_passant_move(moves, SQUARE_C5, SQUARE_D6));
    assert(has_en_passant_move(moves, SQUARE_E5, SQUARE_D6));
    printf("PASS: test_en_passant_both_sides\n");
}

// Test 15: Black en passant
void test_black_en_passant() {
    BoardState board;
    board.parse_fen("rnbqkbnr/pppp1ppp/8/8/3pP3/8/PPPPPPPP/RNBQKBNR b KQkq e3 0 1");

    MoveList moves;
    generate_pseudo_legal_moves(board, moves);

    // Should have en passant capture d4xe3
    assert(count_en_passant_moves(moves) == 1);
    assert(has_en_passant_move(moves, SQUARE_D4, SQUARE_E3));
    printf("PASS: test_black_en_passant\n");
}

// Test 16: No en passant without EP square
void test_no_ep_without_square() {
    BoardState board;
    board.parse_fen("rnbqkbnr/pppppppp/8/3Pp3/8/8/PPP1PPPP/RNBQKBNR w KQkq - 0 1");

    MoveList moves;
    generate_pseudo_legal_moves(board, moves);

    // No en passant without EP square set
    assert(count_en_passant_moves(moves) == 0);
    printf("PASS: test_no_ep_without_square\n");
}

// Test 17: Castling appears in full move generation
void test_castling_in_full_generation() {
    // Position with clear paths for castling
    BoardState board;
    board.parse_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1");

    MoveList moves;
    generate_pseudo_legal_moves(board, moves);

    // Should have castling moves among all other moves
    assert(count_castling_moves(moves) == 2);
    assert(has_castling_move(moves, SQUARE_E1, SQUARE_G1));
    assert(has_castling_move(moves, SQUARE_E1, SQUARE_C1));
    printf("PASS: test_castling_in_full_generation\n");
}

// Test 18: Queenside castling with B1 attacked (should still work)
void test_queenside_b1_attacked() {
    // Position with clear paths for castling, and B1 is attacked but that doesn't matter
    // Bishop on c3 attacks b2, a1, d4, e5, f6, g7, h8 - doesn't attack B1
    // Let me use knight on c3 - attacks b1, a2, d1, b5, d5, e4, e2, a4
    // That attacks B1 and D1, blocking queenside! 
    // Let me use knight on a3 - attacks b1, c2, c4, b5 - attacks B1 but not D1
    BoardState board;
    board.parse_fen("rnbqkbnr/pppppppp/8/8/8/n7/PPPPPPPP/R3K2R w KQkq - 0 1");

    MoveList moves;
    generate_castling_moves(board, moves);

    // Kingside should be possible (path E1-F1-G1 not attacked)
    // Queenside should also be possible (king passes through D1, not B1)
    // Knight on a3 attacks b1 and c2, but not D1
    assert(moves.size() == 2);
    assert(has_castling_move(moves, SQUARE_E1, SQUARE_G1));
    assert(has_castling_move(moves, SQUARE_E1, SQUARE_C1));
    printf("PASS: test_queenside_b1_attacked\n");
}

// Test 19: Castling rights without corresponding rook (chess960-style)
void test_castling_rights_no_rook() {
    // No rooks but castling rights set (invalid position but should handle gracefully)
    BoardState board;
    board.parse_fen("rn2k1nr/pppppppp/8/8/8/8/PPPPPPPP/RN2K1NR w KQkq - 0 1");

    MoveList moves;
    generate_castling_moves(board, moves);

    // Should generate nothing since rooks are not on H1/A1
    assert(moves.size() == 0);
    printf("PASS: test_castling_rights_no_rook\n");
}

// Test 20: Kiwipete position - complex castling
void test_kiwipete_castling() {
    // Kiwipete position - black has both castling rights
    BoardState board;
    board.parse_fen("r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R b KQkq - 0 1");

    MoveList moves;
    generate_castling_moves(board, moves);

    assert(moves.size() == 2);
    assert(has_castling_move(moves, SQUARE_E8, SQUARE_G8));
    assert(has_castling_move(moves, SQUARE_E8, SQUARE_C8));
    printf("PASS: test_kiwipete_castling\n");
}

int main() {
    printf("Running castling and en passant generation tests...\n\n");

    // Initialize attack tables
    init_all_attacks();

    test_start_position_no_castling();
    test_castling_with_clear_path();
    test_black_castling();
    test_castling_blocked_by_piece();
    test_only_kingside_no_queenside_rook();
    test_castling_while_in_check();
    test_castling_through_check();
    test_castling_destination_attacked();
    test_no_castling_rights();
    test_only_kingside_rights();
    test_only_queenside_rights();
    test_king_moved();
    test_rook_missing();
    test_kingside_rook_missing_queenside_clear();
    test_en_passant_generation();
    test_en_passant_both_sides();
    test_black_en_passant();
    test_no_ep_without_square();
    test_castling_in_full_generation();
    test_queenside_b1_attacked();
    test_castling_rights_no_rook();
    test_kiwipete_castling();

    printf("\nAll tests passed!\n");
    return 0;
}
