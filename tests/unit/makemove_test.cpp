// Tests for make_move functionality
// Covers quiet moves and captures (S03-F01)

#include <cassert>
#include <iostream>
#include <string>

#include "thinmint/board/board.h"
#include "thinmint/board/makemove.h"
#include "thinmint/core/move.h"
#include "thinmint/core/piece.h"
#include "thinmint/core/square.h"
#include "thinmint/movegen/movegen.h"

using namespace thinmint::core;
using namespace thinmint::board;
using namespace thinmint::movegen;

// Short aliases for square constants for cleaner tests
constexpr Square SQ_A1 = SQUARE_A1, SQ_B1 = SQUARE_B1, SQ_C1 = SQUARE_C1;
constexpr Square SQ_D1 = SQUARE_D1, SQ_E1 = SQUARE_E1, SQ_F1 = SQUARE_F1;
constexpr Square SQ_G1 = SQUARE_G1, SQ_H1 = SQUARE_H1;
constexpr Square SQ_A2 = SQUARE_A2, SQ_B2 = SQUARE_B2, SQ_C2 = SQUARE_C2;
constexpr Square SQ_D2 = SQUARE_D2, SQ_E2 = SQUARE_E2, SQ_F2 = SQUARE_F2;
constexpr Square SQ_G2 = SQUARE_G2, SQ_H2 = SQUARE_H2;
constexpr Square SQ_A3 = SQUARE_A3, SQ_B3 = SQUARE_B3, SQ_C3 = SQUARE_C3;
constexpr Square SQ_D3 = SQUARE_D3, SQ_E3 = SQUARE_E3, SQ_F3 = SQUARE_F3;
constexpr Square SQ_G3 = SQUARE_G3, SQ_H3 = SQUARE_H3;
constexpr Square SQ_A4 = SQUARE_A4, SQ_B4 = SQUARE_B4, SQ_C4 = SQUARE_C4;
constexpr Square SQ_D4 = SQUARE_D4, SQ_E4 = SQUARE_E4, SQ_F4 = SQUARE_F4;
constexpr Square SQ_G4 = SQUARE_G4, SQ_H4 = SQUARE_H4;
constexpr Square SQ_A5 = SQUARE_A5, SQ_B5 = SQUARE_B5, SQ_C5 = SQUARE_C5;
constexpr Square SQ_D5 = SQUARE_D5, SQ_E5 = SQUARE_E5, SQ_F5 = SQUARE_F5;
constexpr Square SQ_G5 = SQUARE_G5, SQ_H5 = SQUARE_H5;
constexpr Square SQ_A6 = SQUARE_A6, SQ_B6 = SQUARE_B6, SQ_C6 = SQUARE_C6;
constexpr Square SQ_D6 = SQUARE_D6, SQ_E6 = SQUARE_E6, SQ_F6 = SQUARE_F6;
constexpr Square SQ_G6 = SQUARE_G6, SQ_H6 = SQUARE_H6;
constexpr Square SQ_A7 = SQUARE_A7, SQ_B7 = SQUARE_B7, SQ_C7 = SQUARE_C7;
constexpr Square SQ_D7 = SQUARE_D7, SQ_E7 = SQUARE_E7, SQ_F7 = SQUARE_F7;
constexpr Square SQ_G7 = SQUARE_G7, SQ_H7 = SQUARE_H7;
constexpr Square SQ_A8 = SQUARE_A8, SQ_B8 = SQUARE_B8, SQ_C8 = SQUARE_C8;
constexpr Square SQ_D8 = SQUARE_D8, SQ_E8 = SQUARE_E8, SQ_F8 = SQUARE_F8;
constexpr Square SQ_G8 = SQUARE_G8, SQ_H8 = SQUARE_H8;

// Helper to check if a square is occupied by a specific piece
bool has_piece_at(const BoardState& board, Square sq, Color c, PieceType pt) {
    return bb_test(board.pieces_of(c, pt), sq);
}

// Test quiet pawn push
void test_quiet_pawn_push() {
    std::cout << "  Testing quiet pawn push... ";

    BoardState board;
    board.reset_to_start_position();

    // e2-e4 pawn push
    Move move = make_move(SQ_E2, SQ_E4);
    make_move(board, move);

    // Check pawn moved
    assert(!has_piece_at(board, SQ_E2, COLOR_WHITE, PIECE_PAWN));
    assert(has_piece_at(board, SQ_E4, COLOR_WHITE, PIECE_PAWN));

    // Check side to move changed
    assert(board.side_to_move == COLOR_BLACK);

    // Check EP square was set
    assert(board.en_passant_square == SQ_E3);

    // Check halfmove clock was reset (pawn move)
    assert(board.halfmove_clock == 0);

    // Check fullmove number unchanged (White moved)
    assert(board.fullmove_number == 1);

    std::cout << "PASS" << std::endl;
}

// Test quiet piece move
void test_quiet_piece_move() {
    std::cout << "  Testing quiet piece move... ";

    // Knight move position
    BoardState board = board_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    // g1-f3 knight move
    Move move = make_move(SQ_G1, SQ_F3);
    make_move(board, move);

    // Check knight moved
    assert(!has_piece_at(board, SQ_G1, COLOR_WHITE, PIECE_KNIGHT));
    assert(has_piece_at(board, SQ_F3, COLOR_WHITE, PIECE_KNIGHT));

    // Check side to move changed
    assert(board.side_to_move == COLOR_BLACK);

    // Check EP square is none (not a double pawn push)
    assert(board.en_passant_square == SQUARE_NONE);

    // Check halfmove clock incremented
    assert(board.halfmove_clock == 1);

    std::cout << "PASS" << std::endl;
}

// Test simple capture
void test_simple_capture() {
    std::cout << "  Testing simple capture... ";

    // Position where white can capture
    BoardState board = board_from_fen("rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 1");

    // e4xd5 capture
    Move move = make_capture(SQ_E4, SQ_D5);
    make_move(board, move);

    // Check white pawn moved to d5
    assert(!has_piece_at(board, SQ_E4, COLOR_WHITE, PIECE_PAWN));
    assert(has_piece_at(board, SQ_D5, COLOR_WHITE, PIECE_PAWN));

    // Check black pawn was captured
    assert(!has_piece_at(board, SQ_D5, COLOR_BLACK, PIECE_PAWN));

    // Check halfmove clock reset (capture)
    assert(board.halfmove_clock == 0);

    std::cout << "PASS" << std::endl;
}

// Test knight capture
void test_knight_capture() {
    std::cout << "  Testing knight capture... ";

    // Position with knight capture opportunity
    BoardState board = board_from_fen("rnbqkbnr/pppp1ppp/8/4p3/4N3/8/PPPPPPPP/R1BQKBNR w KQkq - 0 1");

    // e4xf6 capture
    Move move = make_capture(SQ_E4, SQ_F6);
    make_move(board, move);

    // Check knight moved
    assert(!has_piece_at(board, SQ_E4, COLOR_WHITE, PIECE_KNIGHT));
    assert(has_piece_at(board, SQ_F6, COLOR_WHITE, PIECE_KNIGHT));

    // Check black knight was captured
    assert(!has_piece_at(board, SQ_F6, COLOR_BLACK, PIECE_KNIGHT));

    std::cout << "PASS" << std::endl;
}

// Test multiple moves to verify state progression
void test_multiple_moves() {
    std::cout << "  Testing multiple moves sequence... ";

    BoardState board;
    board.reset_to_start_position();

    // Move 1: e2-e4
    make_move(board, make_move(SQ_E2, SQ_E4));
    assert(board.side_to_move == COLOR_BLACK);
    assert(board.fullmove_number == 1);
    assert(board.en_passant_square == SQ_E3);

    // Move 2: e7-e5 (Black)
    make_move(board, make_move(SQ_E7, SQ_E5));
    assert(board.side_to_move == COLOR_WHITE);
    assert(board.fullmove_number == 2);  // Incremented after Black's move
    assert(board.en_passant_square == SQ_E6);

    // Move 3: g1-f3 (White)
    make_move(board, make_move(SQ_G1, SQ_F3));
    assert(board.side_to_move == COLOR_BLACK);
    assert(board.fullmove_number == 2);
    assert(board.en_passant_square == SQUARE_NONE);

    std::cout << "PASS" << std::endl;
}

// Test occupancy after moves
void test_occupancy_updates() {
    std::cout << "  Testing occupancy updates... ";

    BoardState board;
    board.reset_to_start_position();

    // Initial counts
    int initial_white = bb_popcount(board.white_occupancy);
    int initial_black = bb_popcount(board.black_occupancy);
    int initial_total = bb_popcount(board.all_occupancy);

    assert(initial_white == 16);
    assert(initial_black == 16);
    assert(initial_total == 32);

    // e2-e4
    make_move(board, make_move(SQ_E2, SQ_E4));

    // Counts should be unchanged
    assert(bb_popcount(board.white_occupancy) == 16);
    assert(bb_popcount(board.black_occupancy) == 16);
    assert(bb_popcount(board.all_occupancy) == 32);

    // e7-e5
    make_move(board, make_move(SQ_E7, SQ_E5));

    // Counts still unchanged
    assert(bb_popcount(board.white_occupancy) == 16);
    assert(bb_popcount(board.black_occupancy) == 16);
    assert(bb_popcount(board.all_occupancy) == 32);

    // Now test capture
    // Set up: e4xd5
    board = board_from_fen("rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 1");

    initial_white = bb_popcount(board.white_occupancy);
    initial_black = bb_popcount(board.black_occupancy);
    initial_total = bb_popcount(board.all_occupancy);

    assert(initial_white == 16);
    assert(initial_black == 16);
    assert(initial_total == 32);

    // Capture
    make_move(board, make_capture(SQ_E4, SQ_D5));

    // White count unchanged (still 16)
    assert(bb_popcount(board.white_occupancy) == 16);
    // Black lost a pawn (15)
    assert(bb_popcount(board.black_occupancy) == 15);
    // Total decreased by 1
    assert(bb_popcount(board.all_occupancy) == 31);

    std::cout << "PASS" << std::endl;
}

// Test capture of major piece
void test_major_piece_capture() {
    std::cout << "  Testing major piece capture... ";

    // Position: White queen on d4, Black queen on d8
    BoardState board = board_from_fen("rnbqkbnr/pppppppp/8/8/3Q4/8/PPPPPPPP/RNB1KBNR b KQkq - 0 1");

    // Check queens are there
    assert(has_piece_at(board, SQ_D4, COLOR_WHITE, PIECE_QUEEN));
    assert(has_piece_at(board, SQ_D8, COLOR_BLACK, PIECE_QUEEN));

    // d8xd4 capture (Black queen takes White queen)
    Move move = make_capture(SQ_D8, SQ_D4);
    make_move(board, move);

    // Black queen moved
    assert(!has_piece_at(board, SQ_D8, COLOR_BLACK, PIECE_QUEEN));
    assert(has_piece_at(board, SQ_D4, COLOR_BLACK, PIECE_QUEEN));

    // White queen was captured
    assert(!has_piece_at(board, SQ_D4, COLOR_WHITE, PIECE_QUEEN));

    std::cout << "PASS" << std::endl;
}

// Test halfmove clock on various moves
void test_halfmove_clock() {
    std::cout << "  Testing halfmove clock... ";

    // Start with some halfmove clock value
    BoardState board = board_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 10 1");
    assert(board.halfmove_clock == 10);

    // Quiet piece move: increment clock
    make_move(board, make_move(SQ_G1, SQ_F3));
    assert(board.halfmove_clock == 11);

    // Make a few more quiet moves
    make_move(board, make_move(SQ_G8, SQ_F6));  // Black knight
    assert(board.halfmove_clock == 12);

    make_move(board, make_move(SQ_F3, SQ_G1));  // White knight back
    assert(board.halfmove_clock == 13);

    // Now a pawn move: reset clock
    make_move(board, make_move(SQ_E7, SQ_E5));
    assert(board.halfmove_clock == 0);

    // Another quiet move
    make_move(board, make_move(SQ_G1, SQ_F3));
    assert(board.halfmove_clock == 1);

    // Capture: reset clock
    board = board_from_fen("rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 50 1");
    make_move(board, make_capture(SQ_E4, SQ_D5));
    assert(board.halfmove_clock == 0);

    std::cout << "PASS" << std::endl;
}

// Test that board remains valid after moves
void test_board_validity() {
    std::cout << "  Testing board validity after moves... ";

    BoardState board;
    board.reset_to_start_position();

    // Make several moves and check validity
    make_move(board, make_move(SQ_E2, SQ_E4));
    assert(board.is_valid());

    make_move(board, make_move(SQ_E7, SQ_E5));
    assert(board.is_valid());

    make_move(board, make_move(SQ_G1, SQ_F3));
    assert(board.is_valid());

    make_move(board, make_move(SQ_B8, SQ_C6));
    assert(board.is_valid());

    // Test with captures
    board = board_from_fen("rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 1");
    make_move(board, make_capture(SQ_E4, SQ_D5));
    assert(board.is_valid());

    std::cout << "PASS" << std::endl;
}

int main() {
    std::cout << "Running make_move tests..." << std::endl;

    test_quiet_pawn_push();
    test_quiet_piece_move();
    test_simple_capture();
    test_knight_capture();
    test_multiple_moves();
    test_occupancy_updates();
    test_major_piece_capture();
    test_halfmove_clock();
    test_board_validity();

    std::cout << "All make_move tests passed!" << std::endl;
    return 0;
}
