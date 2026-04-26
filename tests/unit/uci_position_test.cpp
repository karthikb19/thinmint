#include <iostream>
#include <sstream>
#include <string>

#include "thinmint/uci/protocol.h"
#include "thinmint/movegen/attacks.h"

// Simple assertion helper - no streaming of enum values
#define ASSERT_TRUE(cond) do { if (!(cond)) { std::cerr << "FAIL: " #cond " at line " << __LINE__ << "\n"; return 1; } } while(0)
#define ASSERT_FALSE(cond) do { if (cond) { std::cerr << "FAIL: NOT " #cond " at line " << __LINE__ << "\n"; return 1; } } while(0)
#define ASSERT_EQ_INT(a, b) do { \
    int _a = static_cast<int>(a); \
    int _b = static_cast<int>(b); \
    if (_a != _b) { \
        std::cerr << "FAIL: " #a " == " #b " at line " << __LINE__ << " (got: " << _a << ", expected: " << _b << ")\n"; \
        return 1; \
    } \
} while(0)

int main() {
  // Initialize attack tables
  thinmint::movegen::init_all_attacks();

  std::cerr << "Running UCI position tests...\n";

  // Test 1: Protocol starts at initial position
  {
    std::cerr << "Test 1: Protocol starts at initial position\n";
    thinmint::uci::Protocol protocol;
    std::ostringstream out;

    const auto& board = protocol.GetBoard();

    // Check initial position has pieces on back ranks
    ASSERT_FALSE(board.is_empty());
    ASSERT_EQ_INT(board.piece_count(), 32);  // 32 pieces at start

    // White pieces on rank 1 and 2
    ASSERT_EQ_INT(board.piece_count(thinmint::core::COLOR_WHITE, thinmint::core::PIECE_PAWN), 8);
    ASSERT_EQ_INT(board.piece_count(thinmint::core::COLOR_WHITE, thinmint::core::PIECE_ROOK), 2);

    // Black pieces on rank 7 and 8
    ASSERT_EQ_INT(board.piece_count(thinmint::core::COLOR_BLACK, thinmint::core::PIECE_PAWN), 8);
    ASSERT_EQ_INT(board.piece_count(thinmint::core::COLOR_BLACK, thinmint::core::PIECE_ROOK), 2);

    // White to move
    ASSERT_EQ_INT(board.side_to_move, thinmint::core::COLOR_WHITE);
  }

  // Test 2: position startpos resets to initial position
  {
    std::cerr << "Test 2: position startpos resets to initial position\n";
    thinmint::uci::Protocol protocol;
    std::ostringstream out;

    // Apply a move to change position
    protocol.ProcessCommand("position startpos moves e2e4", out);

    // Reset with startpos
    protocol.ProcessCommand("position startpos", out);

    const auto& board = protocol.GetBoard();
    ASSERT_EQ_INT(board.piece_count(), 32);
    ASSERT_EQ_INT(board.side_to_move, thinmint::core::COLOR_WHITE);

    // Verify e2 pawn is back
    thinmint::core::Square e2 = thinmint::core::algebraic_to_square("e2");
    ASSERT_TRUE(board.is_square_occupied_by(e2, thinmint::core::COLOR_WHITE));
  }

  // Test 3: position fen with valid FEN
  {
    std::cerr << "Test 3: position fen with valid FEN\n";
    thinmint::uci::Protocol protocol;
    std::ostringstream out;

    // Load a position with black to move and specific castling rights
    protocol.ProcessCommand(
        "position fen r1bqkb1r/pppp1ppp/2n2n2/4p3/2B1P3/5N2/PPPP1PPP/RNBQK2R w KQkq - 4 4", out);

    const auto& board = protocol.GetBoard();
    ASSERT_EQ_INT(board.side_to_move, thinmint::core::COLOR_WHITE);
    ASSERT_TRUE(thinmint::core::has_castling_right(board.castling_rights, thinmint::core::CASTLING_WHITE_KINGSIDE));
    ASSERT_TRUE(thinmint::core::has_castling_right(board.castling_rights, thinmint::core::CASTLING_BLACK_KINGSIDE));
  }

  // Test 4: position fen with black to move
  {
    std::cerr << "Test 4: position fen with black to move\n";
    thinmint::uci::Protocol protocol;
    std::ostringstream out;

    protocol.ProcessCommand(
        "position fen rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1", out);

    const auto& board = protocol.GetBoard();
    ASSERT_EQ_INT(board.side_to_move, thinmint::core::COLOR_BLACK);
    ASSERT_EQ_INT(board.en_passant_square, thinmint::core::algebraic_to_square("e3"));
  }

  // Test 5: position startpos with single move
  {
    std::cerr << "Test 5: position startpos with single move\n";
    thinmint::uci::Protocol protocol;
    std::ostringstream out;

    protocol.ProcessCommand("position startpos moves e2e4", out);

    const auto& board = protocol.GetBoard();
    ASSERT_EQ_INT(board.side_to_move, thinmint::core::COLOR_BLACK);

    // e2 should be empty, e4 should have white pawn
    thinmint::core::Square e2 = thinmint::core::algebraic_to_square("e2");
    thinmint::core::Square e4 = thinmint::core::algebraic_to_square("e4");

    ASSERT_FALSE(board.is_square_occupied(e2));
    ASSERT_TRUE(board.is_square_occupied_by(e4, thinmint::core::COLOR_WHITE));
    ASSERT_EQ_INT(board.piece_type_at(e4), thinmint::core::PIECE_PAWN);
  }

  // Test 6: position startpos with multiple moves
  {
    std::cerr << "Test 6: position startpos with multiple moves\n";
    thinmint::uci::Protocol protocol;
    std::ostringstream out;

    // Sicilian Defense sequence
    protocol.ProcessCommand("position startpos moves e2e4 c7c5", out);

    const auto& board = protocol.GetBoard();
    ASSERT_EQ_INT(board.side_to_move, thinmint::core::COLOR_WHITE);

    // Verify both pawn moves
    thinmint::core::Square c5 = thinmint::core::algebraic_to_square("c5");
    thinmint::core::Square e4 = thinmint::core::algebraic_to_square("e4");

    ASSERT_TRUE(board.is_square_occupied_by(c5, thinmint::core::COLOR_BLACK));
    ASSERT_TRUE(board.is_square_occupied_by(e4, thinmint::core::COLOR_WHITE));
  }

  // Test 7: position startpos with capture
  {
    std::cerr << "Test 7: position startpos with capture\n";
    thinmint::uci::Protocol protocol;
    std::ostringstream out;

    // 1. e4 d5 2. exd5
    protocol.ProcessCommand("position startpos moves e2e4 d7d5 e4d5", out);

    const auto& board = protocol.GetBoard();
    thinmint::core::Square d5 = thinmint::core::algebraic_to_square("d5");

    // White pawn should be on d5, black pawn gone
    ASSERT_TRUE(board.is_square_occupied_by(d5, thinmint::core::COLOR_WHITE));
    ASSERT_EQ_INT(board.piece_type_at(d5), thinmint::core::PIECE_PAWN);
  }

  // Test 8: position with kingside castling
  {
    std::cerr << "Test 8: position with kingside castling\n";
    thinmint::uci::Protocol protocol;
    std::ostringstream out;

    // Use a FEN where kingside castling is legal for white
    // White king on e1, rook on h1, black king somewhere safe
    protocol.ProcessCommand(
        "position fen 4k3/8/8/8/8/8/8/4K2R w K - 0 1 moves e1g1", out);

    const auto& board = protocol.GetBoard();
    thinmint::core::Square g1 = thinmint::core::algebraic_to_square("g1");
    thinmint::core::Square f1 = thinmint::core::algebraic_to_square("f1");

    // King on g1, rook on f1
    ASSERT_TRUE(board.is_square_occupied_by(g1, thinmint::core::COLOR_WHITE));
    ASSERT_EQ_INT(board.piece_type_at(g1), thinmint::core::PIECE_KING);
    ASSERT_TRUE(board.is_square_occupied_by(f1, thinmint::core::COLOR_WHITE));
    ASSERT_EQ_INT(board.piece_type_at(f1), thinmint::core::PIECE_ROOK);

    // e1 and h1 should be empty
    thinmint::core::Square e1 = thinmint::core::algebraic_to_square("e1");
    thinmint::core::Square h1 = thinmint::core::algebraic_to_square("h1");
    ASSERT_FALSE(board.is_square_occupied(e1));
    ASSERT_FALSE(board.is_square_occupied(h1));
  }

  // Test 9: position with queenside castling
  {
    std::cerr << "Test 9: position with queenside castling\n";
    thinmint::uci::Protocol protocol;
    std::ostringstream out;

    // Load position where queenside castling is legal
    protocol.ProcessCommand(
        "position fen r3kbnr/ppppqppp/2n5/4p3/2B1P3/5N2/PPPP1PPP/RNBQK2R b KQkq - 4 4", out);
    protocol.ProcessCommand("position fen r3kbnr/ppppqppp/2n5/4p3/2B1P3/5N2/PPPP1PPP/RNBQK2R b KQkq - 4 4 moves e8c8", out);

    const auto& board = protocol.GetBoard();
    thinmint::core::Square c8 = thinmint::core::algebraic_to_square("c8");
    thinmint::core::Square d8 = thinmint::core::algebraic_to_square("d8");

    // Black king on c8, rook on d8
    ASSERT_TRUE(board.is_square_occupied_by(c8, thinmint::core::COLOR_BLACK));
    ASSERT_EQ_INT(board.piece_type_at(c8), thinmint::core::PIECE_KING);
    ASSERT_TRUE(board.is_square_occupied_by(d8, thinmint::core::COLOR_BLACK));
    ASSERT_EQ_INT(board.piece_type_at(d8), thinmint::core::PIECE_ROOK);
  }

  // Test 10: position with en passant capture
  {
    std::cerr << "Test 10: position with en passant capture\n";
    thinmint::uci::Protocol protocol;
    std::ostringstream out;

    // Set up position where EP is possible and execute it
    protocol.ProcessCommand(
        "position fen rnbqkbnr/ppp2ppp/4p3/3pP3/8/8/PPPP1PPP/RNBQKBNR w KQkq d6 0 3", out);
    protocol.ProcessCommand(
        "position fen rnbqkbnr/ppp2ppp/4p3/3pP3/8/8/PPPP1PPP/RNBQKBNR w KQkq d6 0 3 moves e5d6", out);

    const auto& board = protocol.GetBoard();
    thinmint::core::Square d6 = thinmint::core::algebraic_to_square("d6");
    thinmint::core::Square d5 = thinmint::core::algebraic_to_square("d5");

    // White pawn on d6, black pawn gone from d5
    ASSERT_TRUE(board.is_square_occupied_by(d6, thinmint::core::COLOR_WHITE));
    ASSERT_FALSE(board.is_square_occupied(d5));
  }

  // Test 11: position with promotion to queen
  {
    std::cerr << "Test 11: position with promotion to queen\n";
    thinmint::uci::Protocol protocol;
    std::ostringstream out;

    // Position with white pawn ready to promote
    protocol.ProcessCommand(
        "position fen 8/P7/8/8/8/8/8/4K2k w - - 0 1", out);
    protocol.ProcessCommand(
        "position fen 8/P7/8/8/8/8/8/4K2k w - - 0 1 moves a7a8q", out);

    const auto& board = protocol.GetBoard();
    thinmint::core::Square a8 = thinmint::core::algebraic_to_square("a8");
    thinmint::core::Square a7 = thinmint::core::algebraic_to_square("a7");

    // White queen on a8, a7 empty
    ASSERT_TRUE(board.is_square_occupied_by(a8, thinmint::core::COLOR_WHITE));
    ASSERT_EQ_INT(board.piece_type_at(a8), thinmint::core::PIECE_QUEEN);
    ASSERT_FALSE(board.is_square_occupied(a7));
  }

  // Test 12: position with promotion to knight
  {
    std::cerr << "Test 12: position with promotion to knight\n";
    thinmint::uci::Protocol protocol;
    std::ostringstream out;

    protocol.ProcessCommand(
        "position fen 8/P7/8/8/8/8/8/4K2k w - - 0 1 moves a7a8n", out);

    const auto& board = protocol.GetBoard();
    thinmint::core::Square a8 = thinmint::core::algebraic_to_square("a8");

    // White knight on a8
    ASSERT_TRUE(board.is_square_occupied_by(a8, thinmint::core::COLOR_WHITE));
    ASSERT_EQ_INT(board.piece_type_at(a8), thinmint::core::PIECE_KNIGHT);
  }

  // Test 13: invalid FEN keeps current position
  {
    std::cerr << "Test 13: invalid FEN keeps current position\n";
    thinmint::uci::Protocol protocol;
    std::ostringstream out;

    // First set a valid position
    protocol.ProcessCommand("position startpos", out);
    const auto& board = protocol.GetBoard();
    ASSERT_EQ_INT(board.piece_count(), 32);

    // Try invalid FEN
    protocol.ProcessCommand("position fen invalid fen here", out);

    // Position should still be startpos
    ASSERT_EQ_INT(board.piece_count(), 32);
  }

  // Test 14: invalid move in sequence stops processing
  {
    std::cerr << "Test 14: invalid move in sequence stops processing\n";
    thinmint::uci::Protocol protocol;
    std::ostringstream out;

    // First valid move, then invalid
    protocol.ProcessCommand("position startpos moves e2e4 invalid_move", out);

    const auto& board = protocol.GetBoard();
    // e2e4 should have been applied
    thinmint::core::Square e4 = thinmint::core::algebraic_to_square("e4");
    ASSERT_TRUE(board.is_square_occupied_by(e4, thinmint::core::COLOR_WHITE));
  }

  // Test 15: long move sequence
  {
    std::cerr << "Test 15: long move sequence\n";
    thinmint::uci::Protocol protocol;
    std::ostringstream out;

    // Vienna Game sequence
    protocol.ProcessCommand(
        "position startpos moves e2e4 e7e5 b1c3 g8f6 f1c4 f8c5 d2d3 d7d6 c1g5 h7h6", out);

    const auto& board = protocol.GetBoard();

    // Verify final position state
    // White bishop on c4 and g5
    thinmint::core::Square c4 = thinmint::core::algebraic_to_square("c4");
    thinmint::core::Square g5 = thinmint::core::algebraic_to_square("g5");
    ASSERT_TRUE(board.is_square_occupied_by(c4, thinmint::core::COLOR_WHITE));
    ASSERT_TRUE(board.is_square_occupied_by(g5, thinmint::core::COLOR_WHITE));
    ASSERT_EQ_INT(board.piece_type_at(c4), thinmint::core::PIECE_BISHOP);
    ASSERT_EQ_INT(board.piece_type_at(g5), thinmint::core::PIECE_BISHOP);

    // It's white's turn after the 5... h6 move
    ASSERT_EQ_INT(board.side_to_move, thinmint::core::COLOR_WHITE);
  }

  // Test 16: position with promotion capture
  {
    std::cerr << "Test 16: position with promotion capture\n";
    thinmint::uci::Protocol protocol;
    std::ostringstream out;

    // White pawn captures on a8 and promotes
    protocol.ProcessCommand(
        "position fen 1r6/P7/8/8/8/8/8/4K2k w - - 0 1", out);
    protocol.ProcessCommand(
        "position fen 1r6/P7/8/8/8/8/8/4K2k w - - 0 1 moves a7b8q", out);

    const auto& board = protocol.GetBoard();
    thinmint::core::Square b8 = thinmint::core::algebraic_to_square("b8");
    thinmint::core::Square a7 = thinmint::core::algebraic_to_square("a7");

    // White queen on b8, rook captured, a7 empty
    ASSERT_TRUE(board.is_square_occupied_by(b8, thinmint::core::COLOR_WHITE));
    ASSERT_EQ_INT(board.piece_type_at(b8), thinmint::core::PIECE_QUEEN);
    ASSERT_FALSE(board.is_square_occupied(a7));
    // Verify rook is gone
    ASSERT_EQ_INT(board.piece_count(thinmint::core::COLOR_BLACK, thinmint::core::PIECE_ROOK), 0);
  }

  std::cerr << "All UCI position tests passed!\n";
  return 0;
}
