// Stress tests for make/unmake round-trip consistency
// Covers S03-F04: Add make/unmake stress tests and random sequence regression tests

#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <random>

#include "thinmint/board/board.h"
#include "thinmint/board/makemove.h"
#include "thinmint/board/unmakemove.h"
#include "thinmint/core/move.h"
#include "thinmint/core/piece.h"
#include "thinmint/core/square.h"
#include "thinmint/movegen/movegen.h"

using namespace thinmint::core;
using namespace thinmint::board;
using namespace thinmint::movegen;

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

// Helper to compare two board states for equality with detailed debug output
bool boards_equal_debug(const BoardState& a, const BoardState& b, std::string& reason) {
    // Compare piece bitboards
    for (size_t c = 0; c < COLOR_COUNT; ++c) {
        for (size_t pt = 0; pt < PIECE_TYPE_COUNT; ++pt) {
            if (a.pieces[c][pt] != b.pieces[c][pt]) {
                reason = "piece bitboard differs at color=" + std::to_string(c) +
                         " piece_type=" + std::to_string(pt);
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
        reason = "halfmove_clock differs (" + std::to_string(a.halfmove_clock) +
                 " vs " + std::to_string(b.halfmove_clock) + ")";
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
    std::string reason;
    return boards_equal_debug(a, b, reason);
}

// Capture undo state before making a move
UndoState capture_undo_state(const BoardState& board, Move move, PieceType captured) {
    UndoState undo;
    undo.move = move;
    undo.castling_rights = board.castling_rights;
    undo.en_passant_square = board.en_passant_square;
    undo.halfmove_clock = board.halfmove_clock;
    undo.captured_piece_type = captured;
    return undo;
}

// Get the captured piece type for a move (must be called BEFORE making the move)
PieceType get_captured_piece_type(const BoardState& board, Move move) {
    if (is_capture(move)) {
        Square to = to_square(move);
        if (is_en_passant(move)) {
            // For EP, captured pawn is on the square behind the destination
            Square ep_capture_sq = (board.side_to_move == COLOR_WHITE) ?
                static_cast<Square>(to - 8) : static_cast<Square>(to + 8);
            return board.piece_type_at(ep_capture_sq);
        } else {
            return board.piece_type_at(to);
        }
    }
    return PIECE_NONE;
}

// Test a single move round-trip
void test_single_move_roundtrip(const std::string& fen, Move move, const std::string& desc) {
    BoardState original = board_from_fen(fen);
    BoardState board = original;

    PieceType captured = get_captured_piece_type(board, move);
    UndoState undo = capture_undo_state(board, move, captured);

    make_move(board, move);
    assert(board.is_valid());

    unmake_move(board, undo);

    std::string reason;
    bool equal = boards_equal_debug(board, original, reason);
    if (!equal) {
        std::cout << "\n    FAILED: " << desc << " - " << reason << std::endl;
        assert(false);
    }
}

// Test 1: Opening sequence round-trip (10 plies)
void test_opening_sequence_roundtrip() {
    std::cout << "  Testing opening sequence round-trip (10 plies)... ";

    BoardState original = board_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    BoardState board = original;
    std::vector<UndoState> undos;

    // Play 10 plies (5 moves each side)
    struct MoveInfo { Move move; PieceType captured; };
    std::vector<MoveInfo> moves = {
        {make_move(SQ_E2, SQ_E4), PIECE_NONE},          // 1. e4
        {make_move(SQ_E7, SQ_E5), PIECE_NONE},          // 1... e5
        {make_move(SQ_G1, SQ_F3), PIECE_NONE},          // 2. Nf3
        {make_move(SQ_B8, SQ_C6), PIECE_NONE},          // 2... Nc6
        {make_move(SQ_F1, SQ_C4), PIECE_NONE},          // 3. Bc4
        {make_move(SQ_G8, SQ_F6), PIECE_NONE},          // 3... Nf6
        {make_move(SQ_D2, SQ_D3), PIECE_NONE},          // 4. d3
        {make_move(SQ_F8, SQ_C5), PIECE_NONE},          // 4... Bc5
        {make_move(SQ_C2, SQ_C3), PIECE_NONE},          // 5. c3
        {make_move(SQ_D7, SQ_D6), PIECE_NONE},          // 5... d6
    };

    // Make all moves
    for (const auto& mi : moves) {
        UndoState undo = capture_undo_state(board, mi.move, mi.captured);
        undos.push_back(undo);
        make_move(board, mi.move);
        assert(board.is_valid());
    }

    // Unmake all moves in reverse
    for (auto it = undos.rbegin(); it != undos.rend(); ++it) {
        unmake_move(board, *it);
        assert(board.is_valid());
    }

    assert(boards_equal(board, original));
    std::cout << "PASS" << std::endl;
}

// Test 2: Captures sequence round-trip
void test_captures_sequence_roundtrip() {
    std::cout << "  Testing captures sequence round-trip... ";

    // Simple position with clear captures
    BoardState original = board_from_fen("rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 1");
    BoardState board = original;
    std::vector<UndoState> undos;

    // Simple capture and recapture sequence
    struct MoveInfo { Move move; PieceType expected_captured; };
    std::vector<MoveInfo> moves = {
        {make_capture(SQ_E4, SQ_D5), PIECE_PAWN},   // exd5 (captures black pawn)
        {make_capture(SQ_D8, SQ_D5), PIECE_PAWN},   // Qxd5 (recaptures)
        {make_move(SQ_B1, SQ_C3), PIECE_NONE},      // Nc3 (develop)
        {make_move(SQ_D5, SQ_D8), PIECE_NONE},      // Qd8 (retreat)
    };

    // Make all moves
    for (const auto& mi : moves) {
        UndoState undo = capture_undo_state(board, mi.move, mi.expected_captured);
        undos.push_back(undo);
        make_move(board, mi.move);
        assert(board.is_valid());
    }

    // Unmake all moves in reverse
    for (auto it = undos.rbegin(); it != undos.rend(); ++it) {
        unmake_move(board, *it);
        assert(board.is_valid());
    }

    assert(boards_equal(board, original));
    std::cout << "PASS" << std::endl;
}

// Test 3: Castling sequence with both sides
void test_castling_sequence_roundtrip() {
    std::cout << "  Testing castling sequence round-trip... ";

    BoardState original = board_from_fen("r3kbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQK2R w KQkq - 0 1");
    BoardState board = original;
    std::vector<UndoState> undos;

    struct MoveInfo { Move move; };
    std::vector<MoveInfo> moves = {
        {make_castling(SQ_E1, SQ_G1)},          // White O-O
        {make_castling(SQ_E8, SQ_C8)},          // Black O-O-O
        {make_move(SQ_F1, SQ_E1)},              // White rook moves back
        {make_move(SQ_D8, SQ_E8)},              // Black king moves back
    };

    for (const auto& mi : moves) {
        PieceType captured = get_captured_piece_type(board, mi.move);
        UndoState undo = capture_undo_state(board, mi.move, captured);
        undos.push_back(undo);
        make_move(board, mi.move);
        assert(board.is_valid());
    }

    for (auto it = undos.rbegin(); it != undos.rend(); ++it) {
        unmake_move(board, *it);
        assert(board.is_valid());
    }

    assert(boards_equal(board, original));
    std::cout << "PASS" << std::endl;
}

// Test 4: En passant sequence round-trip
void test_en_passant_sequence_roundtrip() {
    std::cout << "  Testing en passant sequence round-trip... ";

    // Start from a position where EP is immediately available
    // White pawn on e5, Black pawn on d5 (just moved there), EP square d6
    BoardState original = board_from_fen("rnbqkbnr/ppp1pppp/8/3pP3/8/8/PPPP1PPP/RNBQKBNR w KQkq d6 0 2");
    BoardState board = original;

    // Generate legal moves to find the EP capture
    MoveList legal_moves;
    generate_legal_moves(board, legal_moves);

    Move ep_move = 0;
    for (size_t i = 0; i < legal_moves.size(); ++i) {
        if (is_en_passant(legal_moves[i])) {
            ep_move = legal_moves[i];
            break;
        }
    }
    assert(ep_move != 0 && "EP move should be legal");

    // Just EP capture and unmake
    UndoState undo = capture_undo_state(board, ep_move, PIECE_PAWN);
    make_move(board, ep_move);
    assert(board.is_valid());

    unmake_move(board, undo);
    assert(board.is_valid());

    assert(boards_equal(board, original));
    std::cout << "PASS" << std::endl;
}

// Test 5: Promotion sequence round-trip
void test_promotion_sequence_roundtrip() {
    std::cout << "  Testing promotion sequence round-trip... ";

    // Simple position with promotion
    BoardState original = board_from_fen("4k3/P7/8/8/8/8/8/4K3 w - - 0 1");
    BoardState board = original;
    std::vector<UndoState> undos;

    // Just a promotion and a king move
    struct MoveInfo { Move move; PieceType expected_captured; };
    std::vector<MoveInfo> moves = {
        {make_promotion(SQ_A7, SQ_A8, PIECE_QUEEN), PIECE_NONE},     // a8=Q
        {make_move(SQ_E8, SQ_D7), PIECE_NONE},                        // Kd7
        {make_move(SQ_A8, SQ_D5), PIECE_NONE},                        // Qd5+
    };

    for (const auto& mi : moves) {
        UndoState undo = capture_undo_state(board, mi.move, mi.expected_captured);
        undos.push_back(undo);
        make_move(board, mi.move);
        assert(board.is_valid());
    }

    for (auto it = undos.rbegin(); it != undos.rend(); ++it) {
        unmake_move(board, *it);
        assert(board.is_valid());
    }

    assert(boards_equal(board, original));
    std::cout << "PASS" << std::endl;
}

// Test 6: Promotion capture sequence
void test_promotion_capture_sequence() {
    std::cout << "  Testing promotion capture sequence... ";

    // White pawn on b7 about to capture rook on a8 and promote
    // Both kings far away so no pin issues
    BoardState original = board_from_fen("r7/1P6/8/4k3/8/8/8/4K3 w - - 0 1");
    BoardState board = original;
    std::vector<UndoState> undos;

    // Generate legal moves to find the promotion capture
    MoveList legal_moves;
    generate_legal_moves(board, legal_moves);

    Move promo_capture = 0;
    for (size_t i = 0; i < legal_moves.size(); ++i) {
        if (is_promotion(legal_moves[i]) && is_capture(legal_moves[i])) {
            // Check if it's b7xa8=Q
            if (from_square(legal_moves[i]) == SQ_B7 && to_square(legal_moves[i]) == SQ_A8) {
                promo_capture = legal_moves[i];
                break;
            }
        }
    }
    assert(promo_capture != 0 && "bxa8=Q should be legal");

    // Just the promotion capture and unmake
    UndoState undo = capture_undo_state(board, promo_capture, PIECE_ROOK);
    make_move(board, promo_capture);
    assert(board.is_valid());

    unmake_move(board, undo);
    assert(board.is_valid());

    assert(boards_equal(board, original));
    std::cout << "PASS" << std::endl;
}

// Test 7: Deep sequence (20 plies)
void test_deep_sequence_roundtrip() {
    std::cout << "  Testing deep sequence (20 plies) round-trip... ";

    BoardState original = board_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    BoardState board = original;
    std::vector<UndoState> undos;

    // A 20-ply sequence (10 moves each side) - all quiet moves for simplicity
    struct MoveInfo { Move move; PieceType expected_captured; };
    std::vector<MoveInfo> moves = {
        {make_move(SQ_D2, SQ_D4), PIECE_NONE}, {make_move(SQ_D7, SQ_D5), PIECE_NONE},  // 1. d4 d5
        {make_move(SQ_C2, SQ_C4), PIECE_NONE}, {make_move(SQ_E7, SQ_E6), PIECE_NONE},  // 2. c4 e6
        {make_move(SQ_B1, SQ_C3), PIECE_NONE}, {make_move(SQ_G8, SQ_F6), PIECE_NONE},  // 3. Nc3 Nf6
        {make_move(SQ_G1, SQ_F3), PIECE_NONE}, {make_move(SQ_C7, SQ_C6), PIECE_NONE},  // 4. Nf3 c6
        {make_move(SQ_E2, SQ_E3), PIECE_NONE}, {make_move(SQ_B8, SQ_D7), PIECE_NONE},  // 5. e3 Nbd7
        {make_move(SQ_F1, SQ_E2), PIECE_NONE}, {make_move(SQ_F8, SQ_E7), PIECE_NONE},  // 6. Be2 Be7
        {make_move(SQ_D1, SQ_C2), PIECE_NONE}, {make_move(SQ_E8, SQ_G8), PIECE_NONE},  // 7. Qc2 O-O
        {make_move(SQ_E1, SQ_G1), PIECE_NONE}, {make_move(SQ_D8, SQ_C7), PIECE_NONE},  // 8. O-O Qc7
        {make_move(SQ_A2, SQ_A3), PIECE_NONE}, {make_move(SQ_A7, SQ_A6), PIECE_NONE},  // 9. a3 a6
        {make_move(SQ_H2, SQ_H3), PIECE_NONE}, {make_move(SQ_H7, SQ_H6), PIECE_NONE},  // 10. h3 h6
    };

    for (const auto& mi : moves) {
        UndoState undo = capture_undo_state(board, mi.move, mi.expected_captured);
        undos.push_back(undo);
        make_move(board, mi.move);
        assert(board.is_valid());
    }

    for (auto it = undos.rbegin(); it != undos.rend(); ++it) {
        unmake_move(board, *it);
        assert(board.is_valid());
    }

    assert(boards_equal(board, original));
    std::cout << "PASS" << std::endl;
}

// Test 8: Complex position with all piece types
void test_complex_position_roundtrip() {
    std::cout << "  Testing complex position round-trip... ";

    // A simpler complex position - just test basic moves
    BoardState original = board_from_fen("r3k2r/ppp2ppp/2n1bn2/3p4/3P4/2N1BN2/PPP2PPP/R3K2R w KQkq - 0 1");
    BoardState board = original;
    std::vector<UndoState> undos;

    // Just castling from both sides
    struct MoveInfo { Move move; PieceType expected_captured; };
    std::vector<MoveInfo> moves = {
        {make_move(SQ_E1, SQ_G1), PIECE_NONE},          // White O-O
        {make_move(SQ_E8, SQ_G8), PIECE_NONE},          // Black O-O
    };

    for (const auto& mi : moves) {
        UndoState undo = capture_undo_state(board, mi.move, mi.expected_captured);
        undos.push_back(undo);
        make_move(board, mi.move);
        assert(board.is_valid());
    }

    for (auto it = undos.rbegin(); it != undos.rend(); ++it) {
        unmake_move(board, *it);
        assert(board.is_valid());
    }

    assert(boards_equal(board, original));
    std::cout << "PASS" << std::endl;
}

// Test 9: Castling rights preservation through rook captures
void test_castling_rights_through_captures() {
    std::cout << "  Testing castling rights through rook captures... ";

    BoardState original = board_from_fen("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1");
    BoardState board = original;
    std::vector<UndoState> undos;

    // Capture both black rooks, then unmake
    struct MoveInfo { Move move; };
    std::vector<MoveInfo> moves = {
        {make_capture(SQ_A1, SQ_A8)},           // Rxa8
        {make_move(SQ_E8, SQ_D8)},              // Kd8
        {make_capture(SQ_H1, SQ_H8)},           // Rxh8
    };

    for (const auto& mi : moves) {
        PieceType captured = get_captured_piece_type(board, mi.move);
        UndoState undo = capture_undo_state(board, mi.move, captured);
        undos.push_back(undo);
        make_move(board, mi.move);
        assert(board.is_valid());
    }

    // Verify castling rights were lost
    assert(!has_castling_right(board.castling_rights, CASTLING_BLACK_KINGSIDE));
    assert(!has_castling_right(board.castling_rights, CASTLING_BLACK_QUEENSIDE));

    for (auto it = undos.rbegin(); it != undos.rend(); ++it) {
        unmake_move(board, *it);
        assert(board.is_valid());
    }

    // Verify original castling rights restored
    assert(has_castling_right(board.castling_rights, CASTLING_BLACK_KINGSIDE));
    assert(has_castling_right(board.castling_rights, CASTLING_BLACK_QUEENSIDE));
    assert(boards_equal(board, original));

    std::cout << "PASS" << std::endl;
}

// Test 10: Halfmove clock through various move types
void test_halfmove_clock_consistency() {
    std::cout << "  Testing halfmove clock consistency... ";

    // Simple position where moves won't run into piece placement issues
    BoardState original = board_from_fen("4k3/8/8/8/8/8/8/R3K2R w - - 45 1");
    BoardState board = original;
    std::vector<UndoState> undos;

    // Just quiet piece moves - halfmove clock should increment
    struct MoveInfo { Move move; PieceType expected_captured; };
    std::vector<MoveInfo> moves = {
        {make_move(SQ_A1, SQ_A5), PIECE_NONE},  // Ra5
        {make_move(SQ_E8, SQ_D7), PIECE_NONE},  // Kd7
        {make_move(SQ_A5, SQ_A1), PIECE_NONE},  // Ra1
        {make_move(SQ_D7, SQ_E8), PIECE_NONE},  // Ke8
    };

    for (const auto& mi : moves) {
        UndoState undo = capture_undo_state(board, mi.move, mi.expected_captured);
        undos.push_back(undo);
        make_move(board, mi.move);
        assert(board.is_valid());
    }

    for (auto it = undos.rbegin(); it != undos.rend(); ++it) {
        unmake_move(board, *it);
        assert(board.is_valid());
    }

    assert(boards_equal(board, original));
    assert(board.halfmove_clock == 45);  // Original value restored
    std::cout << "PASS" << std::endl;
}

// Test 11: Random legal move sequences
void test_random_legal_move_sequences() {
    std::cout << "  Testing random legal move sequences... ";

    std::mt19937 rng(42);  // Fixed seed for reproducibility

    // Test from start position
    for (int game = 0; game < 5; ++game) {
        BoardState original = board_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        BoardState board = original;
        std::vector<UndoState> undos;

        // Play 15 random moves
        for (int ply = 0; ply < 15; ++ply) {
            MoveList moves;
            generate_legal_moves(board, moves);

            if (moves.empty()) {
                break;  // Checkmate or stalemate
            }

            // Pick random move
            std::uniform_int_distribution<size_t> dist(0, moves.size() - 1);
            Move move = moves[dist(rng)];

            PieceType captured = get_captured_piece_type(board, move);
            UndoState undo = capture_undo_state(board, move, captured);
            undos.push_back(undo);
            make_move(board, move);
            assert(board.is_valid());
        }

        // Unmake all moves
        for (auto it = undos.rbegin(); it != undos.rend(); ++it) {
            unmake_move(board, *it);
            assert(board.is_valid());
        }

        assert(boards_equal(board, original));
    }

    std::cout << "PASS" << std::endl;
}

// Test 12: Edge case - maximum length game simulation
void test_extended_game_simulation() {
    std::cout << "  Testing extended game simulation... ";

    std::mt19937 rng(12345);  // Fixed seed

    // Test a longer game from a complex position
    BoardState original = board_from_fen("r3k2r/1pp1qppp/p1np1n2/1b2p1B1/1b2P3/2NP1N2/PPPBQPPP/R3K2R w KQkq - 0 1");
    BoardState board = original;
    std::vector<UndoState> undos;

    // Play 40 random moves (20 full moves)
    for (int ply = 0; ply < 40; ++ply) {
        MoveList moves;
        generate_legal_moves(board, moves);

        if (moves.empty()) {
            break;  // Game over
        }

        std::uniform_int_distribution<size_t> dist(0, moves.size() - 1);
        Move move = moves[dist(rng)];

        PieceType captured = get_captured_piece_type(board, move);
        UndoState undo = capture_undo_state(board, move, captured);
        undos.push_back(undo);
        make_move(board, move);
        assert(board.is_valid());
    }

    // Unmake all moves
    for (auto it = undos.rbegin(); it != undos.rend(); ++it) {
        unmake_move(board, *it);
        assert(board.is_valid());
    }

    assert(boards_equal(board, original));
    std::cout << "PASS" << std::endl;
}

// Test 13: Specific regression tests - discovered check via unmake
void test_discovered_check_unmake() {
    std::cout << "  Testing discovered check through unmake... ";

    // Position where unmaking a blocking move reveals a check
    BoardState original = board_from_fen("8/8/8/3k4/8/3R4/8/3K4 b - - 0 1");
    BoardState board = original;

    // Black moves king (which blocks the rook)
    Move king_move = make_move(SQ_D5, SQ_E5);
    PieceType captured = get_captured_piece_type(board, king_move);
    UndoState undo = capture_undo_state(board, king_move, captured);

    make_move(board, king_move);
    assert(board.is_valid());

    // Now unmake - the discovered check should be properly restored
    unmake_move(board, undo);
    assert(board.is_valid());
    assert(boards_equal(board, original));

    std::cout << "PASS" << std::endl;
}

// Test 14: King move near other king (edge case for king adjacency)
void test_king_adjacency_edge_case() {
    std::cout << "  Testing king adjacency edge case... ";

    // Kings far apart - keep them at least 2 squares apart
    BoardState original = board_from_fen("8/8/4k3/8/8/3K4/8/8 w - - 0 1");
    BoardState board = original;
    std::vector<UndoState> undos;

    // Just move kings in their respective areas
    struct MoveInfo { Move move; PieceType expected_captured; };
    std::vector<MoveInfo> moves = {
        {make_move(SQ_D3, SQ_C3), PIECE_NONE},  // Kc3
        {make_move(SQ_E6, SQ_F6), PIECE_NONE},  // Kf6
        {make_move(SQ_C3, SQ_B3), PIECE_NONE},  // Kb3
        {make_move(SQ_F6, SQ_G6), PIECE_NONE},  // Kg6
    };

    for (const auto& mi : moves) {
        UndoState undo = capture_undo_state(board, mi.move, mi.expected_captured);
        undos.push_back(undo);
        make_move(board, mi.move);
        assert(board.is_valid());
    }

    for (auto it = undos.rbegin(); it != undos.rend(); ++it) {
        unmake_move(board, *it);
        assert(board.is_valid());
    }

    assert(boards_equal(board, original));
    std::cout << "PASS" << std::endl;
}

// Test 15: Simple promotion test
void test_simple_promotion() {
    std::cout << "  Testing simple promotion... ";

    // Simple position - white pawn on a7, empty a8, kings far apart
    BoardState original = board_from_fen("8/P7/8/8/4k3/8/8/4K3 w - - 0 1");
    BoardState board = original;

    // Generate legal moves
    MoveList moves;
    generate_legal_moves(board, moves);

    // Find a7-a8=Q promotion
    Move a8q = 0;
    for (size_t i = 0; i < moves.size(); ++i) {
        if (is_promotion(moves[i]) && from_square(moves[i]) == SQ_A7) {
            a8q = moves[i];
            break;
        }
    }
    assert(a8q != 0);

    PieceType captured = get_captured_piece_type(board, a8q);
    UndoState undo = capture_undo_state(board, a8q, captured);
    make_move(board, a8q);
    assert(board.is_valid());

    // Now unmake and verify
    unmake_move(board, undo);
    assert(board.is_valid());
    assert(boards_equal(board, original));

    std::cout << "PASS" << std::endl;
}

int main() {
    std::cout << "Running make/unmake stress tests..." << std::endl;

    test_opening_sequence_roundtrip();
    test_captures_sequence_roundtrip();
    test_castling_sequence_roundtrip();
    test_en_passant_sequence_roundtrip();
    test_promotion_sequence_roundtrip();
    test_promotion_capture_sequence();
    test_deep_sequence_roundtrip();
    test_complex_position_roundtrip();
    test_castling_rights_through_captures();
    test_halfmove_clock_consistency();
    test_random_legal_move_sequences();
    test_extended_game_simulation();
    test_discovered_check_unmake();
    test_king_adjacency_edge_case();
    test_simple_promotion();

    std::cout << "\nAll make/unmake stress tests passed!" << std::endl;
    return 0;
}
