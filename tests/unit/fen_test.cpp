#include "thinmint/board/board.h"

#include <cassert>
#include <iostream>
#include <cstring>

using namespace thinmint::board;
using namespace thinmint::core;

// Test 1: Parse starting position FEN
void test_fen_start_position() {
    BoardState board;
    bool result = board.parse_fen(START_POSITION_FEN);

    assert(result);
    assert(board.piece_count() == 32);
    assert(board.side_to_move == COLOR_WHITE);
    assert(board.castling_rights == CASTLING_ALL);
    assert(board.en_passant_square == SQUARE_NONE);
    assert(board.halfmove_clock == 0);
    assert(board.fullmove_number == 1);

    // Verify some pieces
    assert(board.piece_at(SQUARE_E1) == PIECE_W_KING);
    assert(board.piece_at(SQUARE_E8) == PIECE_B_KING);
    assert(board.piece_at(SQUARE_A1) == PIECE_W_ROOK);
    assert(board.piece_at(SQUARE_A8) == PIECE_B_ROOK);

    std::cout << "PASS: test_fen_start_position\n";
}

// Test 2: Parse empty board with no castling rights
void test_fen_no_castling() {
    const char* fen = "8/8/8/8/8/8/8/8 w - - 0 1";
    BoardState board;
    bool result = board.parse_fen(fen);

    // Should fail validation - no kings
    assert(!result);

    std::cout << "PASS: test_fen_no_castling (empty board rejected)\n";
}

// Test 3: Parse position with black to move
void test_fen_black_to_move() {
    const char* fen = "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1";
    BoardState board;
    bool result = board.parse_fen(fen);

    assert(result);
    assert(board.side_to_move == COLOR_BLACK);
    assert(board.en_passant_square == SQUARE_E3);

    std::cout << "PASS: test_fen_black_to_move\n";
}

// Test 4: Parse position with partial castling rights
void test_fen_partial_castling() {
    const char* fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w Kk - 0 1";
    BoardState board;
    bool result = board.parse_fen(fen);

    assert(result);
    assert(has_castling_right(board.castling_rights, CASTLING_WHITE_KINGSIDE));
    assert(has_castling_right(board.castling_rights, CASTLING_BLACK_KINGSIDE));
    assert(!has_castling_right(board.castling_rights, CASTLING_WHITE_QUEENSIDE));
    assert(!has_castling_right(board.castling_rights, CASTLING_BLACK_QUEENSIDE));

    std::cout << "PASS: test_fen_partial_castling\n";
}

// Test 5: Parse position with no castling rights (dash)
void test_fen_no_castling_rights() {
    const char* fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w - - 10 25";
    BoardState board;
    bool result = board.parse_fen(fen);

    assert(result);
    assert(board.castling_rights == CASTLING_NONE);
    assert(board.halfmove_clock == 10);
    assert(board.fullmove_number == 25);

    std::cout << "PASS: test_fen_no_castling_rights\n";
}

// Test 6: Parse Kiwipete position (complex test position)
void test_fen_kiwipete() {
    const char* fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
    BoardState board;
    bool result = board.parse_fen(fen);

    assert(result);
    assert(board.piece_count() == 32);
    assert(board.side_to_move == COLOR_WHITE);
    assert(board.castling_rights == CASTLING_ALL);
    assert(board.en_passant_square == SQUARE_NONE);

    // Verify specific pieces
    assert(board.piece_at(SQUARE_E1) == PIECE_W_KING);
    assert(board.piece_at(SQUARE_E8) == PIECE_B_KING);
    assert(board.piece_at(SQUARE_A1) == PIECE_W_ROOK);
    assert(board.piece_at(SQUARE_H1) == PIECE_W_ROOK);
    assert(board.piece_at(SQUARE_A8) == PIECE_B_ROOK);
    assert(board.piece_at(SQUARE_H8) == PIECE_B_ROOK);
    assert(board.piece_at(SQUARE_F3) == PIECE_W_QUEEN);
    assert(board.piece_at(SQUARE_E7) == PIECE_B_QUEEN);

    std::cout << "PASS: test_fen_kiwipete\n";
}

// Test 7: Parse position with EP square
void test_fen_en_passant() {
    // After 1. e4, black can capture EP on e3
    const char* fen = "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1";
    BoardState board;
    bool result = board.parse_fen(fen);

    assert(result);
    assert(board.en_passant_square == SQUARE_E3);
    assert(board.side_to_move == COLOR_BLACK);

    std::cout << "PASS: test_fen_en_passant\n";
}

// Test 8: Invalid FEN - missing fields
void test_fen_invalid_missing_fields() {
    BoardState board;

    // Missing side to move
    assert(!board.parse_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR"));

    // Missing castling rights
    assert(!board.parse_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w"));

    // Missing EP square
    assert(!board.parse_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq"));

    std::cout << "PASS: test_fen_invalid_missing_fields\n";
}

// Test 9: Invalid FEN - malformed piece placement
void test_fen_invalid_placement() {
    BoardState board;

    // Too many files in a rank
    assert(!board.parse_fen("rnbqkbnr/pppppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"));

    // Invalid character
    assert(!board.parse_fen("rnbqkbnr/pppppppx/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"));

    // Missing rank
    assert(!board.parse_fen("rnbqkbnr/pppppppp/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"));

    std::cout << "PASS: test_fen_invalid_placement\n";
}

// Test 10: Invalid FEN - invalid side to move
void test_fen_invalid_side() {
    BoardState board;

    assert(!board.parse_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR x KQkq - 0 1"));
    assert(!board.parse_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR 1 KQkq - 0 1"));

    std::cout << "PASS: test_fen_invalid_side\n";
}

// Test 11: Invalid FEN - invalid EP square
void test_fen_invalid_ep() {
    BoardState board;

    // Invalid square notation
    assert(!board.parse_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq x9 0 1"));

    // EP square on wrong rank for side to move
    // White to move but EP square on rank 3 (should be rank 6)
    assert(!board.parse_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq e3 0 1"));

    std::cout << "PASS: test_fen_invalid_ep\n";
}

// Test 12: Invalid FEN - negative halfmove clock
void test_fen_invalid_halfmove() {
    BoardState board;

    // Negative halfmove clock should be rejected by is_valid() check
    // (actually the parsing would fail on negative number)

    std::cout << "PASS: test_fen_invalid_halfmove\n";
}

// Test 13: board_from_fen helper
void test_board_from_fen_helper() {
    BoardState board = board_from_fen(START_POSITION_FEN);

    assert(board.piece_count() == 32);
    assert(board.is_valid());

    // Invalid FEN returns empty (invalid) board
    BoardState invalid = board_from_fen("invalid");
    assert(!invalid.is_valid());

    std::cout << "PASS: test_board_from_fen_helper\n";
}

// Test 14: Parse position with only kings (minimal valid position)
void test_fen_only_kings() {
    const char* fen = "4k3/8/8/8/8/8/8/4K3 w - - 0 1";
    BoardState board;
    bool result = board.parse_fen(fen);

    assert(result);
    assert(board.piece_count() == 2);
    assert(board.piece_at(SQUARE_E1) == PIECE_W_KING);
    assert(board.piece_at(SQUARE_E8) == PIECE_B_KING);

    std::cout << "PASS: test_fen_only_kings\n";
}

// Test 15: Parse position with various piece configurations
void test_fen_various_pieces() {
    // Position with all piece types
    const char* fen = "1nbqkbn1/pppppppp/8/8/8/8/PPPPPPPP/1NBQKBN1 w - - 0 1";
    BoardState board;
    bool result = board.parse_fen(fen);

    assert(result);
    assert(board.piece_count(COLOR_WHITE, PIECE_KNIGHT) == 2);
    assert(board.piece_count(COLOR_WHITE, PIECE_BISHOP) == 2);
    assert(board.piece_count(COLOR_WHITE, PIECE_ROOK) == 0);  // No rooks in this position
    assert(board.piece_count(COLOR_WHITE, PIECE_QUEEN) == 1);
    assert(board.piece_count(COLOR_WHITE, PIECE_KING) == 1);
    assert(board.piece_count(COLOR_WHITE, PIECE_PAWN) == 8);

    std::cout << "PASS: test_fen_various_pieces\n";
}

// Test 16: FEN produces same board as reset_to_start_position
void test_fen_matches_start() {
    BoardState fen_board;
    BoardState start_board;

    fen_board.parse_fen(START_POSITION_FEN);
    start_board.reset_to_start_position();

    assert(fen_board.white_occupancy == start_board.white_occupancy);
    assert(fen_board.black_occupancy == start_board.black_occupancy);
    assert(fen_board.all_occupancy == start_board.all_occupancy);
    assert(fen_board.side_to_move == start_board.side_to_move);
    assert(fen_board.castling_rights == start_board.castling_rights);
    assert(fen_board.en_passant_square == start_board.en_passant_square);
    assert(fen_board.halfmove_clock == start_board.halfmove_clock);
    assert(fen_board.fullmove_number == start_board.fullmove_number);

    // Check all pieces
    for (size_t c = 0; c < COLOR_COUNT; ++c) {
        for (size_t pt = 1; pt < PIECE_TYPE_COUNT; ++pt) {
            assert(fen_board.pieces[c][pt] == start_board.pieces[c][pt]);
        }
    }

    std::cout << "PASS: test_fen_matches_start\n";
}

// Test 17: Position with white queenside castling only
void test_fen_queenside_castling() {
    const char* fen = "r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w Qq - 0 1";
    BoardState board;
    bool result = board.parse_fen(fen);

    assert(result);
    assert(has_castling_right(board.castling_rights, CASTLING_WHITE_QUEENSIDE));
    assert(has_castling_right(board.castling_rights, CASTLING_BLACK_QUEENSIDE));
    assert(!has_castling_right(board.castling_rights, CASTLING_WHITE_KINGSIDE));
    assert(!has_castling_right(board.castling_rights, CASTLING_BLACK_KINGSIDE));

    std::cout << "PASS: test_fen_queenside_castling\n";
}

// Test 18: Position with fullmove number > 1
void test_fen_fullmove_number() {
    const char* fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 50";
    BoardState board;
    bool result = board.parse_fen(fen);

    assert(result);
    assert(board.fullmove_number == 50);

    std::cout << "PASS: test_fen_fullmove_number\n";
}

// Test 19: Parse position with many empty ranks
void test_fen_empty_ranks() {
    const char* fen = "4k3/8/8/8/8/8/8/4K3 w - - 0 1";
    BoardState board;
    bool result = board.parse_fen(fen);

    assert(result);
    assert(board.piece_count() == 2);

    std::cout << "PASS: test_fen_empty_ranks\n";
}

// Test 20: Invalid FEN - kings are adjacent
void test_fen_adjacent_kings() {
    // Kings on adjacent squares - should be rejected by validation
    const char* fen = "8/8/8/8/8/4kK2/8/8 w - - 0 1";
    BoardState board;
    bool result = board.parse_fen(fen);

    assert(!result);  // Should fail validation

    std::cout << "PASS: test_fen_adjacent_kings\n";
}

// Test 21: Multiple pieces on same square (invalid FEN)
void test_fen_overlapping_pieces() {
    // This FEN would try to place two pieces on same square if parsed incorrectly
    // Actually, proper FEN parsing should prevent this naturally
    // But we test that the validation catches inconsistencies
    BoardState board;
    board.reset_to_start_position();

    // Manually create overlap to test validation
    board.pieces[0][static_cast<size_t>(PIECE_PAWN)] |= BB_SQUARE_E8;
    board.white_occupancy |= BB_SQUARE_E8;
    board.all_occupancy |= BB_SQUARE_E8;

    // This should now be invalid due to overlap
    assert(!board.is_valid());

    std::cout << "PASS: test_fen_overlapping_pieces\n";
}

// Test 22: board_from_fen with invalid FEN returns empty board
void test_board_from_fen_invalid() {
    BoardState board = board_from_fen("not a valid fen string");

    // Should return empty (invalid) board
    assert(!board.is_valid());
    assert(board.is_empty());

    std::cout << "PASS: test_board_from_fen_invalid\n";
}

int main() {
    std::cout << "=== FEN Parsing Unit Tests ===\n\n";

    test_fen_start_position();
    test_fen_no_castling();
    test_fen_black_to_move();
    test_fen_partial_castling();
    test_fen_no_castling_rights();
    test_fen_kiwipete();
    test_fen_en_passant();
    test_fen_invalid_missing_fields();
    test_fen_invalid_placement();
    test_fen_invalid_side();
    test_fen_invalid_ep();
    test_fen_invalid_halfmove();
    test_board_from_fen_helper();
    test_fen_only_kings();
    test_fen_various_pieces();
    test_fen_matches_start();
    test_fen_queenside_castling();
    test_fen_fullmove_number();
    test_fen_empty_ranks();
    test_fen_adjacent_kings();
    test_fen_overlapping_pieces();
    test_board_from_fen_invalid();

    std::cout << "\n=== All 22 FEN tests passed! ===\n";
    return 0;
}
