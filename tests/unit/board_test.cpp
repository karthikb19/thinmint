#include "thinmint/board/board.h"

#include <cassert>
#include <iostream>
#include <cstring>

using namespace thinmint::board;
using namespace thinmint::core;

// Helper to count set bits in a bitboard (for testing)
int count_bits(uint64_t bb) {
    int count = 0;
    while (bb) {
        bb &= bb - 1;
        ++count;
    }
    return count;
}

// Test 1: BoardState default constructor creates empty board
void test_default_constructor() {
    BoardState board;

    assert(board.is_empty());
    assert(board.white_occupancy == BB_EMPTY);
    assert(board.black_occupancy == BB_EMPTY);
    assert(board.all_occupancy == BB_EMPTY);
    assert(board.side_to_move == COLOR_WHITE);
    assert(board.castling_rights == CASTLING_NONE);
    assert(board.en_passant_square == SQUARE_NONE);
    assert(board.halfmove_clock == 0);
    assert(board.fullmove_number == 1);

    std::cout << "PASS: test_default_constructor\n";
}

// Test 2: BoardState clear() resets to empty
void test_clear() {
    BoardState board;
    board.reset_to_start_position();
    assert(!board.is_empty());

    board.clear();
    assert(board.is_empty());
    assert(board.side_to_move == COLOR_WHITE);

    std::cout << "PASS: test_clear\n";
}

// Test 3: Start position has correct piece counts
void test_start_position_pieces() {
    BoardState board;
    board.reset_to_start_position();

    // Check total pieces
    assert(board.piece_count() == 32);

    // Check white pieces
    assert(board.piece_count(COLOR_WHITE, PIECE_PAWN) == 8);
    assert(board.piece_count(COLOR_WHITE, PIECE_ROOK) == 2);
    assert(board.piece_count(COLOR_WHITE, PIECE_KNIGHT) == 2);
    assert(board.piece_count(COLOR_WHITE, PIECE_BISHOP) == 2);
    assert(board.piece_count(COLOR_WHITE, PIECE_QUEEN) == 1);
    assert(board.piece_count(COLOR_WHITE, PIECE_KING) == 1);

    // Check black pieces
    assert(board.piece_count(COLOR_BLACK, PIECE_PAWN) == 8);
    assert(board.piece_count(COLOR_BLACK, PIECE_ROOK) == 2);
    assert(board.piece_count(COLOR_BLACK, PIECE_KNIGHT) == 2);
    assert(board.piece_count(COLOR_BLACK, PIECE_BISHOP) == 2);
    assert(board.piece_count(COLOR_BLACK, PIECE_QUEEN) == 1);
    assert(board.piece_count(COLOR_BLACK, PIECE_KING) == 1);

    std::cout << "PASS: test_start_position_pieces\n";
}

// Test 4: Start position has correct occupancy
void test_start_position_occupancy() {
    BoardState board;
    board.reset_to_start_position();

    // White should occupy ranks 1-2 (bits 0-15)
    assert(board.white_occupancy == 0x000000000000FFFFULL);

    // Black should occupy ranks 7-8 (bits 48-63)
    assert(board.black_occupancy == 0xFFFF000000000000ULL);

    // Combined
    assert(board.all_occupancy == (0x000000000000FFFFULL | 0xFFFF000000000000ULL));

    std::cout << "PASS: test_start_position_occupancy\n";
}

// Test 5: Start position game state
void test_start_position_state() {
    BoardState board;
    board.reset_to_start_position();

    assert(board.side_to_move == COLOR_WHITE);
    assert(board.castling_rights == CASTLING_ALL);
    assert(board.en_passant_square == SQUARE_NONE);
    assert(board.halfmove_clock == 0);
    assert(board.fullmove_number == 1);

    std::cout << "PASS: test_start_position_state\n";
}

// Test 6: piece_at returns correct pieces in start position
void test_piece_at_start_position() {
    BoardState board;
    board.reset_to_start_position();

    // White back rank (A1-H1)
    assert(board.piece_at(SQUARE_A1) == PIECE_W_ROOK);
    assert(board.piece_at(SQUARE_B1) == PIECE_W_KNIGHT);
    assert(board.piece_at(SQUARE_C1) == PIECE_W_BISHOP);
    assert(board.piece_at(SQUARE_D1) == PIECE_W_QUEEN);
    assert(board.piece_at(SQUARE_E1) == PIECE_W_KING);
    assert(board.piece_at(SQUARE_F1) == PIECE_W_BISHOP);
    assert(board.piece_at(SQUARE_G1) == PIECE_W_KNIGHT);
    assert(board.piece_at(SQUARE_H1) == PIECE_W_ROOK);

    // White pawns (A2-H2)
    assert(board.piece_at(SQUARE_A2) == PIECE_W_PAWN);
    assert(board.piece_at(SQUARE_E2) == PIECE_W_PAWN);
    assert(board.piece_at(SQUARE_H2) == PIECE_W_PAWN);

    // Black back rank (A8-H8)
    assert(board.piece_at(SQUARE_A8) == PIECE_B_ROOK);
    assert(board.piece_at(SQUARE_B8) == PIECE_B_KNIGHT);
    assert(board.piece_at(SQUARE_C8) == PIECE_B_BISHOP);
    assert(board.piece_at(SQUARE_D8) == PIECE_B_QUEEN);
    assert(board.piece_at(SQUARE_E8) == PIECE_B_KING);
    assert(board.piece_at(SQUARE_F8) == PIECE_B_BISHOP);
    assert(board.piece_at(SQUARE_G8) == PIECE_B_KNIGHT);
    assert(board.piece_at(SQUARE_H8) == PIECE_B_ROOK);

    // Black pawns (A7-H7)
    assert(board.piece_at(SQUARE_A7) == PIECE_B_PAWN);
    assert(board.piece_at(SQUARE_E7) == PIECE_B_PAWN);
    assert(board.piece_at(SQUARE_H7) == PIECE_B_PAWN);

    // Empty squares
    assert(board.piece_at(SQUARE_A3) == PIECE_NO_PIECE);
    assert(board.piece_at(SQUARE_E4) == PIECE_NO_PIECE);
    assert(board.piece_at(SQUARE_H6) == PIECE_NO_PIECE);

    std::cout << "PASS: test_piece_at_start_position\n";
}

// Test 7: piece_type_at returns correct types
void test_piece_type_at() {
    BoardState board;
    board.reset_to_start_position();

    assert(board.piece_type_at(SQUARE_E1) == PIECE_KING);
    assert(board.piece_type_at(SQUARE_E8) == PIECE_KING);
    assert(board.piece_type_at(SQUARE_D1) == PIECE_QUEEN);
    assert(board.piece_type_at(SQUARE_A2) == PIECE_PAWN);
    assert(board.piece_type_at(SQUARE_E4) == PIECE_NONE);

    std::cout << "PASS: test_piece_type_at\n";
}

// Test 8: color_at returns correct colors
void test_color_at() {
    BoardState board;
    board.reset_to_start_position();

    assert(board.color_at(SQUARE_E1) == COLOR_WHITE);
    assert(board.color_at(SQUARE_E8) == COLOR_BLACK);
    assert(board.color_at(SQUARE_A2) == COLOR_WHITE);
    assert(board.color_at(SQUARE_A7) == COLOR_BLACK);
    // Empty squares return WHITE by default
    assert(board.color_at(SQUARE_E4) == COLOR_WHITE);

    std::cout << "PASS: test_color_at\n";
}

// Test 9: king_square returns correct king positions
void test_king_square() {
    BoardState board;
    board.reset_to_start_position();

    assert(board.king_square(COLOR_WHITE) == SQUARE_E1);
    assert(board.king_square(COLOR_BLACK) == SQUARE_E8);

    std::cout << "PASS: test_king_square\n";
}

// Test 10: is_square_occupied works correctly
void test_is_square_occupied() {
    BoardState board;
    board.reset_to_start_position();

    assert(board.is_square_occupied(SQUARE_E1));
    assert(board.is_square_occupied(SQUARE_E2));
    assert(!board.is_square_occupied(SQUARE_E4));
    assert(!board.is_square_occupied(SQUARE_E5));
    assert(board.is_square_occupied(SQUARE_E7));
    assert(board.is_square_occupied(SQUARE_E8));

    std::cout << "PASS: test_is_square_occupied\n";
}

// Test 11: is_square_occupied_by works correctly
void test_is_square_occupied_by() {
    BoardState board;
    board.reset_to_start_position();

    assert(board.is_square_occupied_by(SQUARE_E1, COLOR_WHITE));
    assert(board.is_square_occupied_by(SQUARE_E2, COLOR_WHITE));
    assert(board.is_square_occupied_by(SQUARE_E7, COLOR_BLACK));
    assert(board.is_square_occupied_by(SQUARE_E8, COLOR_BLACK));

    assert(!board.is_square_occupied_by(SQUARE_E1, COLOR_BLACK));
    assert(!board.is_square_occupied_by(SQUARE_E8, COLOR_WHITE));
    assert(!board.is_square_occupied_by(SQUARE_E4, COLOR_WHITE));

    std::cout << "PASS: test_is_square_occupied_by\n";
}

// Test 12: occupancy_of works correctly
void test_occupancy_of() {
    BoardState board;
    board.reset_to_start_position();

    assert(board.occupancy_of(COLOR_WHITE) == board.white_occupancy);
    assert(board.occupancy_of(COLOR_BLACK) == board.black_occupancy);

    std::cout << "PASS: test_occupancy_of\n";
}

// Test 13: toggle_side_to_move works correctly
void test_toggle_side_to_move() {
    BoardState board;
    board.reset_to_start_position();

    assert(board.side_to_move == COLOR_WHITE);
    board.toggle_side_to_move();
    assert(board.side_to_move == COLOR_BLACK);
    board.toggle_side_to_move();
    assert(board.side_to_move == COLOR_WHITE);

    std::cout << "PASS: test_toggle_side_to_move\n";
}

// Test 14: Board validation - valid start position
void test_validation_start_position() {
    BoardState board;
    board.reset_to_start_position();

    assert(board.is_valid());

    std::cout << "PASS: test_validation_start_position\n";
}

// Test 15: Board validation - empty board is invalid (no kings)
void test_validation_empty() {
    BoardState board;  // Default is empty

    // Empty board has no kings, so it's invalid
    assert(!board.is_valid());

    std::cout << "PASS: test_validation_empty\n";
}

// Test 16: Board validation - overlapping pieces
void test_validation_overlapping() {
    BoardState board;
    board.reset_to_start_position();

    // Manually create overlap
    board.white_occupancy |= BB_SQUARE_E8;  // Make white occupy black's king square

    assert(!board.is_valid());

    std::cout << "PASS: test_validation_overlapping\n";
}

// Test 17: Board validation - inconsistent occupancy
void test_validation_inconsistent() {
    BoardState board;
    board.reset_to_start_position();

    // Manually break consistency
    board.all_occupancy = BB_EMPTY;

    assert(!board.is_valid());

    std::cout << "PASS: test_validation_inconsistent\n";
}

// Test 18: BoardState::to_string produces output
void test_to_string() {
    BoardState board;
    board.reset_to_start_position();

    std::string str = board.to_string();

    // Should contain rank numbers
    assert(str.find('8') != std::string::npos);
    assert(str.find('1') != std::string::npos);

    // Should contain file letters
    assert(str.find('a') != std::string::npos);
    assert(str.find('h') != std::string::npos);

    // Should contain piece characters
    assert(str.find('K') != std::string::npos);  // White king
    assert(str.find('k') != std::string::npos);  // Black king

    std::cout << "PASS: test_to_string\n";
}

// Test 19: Empty board piece_at returns NO_PIECE
void test_empty_board_piece_at() {
    BoardState board;  // Empty by default

    for (int sq = 0; sq < SQUARE_COUNT; ++sq) {
        assert(board.piece_at(static_cast<Square>(sq)) == PIECE_NO_PIECE);
    }

    std::cout << "PASS: test_empty_board_piece_at\n";
}

// Test 20: Manually set EP square validation
void test_ep_square_validation() {
    BoardState board;
    board.reset_to_start_position();

    // Valid EP square for white to move (rank 6, 0-indexed: rank 5)
    board.en_passant_square = SQUARE_E6;  // E6, rank 5 (0-indexed)
    assert(board.is_valid());

    // Invalid EP square for white to move (wrong rank)
    board.en_passant_square = SQUARE_E5;  // E5, rank 4
    assert(!board.is_valid());

    // Reset and test for black to move
    board.reset_to_start_position();
    board.toggle_side_to_move();

    // Valid EP square for black to move (rank 3, 0-indexed: rank 2)
    board.en_passant_square = SQUARE_E3;  // E3, rank 2
    assert(board.is_valid());

    // Invalid EP square for black to move (wrong rank)
    board.en_passant_square = SQUARE_E4;  // E4, rank 3
    assert(!board.is_valid());

    std::cout << "PASS: test_ep_square_validation\n";
}

int main() {
    std::cout << "=== BoardState Unit Tests ===\n\n";

    test_default_constructor();
    test_clear();
    test_start_position_pieces();
    test_start_position_occupancy();
    test_start_position_state();
    test_piece_at_start_position();
    test_piece_type_at();
    test_color_at();
    test_king_square();
    test_is_square_occupied();
    test_is_square_occupied_by();
    test_occupancy_of();
    test_toggle_side_to_move();
    test_validation_start_position();
    test_validation_empty();
    test_validation_overlapping();
    test_validation_inconsistent();
    test_to_string();
    test_empty_board_piece_at();
    test_ep_square_validation();

    std::cout << "\n=== All 20 tests passed! ===\n";
    return 0;
}
