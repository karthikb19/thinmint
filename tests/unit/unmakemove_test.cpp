// Tests for unmake_move functionality
// Covers S03-F03: Implement unmake move with undo state

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include "thinmint/board/board.h"
#include "thinmint/board/makemove.h"
#include "thinmint/board/unmakemove.h"
#include "thinmint/core/move.h"
#include "thinmint/core/piece.h"
#include "thinmint/core/square.h"

using namespace thinmint::core;
using namespace thinmint::board;

// Short aliases for square constants
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

// Helper to compare two board states for equality with debug output
bool boards_equal_debug(const BoardState& a, const BoardState& b, const char*& reason) {
    // Compare piece bitboards
    for (size_t c = 0; c < COLOR_COUNT; ++c) {
        for (size_t pt = 0; pt < PIECE_TYPE_COUNT; ++pt) {
            if (a.pieces[c][pt] != b.pieces[c][pt]) {
                reason = "piece bitboard differs";
                return false;
            }
        }
    }
    // Compare occupancy
    if (a.white_occupancy != b.white_occupancy) {
        reason = "white_occupancy differs";
        return false;
    }
    if (a.black_occupancy != b.black_occupancy) {
        reason = "black_occupancy differs";
        return false;
    }
    if (a.all_occupancy != b.all_occupancy) {
        reason = "all_occupancy differs";
        return false;
    }
    // Compare game state
    if (a.side_to_move != b.side_to_move) {
        reason = "side_to_move differs";
        return false;
    }
    if (a.castling_rights != b.castling_rights) {
        reason = "castling_rights differs";
        return false;
    }
    if (a.en_passant_square != b.en_passant_square) {
        reason = "en_passant_square differs";
        return false;
    }
    if (a.halfmove_clock != b.halfmove_clock) {
        reason = "halfmove_clock differs";
        return false;
    }
    if (a.fullmove_number != b.fullmove_number) {
        reason = "fullmove_number differs";
        return false;
    }
    return true;
}

// Helper to compare two board states for equality
bool boards_equal(const BoardState& a, const BoardState& b) {
    const char* reason = nullptr;
    return boards_equal_debug(a, b, reason);
}

// Test undo stack basic operations
void test_undo_stack() {
    std::cout << "  Testing UndoStack basic operations... ";

    UndoStack stack;
    assert(stack.empty());
    assert(stack.size() == 0);

    UndoState state1;
    state1.move = make_move(SQ_E2, SQ_E4);
    state1.castling_rights = CASTLING_WHITE_KINGSIDE;
    state1.halfmove_clock = 5;

    stack.push(state1);
    assert(!stack.empty());
    assert(stack.size() == 1);
    assert(stack.top().move == state1.move);

    UndoState state2;
    state2.move = make_move(SQ_E7, SQ_E5);
    state2.halfmove_clock = 0;

    stack.push(state2);
    assert(stack.size() == 2);

    UndoState popped = stack.pop();
    assert(popped.move == state2.move);
    assert(stack.size() == 1);

    stack.clear();
    assert(stack.empty());
    assert(stack.size() == 0);

    std::cout << "PASS" << std::endl;
}

// Test unmake of a simple quiet pawn push
void test_unmake_quiet_pawn_push() {
    std::cout << "  Testing unmake quiet pawn push... ";

    BoardState original = board_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    BoardState board = original;

    Move move = make_move(SQ_E2, SQ_E4);

    // Create undo state
    UndoState undo;
    undo.move = move;
    undo.castling_rights = board.castling_rights;
    undo.en_passant_square = board.en_passant_square;
    undo.halfmove_clock = board.halfmove_clock;
    undo.captured_piece_type = PIECE_NONE;

    // Make and unmake
    make_move(board, move);
    assert(!boards_equal(board, original));  // Board should be different
    unmake_move(board, undo);

    // Should be back to original
    assert(boards_equal(board, original));
    assert(board.is_valid());

    std::cout << "PASS" << std::endl;
}

// Test unmake of a quiet piece move
void test_unmake_quiet_piece_move() {
    std::cout << "  Testing unmake quiet piece move... ";

    BoardState original = board_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    BoardState board = original;

    Move move = make_move(SQ_G1, SQ_F3);

    UndoState undo;
    undo.move = move;
    undo.castling_rights = board.castling_rights;
    undo.en_passant_square = board.en_passant_square;
    undo.halfmove_clock = board.halfmove_clock;
    undo.captured_piece_type = PIECE_NONE;

    make_move(board, move);
    unmake_move(board, undo);

    assert(boards_equal(board, original));
    assert(board.is_valid());

    std::cout << "PASS" << std::endl;
}

// Test unmake of a capture
void test_unmake_capture() {
    std::cout << "  Testing unmake capture... ";

    BoardState original = board_from_fen("rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 1");
    BoardState board = original;

    Move move = make_capture(SQ_E4, SQ_D5);

    UndoState undo;
    undo.move = move;
    undo.castling_rights = board.castling_rights;
    undo.en_passant_square = board.en_passant_square;
    undo.halfmove_clock = board.halfmove_clock;
    undo.captured_piece_type = PIECE_PAWN;  // Captured black pawn

    make_move(board, move);

    // Verify capture happened
    assert(has_piece_at(board, SQ_D5, COLOR_WHITE, PIECE_PAWN));
    assert(!has_piece_at(board, SQ_D5, COLOR_BLACK, PIECE_PAWN));

    unmake_move(board, undo);

    assert(boards_equal(board, original));
    assert(board.is_valid());

    // Verify piece restored
    assert(has_piece_at(board, SQ_E4, COLOR_WHITE, PIECE_PAWN));
    assert(has_piece_at(board, SQ_D5, COLOR_BLACK, PIECE_PAWN));

    std::cout << "PASS" << std::endl;
}

// Test unmake of kingside castling
void test_unmake_kingside_castling() {
    std::cout << "  Testing unmake kingside castling... ";

    BoardState original = board_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQK2R w KQkq - 0 1");
    BoardState board = original;

    Move move = make_castling(SQ_E1, SQ_G1);

    UndoState undo;
    undo.move = move;
    undo.castling_rights = board.castling_rights;
    undo.en_passant_square = board.en_passant_square;
    undo.halfmove_clock = board.halfmove_clock;
    undo.captured_piece_type = PIECE_NONE;

    make_move(board, move);

    // Verify castling happened
    assert(has_piece_at(board, SQ_G1, COLOR_WHITE, PIECE_KING));
    assert(has_piece_at(board, SQ_F1, COLOR_WHITE, PIECE_ROOK));
    assert(!has_piece_at(board, SQ_E1, COLOR_WHITE, PIECE_KING));
    assert(!has_piece_at(board, SQ_H1, COLOR_WHITE, PIECE_ROOK));

    unmake_move(board, undo);

    assert(boards_equal(board, original));
    assert(board.is_valid());

    // Verify pieces restored
    assert(has_piece_at(board, SQ_E1, COLOR_WHITE, PIECE_KING));
    assert(has_piece_at(board, SQ_H1, COLOR_WHITE, PIECE_ROOK));

    std::cout << "PASS" << std::endl;
}

// Test unmake of queenside castling
void test_unmake_queenside_castling() {
    std::cout << "  Testing unmake queenside castling... ";

    BoardState original = board_from_fen("r3kbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1");
    BoardState board = original;

    Move move = make_castling(SQ_E8, SQ_C8);

    UndoState undo;
    undo.move = move;
    undo.castling_rights = board.castling_rights;
    undo.en_passant_square = board.en_passant_square;
    undo.halfmove_clock = board.halfmove_clock;
    undo.captured_piece_type = PIECE_NONE;

    make_move(board, move);

    // Verify castling happened
    assert(has_piece_at(board, SQ_C8, COLOR_BLACK, PIECE_KING));
    assert(has_piece_at(board, SQ_D8, COLOR_BLACK, PIECE_ROOK));

    unmake_move(board, undo);

    assert(boards_equal(board, original));
    assert(board.is_valid());

    std::cout << "PASS" << std::endl;
}

// Test unmake of en passant capture (white capturing black)
void test_unmake_en_passant_white() {
    std::cout << "  Testing unmake en passant (White)... ";

    // Position after Black d7-d5, White to move, EP on d6
    BoardState original = board_from_fen("rnbqkbnr/ppp1pppp/8/3pP3/8/8/PPPP1PPP/RNBQKBNR w KQkq d6 0 2");
    BoardState board = original;

    Move move = make_en_passant(SQ_E5, SQ_D6);

    UndoState undo;
    undo.move = move;
    undo.castling_rights = board.castling_rights;
    undo.en_passant_square = board.en_passant_square;
    undo.halfmove_clock = board.halfmove_clock;
    undo.captured_piece_type = PIECE_PAWN;

    make_move(board, move);

    // Verify EP happened
    assert(has_piece_at(board, SQ_D6, COLOR_WHITE, PIECE_PAWN));
    assert(!has_piece_at(board, SQ_E5, COLOR_WHITE, PIECE_PAWN));
    assert(!has_piece_at(board, SQ_D5, COLOR_BLACK, PIECE_PAWN));  // Captured

    unmake_move(board, undo);

    assert(boards_equal(board, original));
    assert(board.is_valid());

    // Verify pieces restored
    assert(has_piece_at(board, SQ_E5, COLOR_WHITE, PIECE_PAWN));
    assert(has_piece_at(board, SQ_D5, COLOR_BLACK, PIECE_PAWN));
    assert(!has_piece_at(board, SQ_D6, COLOR_WHITE, PIECE_PAWN));

    std::cout << "PASS" << std::endl;
}

// Test unmake of en passant capture (black capturing white)
void test_unmake_en_passant_black() {
    std::cout << "  Testing unmake en passant (Black)... ";

    // Position after White g2-g4, Black to move, EP on g3
    BoardState original = board_from_fen("rnbqkbnr/pppp1ppp/8/8/6Pp/8/PPPPPP1P/RNBQKBNR b KQkq g3 0 1");
    BoardState board = original;

    Move move = make_en_passant(SQ_H4, SQ_G3);

    UndoState undo;
    undo.move = move;
    undo.castling_rights = board.castling_rights;
    undo.en_passant_square = board.en_passant_square;
    undo.halfmove_clock = board.halfmove_clock;
    undo.captured_piece_type = PIECE_PAWN;

    make_move(board, move);

    // Verify EP happened
    assert(has_piece_at(board, SQ_G3, COLOR_BLACK, PIECE_PAWN));
    assert(!has_piece_at(board, SQ_G4, COLOR_WHITE, PIECE_PAWN));  // Captured

    unmake_move(board, undo);

    assert(boards_equal(board, original));
    assert(board.is_valid());

    std::cout << "PASS" << std::endl;
}

// Test unmake of promotion
void test_unmake_promotion() {
    std::cout << "  Testing unmake promotion... ";

    BoardState original = board_from_fen("8/P4k2/8/8/8/8/8/4K3 w - - 0 1");
    BoardState board = original;

    Move move = make_promotion(SQ_A7, SQ_A8, PIECE_QUEEN);

    UndoState undo;
    undo.move = move;
    undo.castling_rights = board.castling_rights;
    undo.en_passant_square = board.en_passant_square;
    undo.halfmove_clock = board.halfmove_clock;
    undo.captured_piece_type = PIECE_NONE;

    make_move(board, move);

    // Verify promotion happened
    assert(!has_piece_at(board, SQ_A7, COLOR_WHITE, PIECE_PAWN));
    assert(has_piece_at(board, SQ_A8, COLOR_WHITE, PIECE_QUEEN));

    unmake_move(board, undo);

    assert(boards_equal(board, original));
    assert(board.is_valid());

    // Verify pawn restored
    assert(has_piece_at(board, SQ_A7, COLOR_WHITE, PIECE_PAWN));
    assert(!has_piece_at(board, SQ_A8, COLOR_WHITE, PIECE_QUEEN));

    std::cout << "PASS" << std::endl;
}

// Test unmake of promotion capture
void test_unmake_promotion_capture() {
    std::cout << "  Testing unmake promotion capture... ";

    // Black rook on a8, White pawn on b7 about to capture and promote
    BoardState original = board_from_fen("r5k1/1P6/8/8/8/8/8/4K3 w - - 0 1");
    BoardState board = original;

    Move move = make_promotion(SQ_B7, SQ_A8, PIECE_QUEEN, true);

    UndoState undo;
    undo.move = move;
    undo.castling_rights = board.castling_rights;
    undo.en_passant_square = board.en_passant_square;
    undo.halfmove_clock = board.halfmove_clock;
    undo.captured_piece_type = PIECE_ROOK;

    make_move(board, move);

    // Verify promotion capture happened
    assert(!has_piece_at(board, SQ_B7, COLOR_WHITE, PIECE_PAWN));
    assert(has_piece_at(board, SQ_A8, COLOR_WHITE, PIECE_QUEEN));
    assert(!has_piece_at(board, SQ_A8, COLOR_BLACK, PIECE_ROOK));  // Captured

    unmake_move(board, undo);

    assert(boards_equal(board, original));
    assert(board.is_valid());

    // Verify pieces restored - pawn at b7, rook at a8
    assert(has_piece_at(board, SQ_B7, COLOR_WHITE, PIECE_PAWN));
    assert(has_piece_at(board, SQ_A8, COLOR_BLACK, PIECE_ROOK));
    assert(!has_piece_at(board, SQ_A8, COLOR_WHITE, PIECE_QUEEN));

    std::cout << "PASS" << std::endl;
}

// Test unmake preserves castling rights correctly
void test_unmake_castling_rights() {
    std::cout << "  Testing unmake preserves castling rights... ";

    // Start with full castling rights - use a clean position with king and rooks on back rank
    // King on e1, rook on h1, empty squares elsewhere on back rank
    BoardState original = board_from_fen("r3k2r/pppppppp/8/8/8/8/8/4K2R w KQkq - 0 1");
    BoardState board = original;

    // Verify king is at e1 before move
    assert(board.piece_type_at(SQ_E1) == PIECE_KING);
    assert(board.king_square(COLOR_WHITE) == SQ_E1);

    // Move white king e1-f1 - should lose castling rights (f1 is empty)
    Move move = make_move(SQ_E1, SQ_F1);

    UndoState undo;
    undo.move = move;
    undo.castling_rights = board.castling_rights;  // Save original rights
    undo.en_passant_square = board.en_passant_square;
    undo.halfmove_clock = board.halfmove_clock;
    undo.captured_piece_type = PIECE_NONE;

    CastlingRights original_rights = board.castling_rights;
    assert(has_castling_right(original_rights, CASTLING_WHITE_KINGSIDE));
    assert(has_castling_right(original_rights, CASTLING_WHITE_QUEENSIDE));

    make_move(board, move);

    // After make_move, it's Black's turn - the king should be at f1
    assert(board.piece_type_at(SQ_F1) == PIECE_KING);
    assert(board.piece_type_at(SQ_E1) == PIECE_NONE);  // e1 should be empty

    // Castling rights should be lost after king move
    assert(!has_castling_right(board.castling_rights, CASTLING_WHITE_KINGSIDE));
    assert(!has_castling_right(board.castling_rights, CASTLING_WHITE_QUEENSIDE));

    unmake_move(board, undo);

    // Rights should be restored
    assert(has_castling_right(board.castling_rights, CASTLING_WHITE_KINGSIDE));
    assert(has_castling_right(board.castling_rights, CASTLING_WHITE_QUEENSIDE));
    assert(boards_equal(board, original));

    std::cout << "PASS" << std::endl;
}

// Test unmake preserves en passant square correctly
void test_unmake_ep_square() {
    std::cout << "  Testing unmake preserves EP square... ";

    // Position with EP square set
    BoardState original = board_from_fen("rnbqkbnr/pppppppp/8/8/4Pp2/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");
    BoardState board = original;

    assert(board.en_passant_square == SQ_E3);

    // Make a move that doesn't capture EP
    Move move = make_move(SQ_G8, SQ_F6);

    UndoState undo;
    undo.move = move;
    undo.castling_rights = board.castling_rights;
    undo.en_passant_square = board.en_passant_square;  // Save EP square
    undo.halfmove_clock = board.halfmove_clock;
    undo.captured_piece_type = PIECE_NONE;

    make_move(board, move);

    // EP square should be reset after this move
    assert(board.en_passant_square == SQUARE_NONE);

    unmake_move(board, undo);

    // EP square should be restored
    assert(board.en_passant_square == SQ_E3);
    assert(boards_equal(board, original));

    std::cout << "PASS" << std::endl;
}

// Test unmake preserves halfmove clock correctly
void test_unmake_halfmove_clock() {
    std::cout << "  Testing unmake preserves halfmove clock... ";

    // Position with non-zero halfmove clock
    BoardState original = board_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 10 1");
    BoardState board = original;

    assert(board.halfmove_clock == 10);

    // Pawn move resets clock
    Move move = make_move(SQ_E2, SQ_E4);

    UndoState undo;
    undo.move = move;
    undo.castling_rights = board.castling_rights;
    undo.en_passant_square = board.en_passant_square;
    undo.halfmove_clock = 10;  // Original value
    undo.captured_piece_type = PIECE_NONE;

    make_move(board, move);

    // Clock should be reset
    assert(board.halfmove_clock == 0);

    unmake_move(board, undo);

    // Clock should be restored
    assert(board.halfmove_clock == 10);
    assert(boards_equal(board, original));

    std::cout << "PASS" << std::endl;
}

// Test unmake preserves fullmove number correctly
void test_unmake_fullmove_number() {
    std::cout << "  Testing unmake preserves fullmove number... ";

    // Black to move at move 5
    BoardState original = board_from_fen("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq - 0 5");
    BoardState board = original;

    assert(board.fullmove_number == 5);

    Move move = make_move(SQ_E7, SQ_E5);

    UndoState undo;
    undo.move = move;
    undo.castling_rights = board.castling_rights;
    undo.en_passant_square = board.en_passant_square;
    undo.halfmove_clock = board.halfmove_clock;
    undo.captured_piece_type = PIECE_NONE;

    make_move(board, move);

    // After Black moves, fullmove should increment
    assert(board.fullmove_number == 6);

    unmake_move(board, undo);

    // Should be restored
    assert(board.fullmove_number == 5);
    assert(boards_equal(board, original));

    std::cout << "PASS" << std::endl;
}

// Test multiple sequential make/unmake operations
void test_sequential_make_unmake() {
    std::cout << "  Testing sequential make/unmake... ";

    BoardState original = board_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    BoardState board = original;
    std::vector<UndoState> undos;

    // Make several moves
    std::vector<Move> moves = {
        make_move(SQ_E2, SQ_E4),
        make_move(SQ_E7, SQ_E5),
        make_move(SQ_G1, SQ_F3),
        make_move(SQ_B8, SQ_C6)
    };

    // Make all moves, saving undo states
    for (const auto& move : moves) {
        UndoState undo;
        undo.move = move;
        undo.castling_rights = board.castling_rights;
        undo.en_passant_square = board.en_passant_square;
        undo.halfmove_clock = board.halfmove_clock;
        undo.captured_piece_type = PIECE_NONE;
        undos.push_back(undo);

        make_move(board, move);
        assert(board.is_valid());
    }

    // Unmake all moves in reverse order
    for (auto it = undos.rbegin(); it != undos.rend(); ++it) {
        unmake_move(board, *it);
        assert(board.is_valid());
    }

    // Should be back to original
    assert(boards_equal(board, original));

    std::cout << "PASS" << std::endl;
}

// Test unmake of knight underpromotion
void test_unmake_underpromotion() {
    std::cout << "  Testing unmake underpromotion... ";

    // Black pawn on e2, promoting to e1=N, White king on f1 (not on e1)
    BoardState original = board_from_fen("4k3/8/8/8/8/8/4p3/5K2 b - - 0 1");
    BoardState board = original;

    Move move = make_promotion(SQ_E2, SQ_E1, PIECE_KNIGHT);

    UndoState undo;
    undo.move = move;
    undo.castling_rights = board.castling_rights;
    undo.en_passant_square = board.en_passant_square;
    undo.halfmove_clock = board.halfmove_clock;
    undo.captured_piece_type = PIECE_NONE;

    make_move(board, move);

    // Verify knight promotion
    assert(has_piece_at(board, SQ_E1, COLOR_BLACK, PIECE_KNIGHT));
    assert(!has_piece_at(board, SQ_E2, COLOR_BLACK, PIECE_PAWN));

    unmake_move(board, undo);

    assert(boards_equal(board, original));
    assert(has_piece_at(board, SQ_E2, COLOR_BLACK, PIECE_PAWN));
    assert(!has_piece_at(board, SQ_E1, COLOR_BLACK, PIECE_KNIGHT));

    std::cout << "PASS" << std::endl;
}

// Test unmake of rook capture
void test_unmake_rook_capture() {
    std::cout << "  Testing unmake rook capture... ";

    BoardState original = board_from_fen("r3k3/8/8/8/8/8/8/R3K3 w Qq - 0 1");
    BoardState board = original;

    Move move = make_capture(SQ_A1, SQ_A8);

    UndoState undo;
    undo.move = move;
    undo.castling_rights = board.castling_rights;
    undo.en_passant_square = board.en_passant_square;
    undo.halfmove_clock = board.halfmove_clock;
    undo.captured_piece_type = PIECE_ROOK;

    make_move(board, move);

    // Verify capture
    assert(has_piece_at(board, SQ_A8, COLOR_WHITE, PIECE_ROOK));
    assert(!has_piece_at(board, SQ_A8, COLOR_BLACK, PIECE_ROOK));

    unmake_move(board, undo);

    assert(boards_equal(board, original));
    assert(has_piece_at(board, SQ_A1, COLOR_WHITE, PIECE_ROOK));
    assert(has_piece_at(board, SQ_A8, COLOR_BLACK, PIECE_ROOK));

    std::cout << "PASS" << std::endl;
}

int main() {
    std::cout << "Running unmake_move tests..." << std::endl;

    test_undo_stack();
    test_unmake_quiet_pawn_push();
    test_unmake_quiet_piece_move();
    test_unmake_capture();
    test_unmake_kingside_castling();
    test_unmake_queenside_castling();
    test_unmake_en_passant_white();
    test_unmake_en_passant_black();
    test_unmake_promotion();
    test_unmake_promotion_capture();
    test_unmake_castling_rights();
    test_unmake_ep_square();
    test_unmake_halfmove_clock();
    test_unmake_fullmove_number();
    test_sequential_make_unmake();
    test_unmake_underpromotion();
    test_unmake_rook_capture();

    std::cout << "All unmake_move tests passed!" << std::endl;
    return 0;
}
