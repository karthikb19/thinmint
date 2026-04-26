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

// ============ SPECIAL MOVES TESTS (S03-F02) ============

// Test kingside castling
void test_kingside_castling() {
    std::cout << "  Testing kingside castling... ";

    // White kingside castling
    BoardState board = board_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQK2R w KQkq - 0 1");

    // Verify initial state
    assert(has_piece_at(board, SQ_E1, COLOR_WHITE, PIECE_KING));
    assert(has_piece_at(board, SQ_H1, COLOR_WHITE, PIECE_ROOK));
    assert(has_castling_right(board.castling_rights, CASTLING_WHITE_KINGSIDE));

    // e1-g1 castling
    Move move = make_castling(SQ_E1, SQ_G1);
    make_move(board, move);

    // King moved to g1
    assert(!has_piece_at(board, SQ_E1, COLOR_WHITE, PIECE_KING));
    assert(has_piece_at(board, SQ_G1, COLOR_WHITE, PIECE_KING));

    // Rook moved to f1
    assert(!has_piece_at(board, SQ_H1, COLOR_WHITE, PIECE_ROOK));
    assert(has_piece_at(board, SQ_F1, COLOR_WHITE, PIECE_ROOK));

    // Castling rights lost for white
    assert(!has_castling_right(board.castling_rights, CASTLING_WHITE_KINGSIDE));
    assert(!has_castling_right(board.castling_rights, CASTLING_WHITE_QUEENSIDE));

    // Side to move changed
    assert(board.side_to_move == COLOR_BLACK);

    std::cout << "PASS" << std::endl;
}

// Test queenside castling
void test_queenside_castling() {
    std::cout << "  Testing queenside castling... ";

    // Black queenside castling
    BoardState board = board_from_fen("r3kbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1");

    // Verify initial state
    assert(has_piece_at(board, SQ_E8, COLOR_BLACK, PIECE_KING));
    assert(has_piece_at(board, SQ_A8, COLOR_BLACK, PIECE_ROOK));
    assert(has_castling_right(board.castling_rights, CASTLING_BLACK_QUEENSIDE));

    // e8-c8 castling
    Move move = make_castling(SQ_E8, SQ_C8);
    make_move(board, move);

    // King moved to c8
    assert(!has_piece_at(board, SQ_E8, COLOR_BLACK, PIECE_KING));
    assert(has_piece_at(board, SQ_C8, COLOR_BLACK, PIECE_KING));

    // Rook moved to d8
    assert(!has_piece_at(board, SQ_A8, COLOR_BLACK, PIECE_ROOK));
    assert(has_piece_at(board, SQ_D8, COLOR_BLACK, PIECE_ROOK));

    // Castling rights lost for black
    assert(!has_castling_right(board.castling_rights, CASTLING_BLACK_KINGSIDE));
    assert(!has_castling_right(board.castling_rights, CASTLING_BLACK_QUEENSIDE));

    // Side to move changed
    assert(board.side_to_move == COLOR_WHITE);

    std::cout << "PASS" << std::endl;
}

// Test castling rights update when king moves
void test_castling_rights_king_move() {
    std::cout << "  Testing castling rights on king move... ";

    // White king moves (not castling)
    BoardState board = board_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1");

    // Both castling rights available initially
    assert(has_castling_right(board.castling_rights, CASTLING_WHITE_KINGSIDE));
    assert(has_castling_right(board.castling_rights, CASTLING_WHITE_QUEENSIDE));

    // King moves to e2 (not castling)
    make_move(board, make_move(SQ_E1, SQ_E2));

    // Both castling rights lost
    assert(!has_castling_right(board.castling_rights, CASTLING_WHITE_KINGSIDE));
    assert(!has_castling_right(board.castling_rights, CASTLING_WHITE_QUEENSIDE));

    // Black castling rights unchanged
    assert(has_castling_right(board.castling_rights, CASTLING_BLACK_KINGSIDE));
    assert(has_castling_right(board.castling_rights, CASTLING_BLACK_QUEENSIDE));

    std::cout << "PASS" << std::endl;
}

// Test castling rights update when rook moves
void test_castling_rights_rook_move() {
    std::cout << "  Testing castling rights on rook move... ";

    // White h1 rook moves
    BoardState board = board_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1");

    // Both castling rights available initially
    assert(has_castling_right(board.castling_rights, CASTLING_WHITE_KINGSIDE));
    assert(has_castling_right(board.castling_rights, CASTLING_WHITE_QUEENSIDE));

    // h1 rook moves to h2
    make_move(board, make_move(SQ_H1, SQ_H2));

    // Only kingside rights lost
    assert(!has_castling_right(board.castling_rights, CASTLING_WHITE_KINGSIDE));
    assert(has_castling_right(board.castling_rights, CASTLING_WHITE_QUEENSIDE));

    // Reset and move a1 rook
    board = board_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1");
    make_move(board, make_move(SQ_A1, SQ_A2));

    // Only queenside rights lost
    assert(has_castling_right(board.castling_rights, CASTLING_WHITE_KINGSIDE));
    assert(!has_castling_right(board.castling_rights, CASTLING_WHITE_QUEENSIDE));

    std::cout << "PASS" << std::endl;
}

// Test castling rights update when rook is captured
void test_castling_rights_rook_capture() {
    std::cout << "  Testing castling rights on rook capture... ";

    // Test 1: White captures Black's h8 rook, Black loses kingside rights
    // FEN: Black has kingside rights (k), White has queenside rights (Q)
    BoardState board = board_from_fen("4k2r/8/8/8/8/8/8/R3K3 w kQ - 0 1");

    // Black kingside right available initially
    assert(has_castling_right(board.castling_rights, CASTLING_BLACK_KINGSIDE));

    // Rook captures h8 rook
    make_move(board, make_capture(SQ_A1, SQ_H8));

    // Black kingside rights lost
    assert(!has_castling_right(board.castling_rights, CASTLING_BLACK_KINGSIDE));

    // Test 2: Black captures White's a1 rook, White loses queenside rights
    board = board_from_fen("r3k3/8/8/8/8/8/8/R3K2R b KQ - 0 1");
    assert(has_castling_right(board.castling_rights, CASTLING_WHITE_KINGSIDE));
    assert(has_castling_right(board.castling_rights, CASTLING_WHITE_QUEENSIDE));

    // Black rook captures a1 rook
    make_move(board, make_capture(SQ_A8, SQ_A1));

    // White queenside rights lost (kingside still available)
    assert(has_castling_right(board.castling_rights, CASTLING_WHITE_KINGSIDE));
    assert(!has_castling_right(board.castling_rights, CASTLING_WHITE_QUEENSIDE));

    std::cout << "PASS" << std::endl;
}

// Test en passant capture
void test_en_passant_capture() {
    std::cout << "  Testing en passant capture... ";

    // White captures en passant
    BoardState board = board_from_fen("rnbqkbnr/pppppppp/8/4P3/8/8/PPPP1PPP/RNBQKBNR b KQkq - 0 1");

    // Black double pawn push d7-d5
    make_move(board, make_move(SQ_D7, SQ_D5));

    // EP square should be set
    assert(board.en_passant_square == SQ_D6);

    // Verify pawns are in position
    assert(has_piece_at(board, SQ_E5, COLOR_WHITE, PIECE_PAWN));
    assert(has_piece_at(board, SQ_D5, COLOR_BLACK, PIECE_PAWN));

    // White captures en passant: e5xd6
    Move ep_move = make_en_passant(SQ_E5, SQ_D6);
    make_move(board, ep_move);

    // White pawn moved to d6
    assert(!has_piece_at(board, SQ_E5, COLOR_WHITE, PIECE_PAWN));
    assert(has_piece_at(board, SQ_D6, COLOR_WHITE, PIECE_PAWN));

    // Black pawn on d5 was captured (removed)
    assert(!has_piece_at(board, SQ_D5, COLOR_BLACK, PIECE_PAWN));

    // EP square reset
    assert(board.en_passant_square == SQUARE_NONE);

    // Halfmove clock reset (capture)
    assert(board.halfmove_clock == 0);

    std::cout << "PASS" << std::endl;
}

// Test en passant capture by black
void test_en_passant_capture_black() {
    std::cout << "  Testing en passant capture by Black... ";

    // Black captures en passant
    // FEN: Black pawn on e4 (can capture g4 EP after g2-g4), White to move
    BoardState board = board_from_fen("rnbqkbnr/pppp1ppp/8/8/4p3/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    // White double pawn push g2-g4
    make_move(board, make_move(SQ_G2, SQ_G4));

    // EP square should be set
    assert(board.en_passant_square == SQ_G3);

    // Black captures en passant: e4xg3
    Move ep_move = make_en_passant(SQ_E4, SQ_G3);
    make_move(board, ep_move);

    // Black pawn moved to g3
    assert(!has_piece_at(board, SQ_E4, COLOR_BLACK, PIECE_PAWN));
    assert(has_piece_at(board, SQ_G3, COLOR_BLACK, PIECE_PAWN));

    // White pawn on g4 was captured (removed)
    assert(!has_piece_at(board, SQ_G4, COLOR_WHITE, PIECE_PAWN));

    std::cout << "PASS" << std::endl;
}

// Test promotion to queen
void test_promotion_queen() {
    std::cout << "  Testing promotion to queen... ";

    // White pawn promotion
    BoardState board = board_from_fen("8/P4k2/8/8/8/8/8/4K3 w - - 0 1");

    // Verify initial state
    assert(has_piece_at(board, SQ_A7, COLOR_WHITE, PIECE_PAWN));
    assert(!has_piece_at(board, SQ_A8, COLOR_WHITE, PIECE_QUEEN));

    // a7-a8=Q
    Move move = make_promotion(SQ_A7, SQ_A8, PIECE_QUEEN);
    make_move(board, move);

    // Pawn is gone
    assert(!has_piece_at(board, SQ_A7, COLOR_WHITE, PIECE_PAWN));

    // Queen is on a8
    assert(has_piece_at(board, SQ_A8, COLOR_WHITE, PIECE_QUEEN));

    // Halfmove clock reset (pawn move)
    assert(board.halfmove_clock == 0);

    std::cout << "PASS" << std::endl;
}

// Test promotion to knight
void test_promotion_knight() {
    std::cout << "  Testing promotion to knight... ";

    // Black pawn promotion
    BoardState board = board_from_fen("8/8/8/8/8/8/4k3/4K2n b - - 0 1");

    // h1-h2 with underpromotion to knight
    // Actually, let's do a regular promotion
    board = board_from_fen("4k3/4p3/8/8/8/8/8/4K3 b - - 0 1");
    // Need a pawn about to promote, let's set up properly
    board = board_from_fen("4k3/8/8/8/8/8/4p3/4K3 b - - 0 1");

    // e2-e1=N
    Move move = make_promotion(SQ_E2, SQ_E1, PIECE_KNIGHT);
    make_move(board, move);

    // Pawn is gone
    assert(!has_piece_at(board, SQ_E2, COLOR_BLACK, PIECE_PAWN));

    // Knight is on e1
    assert(has_piece_at(board, SQ_E1, COLOR_BLACK, PIECE_KNIGHT));

    std::cout << "PASS" << std::endl;
}

// Test promotion capture
void test_promotion_capture() {
    std::cout << "  Testing promotion capture... ";

    // White pawn captures and promotes
    BoardState board = board_from_fen("1r4k1/1P6/8/8/8/8/8/4K3 w - - 0 1");

    // Verify initial state
    assert(has_piece_at(board, SQ_B7, COLOR_WHITE, PIECE_PAWN));
    assert(has_piece_at(board, SQ_B8, COLOR_BLACK, PIECE_ROOK));

    // b7xa8=Q
    Move move = make_promotion(SQ_B7, SQ_A8, PIECE_QUEEN, true);
    make_move(board, move);

    // Pawn is gone
    assert(!has_piece_at(board, SQ_B7, COLOR_WHITE, PIECE_PAWN));

    // Queen is on a8
    assert(has_piece_at(board, SQ_A8, COLOR_WHITE, PIECE_QUEEN));

    // Black rook was captured
    assert(!has_piece_at(board, SQ_A8, COLOR_BLACK, PIECE_ROOK));

    // Halfmove clock reset (capture)
    assert(board.halfmove_clock == 0);

    std::cout << "PASS" << std::endl;
}

// Test board validity after special moves
void test_special_moves_validity() {
    std::cout << "  Testing board validity after special moves... ";

    BoardState board;

    // Castling validity - kingside
    board = board_from_fen("r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1");
    make_move(board, make_castling(SQ_E1, SQ_G1));
    assert(board.is_valid());

    // Castling validity - queenside
    board = board_from_fen("r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1");
    make_move(board, make_castling(SQ_E1, SQ_C1));
    assert(board.is_valid());

    // En passant validity - White captures Black's pawn
    // After Black d7-d5, White plays e5xd6 EP
    board = board_from_fen("rnbqkbnr/pppppppp/8/4P3/8/8/PPPP1PPP/RNBQKBNR b KQkq - 0 1");
    make_move(board, make_move(SQ_D7, SQ_D5));  // Black pushes d-pawn
    // Now White's turn, EP square is d6
    assert(board.en_passant_square == SQ_D6);
    make_move(board, make_en_passant(SQ_E5, SQ_D6));  // White captures EP
    assert(board.is_valid());

    // Promotion validity
    board = board_from_fen("8/P4k2/8/8/8/8/8/4K3 w - - 0 1");
    make_move(board, make_promotion(SQ_A7, SQ_A8, PIECE_QUEEN));
    assert(board.is_valid());

    std::cout << "PASS" << std::endl;
}

int main() {
    std::cout << "Running make_move tests..." << std::endl;

    // S03-F01: Basic moves
    test_quiet_pawn_push();
    test_quiet_piece_move();
    test_simple_capture();
    test_knight_capture();
    test_multiple_moves();
    test_occupancy_updates();
    test_major_piece_capture();
    test_halfmove_clock();
    test_board_validity();

    // S03-F02: Special moves
    test_kingside_castling();
    test_queenside_castling();
    test_castling_rights_king_move();
    test_castling_rights_rook_move();
    test_castling_rights_rook_capture();
    test_en_passant_capture();
    test_en_passant_capture_black();
    test_promotion_queen();
    test_promotion_knight();
    test_promotion_capture();
    test_special_moves_validity();

    std::cout << "All make_move tests passed!" << std::endl;
    return 0;
}
