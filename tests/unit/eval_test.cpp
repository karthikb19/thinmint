#include <iostream>
#include <cstdlib>
#include <cmath>

#include "thinmint/eval/eval.h"
#include "thinmint/board/board.h"
#include "thinmint/movegen/attacks.h"

using namespace thinmint;
using namespace thinmint::core;
using namespace thinmint::board;
using namespace thinmint::eval;
using namespace thinmint::movegen;

// Simple test framework
#define TEST_ASSERT(cond, msg) do { \
    if (!(cond)) { \
        std::cerr << "FAIL: " << msg << " at line " << __LINE__ << std::endl; \
        return 1; \
    } \
} while(0)

#define RUN_TEST(name) do { \
    std::cout << "Running " << #name << "... "; \
    if (name() == 0) { \
        std::cout << "PASS" << std::endl; \
    } else { \
        std::cerr << "FAIL" << std::endl; \
        return 1; \
    } \
} while(0)

// Test material values
int test_material_values() {
    // Test piece values
    TEST_ASSERT(piece_value(PIECE_PAWN) == PAWN_VALUE, "Pawn value");
    TEST_ASSERT(piece_value(PIECE_KNIGHT) == KNIGHT_VALUE, "Knight value");
    TEST_ASSERT(piece_value(PIECE_BISHOP) == BISHOP_VALUE, "Bishop value");
    TEST_ASSERT(piece_value(PIECE_ROOK) == ROOK_VALUE, "Rook value");
    TEST_ASSERT(piece_value(PIECE_QUEEN) == QUEEN_VALUE, "Queen value");
    TEST_ASSERT(piece_value(PIECE_KING) == KING_VALUE, "King value");
    TEST_ASSERT(piece_value(PIECE_NONE) == 0, "None value");

    return 0;
}

// Test material evaluation for equal positions
int test_equal_material() {
    BoardState board;
    board.reset_to_start_position();

    // Start position has equal material
    int material = evaluate_material(board);
    TEST_ASSERT(material == 0, "Start position material should be equal (0)");

    return 0;
}

// Test material evaluation for imbalanced positions
int test_material_imbalance() {
    // White up a pawn
    BoardState board1;
    board1.parse_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    // Remove a black pawn
    board1.pieces[static_cast<size_t>(COLOR_BLACK)][static_cast<size_t>(PIECE_PAWN)] &= ~bb_from_square(52); // a7
    board1.black_occupancy &= ~bb_from_square(52);
    board1.all_occupancy = board1.white_occupancy | board1.black_occupancy;

    int material1 = evaluate_material(board1);
    TEST_ASSERT(material1 == PAWN_VALUE, "White up a pawn should be +100");

    // Black up a knight
    BoardState board2;
    board2.reset_to_start_position();
    // Remove a white knight
    board2.pieces[static_cast<size_t>(COLOR_WHITE)][static_cast<size_t>(PIECE_KNIGHT)] &= ~bb_from_square(1); // b1
    board2.white_occupancy &= ~bb_from_square(1);
    board2.all_occupancy = board2.white_occupancy | board2.black_occupancy;

    int material2 = evaluate_material(board2);
    TEST_ASSERT(material2 == -KNIGHT_VALUE, "Black up a knight should be -320");

    return 0;
}

// Test PST access
int test_pst_access() {
    TEST_ASSERT(get_pst(PIECE_PAWN) == PAWN_PST, "Pawn PST access");
    TEST_ASSERT(get_pst(PIECE_KNIGHT) == KNIGHT_PST, "Knight PST access");
    TEST_ASSERT(get_pst(PIECE_BISHOP) == BISHOP_PST, "Bishop PST access");
    TEST_ASSERT(get_pst(PIECE_ROOK) == ROOK_PST, "Rook PST access");
    TEST_ASSERT(get_pst(PIECE_QUEEN) == QUEEN_PST, "Queen PST access");
    TEST_ASSERT(get_pst(PIECE_KING) == KING_PST, "King PST access");
    TEST_ASSERT(get_pst(PIECE_NONE) == nullptr, "None PST returns null");

    return 0;
}

int test_endgame_pst_access() {
    TEST_ASSERT(get_pst(PIECE_PAWN, EvalPhase::ENDGAME) == PAWN_ENDGAME_PST, "Pawn endgame PST access");
    TEST_ASSERT(get_pst(PIECE_KNIGHT, EvalPhase::ENDGAME) == KNIGHT_ENDGAME_PST, "Knight endgame PST access");
    TEST_ASSERT(get_pst(PIECE_BISHOP, EvalPhase::ENDGAME) == BISHOP_ENDGAME_PST, "Bishop endgame PST access");
    TEST_ASSERT(get_pst(PIECE_ROOK, EvalPhase::ENDGAME) == ROOK_ENDGAME_PST, "Rook endgame PST access");
    TEST_ASSERT(get_pst(PIECE_QUEEN, EvalPhase::ENDGAME) == QUEEN_ENDGAME_PST, "Queen endgame PST access");
    TEST_ASSERT(get_pst(PIECE_KING, EvalPhase::ENDGAME) == KING_ENDGAME_PST, "King endgame PST access");

    return 0;
}

// Test piece-square evaluation for central squares
int test_piece_square_center() {
    // Knight in center (d4 = square 27) should have good score
    int white_knight_d4 = evaluate_piece_position(27, PIECE_KNIGHT, true);
    // Knight in corner (a1 = square 0) should have bad score
    int white_knight_a1 = evaluate_piece_position(0, PIECE_KNIGHT, true);

    TEST_ASSERT(white_knight_d4 > white_knight_a1, "Knight in center better than corner");

    // Same for Black (squares mirrored)
    int black_knight_d5 = evaluate_piece_position(35, PIECE_KNIGHT, false);  // d5 from Black's view
    int black_knight_a8 = evaluate_piece_position(56, PIECE_KNIGHT, false); // a8 from Black's view

    TEST_ASSERT(black_knight_d5 > black_knight_a8, "Black knight in center better than corner");

    return 0;
}

// Test full evaluation returns correct perspective
int test_evaluate_perspective() {
    BoardState board;
    board.reset_to_start_position();

    // Start position should evaluate to approximately 0 (slight White advantage due to tempo)
    int white_eval = evaluate(board);
    // White to move, slight advantage expected (just tempo)
    // PST values for start position favor central pawns slightly
    TEST_ASSERT(std::abs(white_eval) < 50, "Start position eval should be close to 0");

    return 0;
}

// Test evaluation flips correctly with side to move
int test_evaluate_flips_with_side() {
    // Create position where White has material advantage
    BoardState board;
    board.parse_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    // Remove black queen
    board.pieces[static_cast<size_t>(COLOR_BLACK)][static_cast<size_t>(PIECE_QUEEN)] = BB_EMPTY;
    board.black_occupancy &= ~bb_from_square(59); // d8
    board.all_occupancy = board.white_occupancy | board.black_occupancy;
    board.side_to_move = COLOR_WHITE;

    int white_to_move = evaluate(board);
    // White is up a queen, should be positive from White's perspective
    TEST_ASSERT(white_to_move > 800, "White up queen, White to move should be highly positive");

    // Same position, Black to move
    board.side_to_move = COLOR_BLACK;
    int black_to_move = evaluate(board);
    // Should be negative from Black's perspective (they're down a queen)
    TEST_ASSERT(black_to_move < -800, "White up queen, Black to move should be highly negative");

    // They should be opposites
    TEST_ASSERT(white_to_move == -black_to_move, "Evaluation should flip sign with side to move");

    return 0;
}

// Test positional evaluation affects score
int test_positional_factors() {
    // Compare two positions with same material but different piece placement

    // Position 1: Knight in center
    BoardState board1;
    board1.parse_fen("8/8/8/3N4/8/8/8/4k2K w - - 0 1");

    // Position 2: Knight in corner
    BoardState board2;
    board2.parse_fen("N7/8/8/8/8/8/8/4k2K w - - 0 1");

    int eval1 = evaluate(board1);
    int eval2 = evaluate(board2);

    // Center knight should be better than corner knight
    TEST_ASSERT(eval1 > eval2, "Knight in center should evaluate better than knight in corner");

    return 0;
}

// Test king safety evaluation
int test_king_safety() {
    // Castled king (g1) should be safer than central king (e4)
    BoardState castled;
    castled.parse_fen("8/8/8/8/8/8/8/5RK1 w - - 0 1");

    BoardState central;
    central.parse_fen("8/8/8/8/4K3/8/8/8 w - - 0 1");

    int castled_eval = evaluate_position(castled);
    int central_eval = evaluate_position(central);

    // Castled position should have better PST score for king
    TEST_ASSERT(castled_eval > central_eval, "Castled king should have better position than central king");

    return 0;
}

int test_endgame_king_activity() {
    int center_king = evaluate_piece_position(SQUARE_D4, PIECE_KING, true, EvalPhase::ENDGAME);
    int corner_king = evaluate_piece_position(SQUARE_A1, PIECE_KING, true, EvalPhase::ENDGAME);
    TEST_ASSERT(center_king > corner_king, "Endgame king should prefer central squares");

    int opening_center_king = evaluate_piece_position(SQUARE_D4, PIECE_KING, true, EvalPhase::OPENING);
    TEST_ASSERT(center_king > opening_center_king, "Endgame king centralization differs from opening safety");

    return 0;
}

// Test game phase detection
int test_game_phase() {
    // Start position should be full middlegame
    BoardState start;
    start.reset_to_start_position();
    double start_phase = game_phase(start);
    TEST_ASSERT(start_phase > 0.9, "Start position should be phase near 1.0");

    // Endgame (just kings and pawns)
    BoardState endgame;
    endgame.parse_fen("4k3/pppppppp/8/8/8/8/PPPPPPPP/4K3 w - - 0 1");
    double end_phase = game_phase(endgame);
    TEST_ASSERT(end_phase < 0.3, "Pawn endgame should be phase near 0");

    // Bare kings
    BoardState bare;
    bare.parse_fen("4k3/8/8/8/8/8/8/4K3 w - - 0 1");
    double bare_phase = game_phase(bare);
    TEST_ASSERT(bare_phase == 0.0, "Bare kings should be phase 0");

    return 0;
}

int test_tapered_components() {
    BoardState start;
    start.reset_to_start_position();
    EvalComponents start_eval = evaluate_components(start);

    TEST_ASSERT(start_eval.phase == MAX_PHASE, "Start position should have full opening phase");
    TEST_ASSERT(start_eval.score == start_eval.opening, "Full phase uses opening score");

    BoardState bare;
    bare.parse_fen("4k3/8/8/8/8/8/8/4K3 w - - 0 1");
    EvalComponents bare_eval = evaluate_components(bare);

    TEST_ASSERT(bare_eval.phase == 0, "Bare kings should have endgame phase");
    TEST_ASSERT(bare_eval.score == bare_eval.endgame, "Zero phase uses endgame score");

    BoardState mixed;
    mixed.parse_fen("4k3/8/8/8/3N4/8/8/4K2R w - - 0 1");
    EvalComponents mixed_eval = evaluate_components(mixed);
    int expected = ((mixed_eval.opening * mixed_eval.phase) +
                    (mixed_eval.endgame * (MAX_PHASE - mixed_eval.phase))) / MAX_PHASE;
    TEST_ASSERT(mixed_eval.score == expected, "Tapered score interpolates opening and endgame");

    mixed.side_to_move = COLOR_BLACK;
    EvalComponents flipped = evaluate_components(mixed);
    TEST_ASSERT(flipped.score == -mixed_eval.score, "Tapered score flips with side to move");

    return 0;
}

// Test evaluation is deterministic
int test_evaluation_determinism() {
    BoardState board;
    board.reset_to_start_position();

    int eval1 = evaluate(board);
    int eval2 = evaluate(board);
    int eval3 = evaluate(board);

    TEST_ASSERT(eval1 == eval2 && eval2 == eval3, "Evaluation should be deterministic");

    return 0;
}

// Test specific known positions
int test_known_positions() {
    // Scholar's mate position - White has checkmate
    BoardState scholars;
    scholars.parse_fen("rnb1kbnr/pppp1ppp/8/4p3/6Pq/5P2/PPPPP2P/RNBQKBNR b KQkq - 0 3");
    // This is checkmate - Black's eval should be terrible
    int scholars_eval = evaluate(scholars);
    // Just verify it runs without crash and returns a value
    // We don't check specific value since this is material-only eval

    // Fool's mate position - Black has checkmate
    BoardState fools;
    fools.parse_fen("rnbqkbnr/pppp1ppp/4p3/8/6Pq/8/PPPPPP1P/RNBQKBNR w KQkq - 0 2");
    int fools_eval = evaluate(fools);
    // White to move is checkmated - very bad for White

    return 0;
}

int main() {
    std::cout << "=== Evaluation Tests ===" << std::endl;

    // Initialize attack tables (needed for any move generation, just in case)
    init_all_attacks();

    RUN_TEST(test_material_values);
    RUN_TEST(test_equal_material);
    RUN_TEST(test_material_imbalance);
    RUN_TEST(test_pst_access);
    RUN_TEST(test_endgame_pst_access);
    RUN_TEST(test_piece_square_center);
    RUN_TEST(test_evaluate_perspective);
    RUN_TEST(test_evaluate_flips_with_side);
    RUN_TEST(test_positional_factors);
    RUN_TEST(test_king_safety);
    RUN_TEST(test_endgame_king_activity);
    RUN_TEST(test_game_phase);
    RUN_TEST(test_tapered_components);
    RUN_TEST(test_evaluation_determinism);
    RUN_TEST(test_known_positions);

    std::cout << "\nAll tests passed!" << std::endl;
    return 0;
}
