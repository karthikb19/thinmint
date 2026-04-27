#include <iostream>
#include <cstdlib>
#include <cmath>

#include "thinmint/search/search.h"
#include "thinmint/board/board.h"
#include "thinmint/board/makemove.h"
#include "thinmint/board/unmakemove.h"
#include "thinmint/eval/eval.h"
#include "thinmint/movegen/movegen.h"
#include "thinmint/movegen/attacks.h"

using namespace thinmint;
using namespace thinmint::core;
using namespace thinmint::board;
using namespace thinmint::movegen;
using namespace thinmint::eval;
using namespace thinmint::search;

// Test counter
int tests_passed = 0;
int tests_failed = 0;

void check(bool condition, const char* test_name) {
    if (condition) {
        std::cout << "  [PASS] " << test_name << std::endl;
        tests_passed++;
    } else {
        std::cout << "  [FAIL] " << test_name << std::endl;
        tests_failed++;
    }
}

// Test that search doesn't crash on start position
void test_search_start_position() {
    std::cout << "\nTest: Search start position" << std::endl;

    BoardState board;
    board.reset_to_start_position();

    // Search to depth 1
    SearchResult result = search_root(board, 1);

    check(result.best_move != MOVE_NONE, "Depth 1 returns a move");
    check(result.nodes > 0, "Depth 1 searched at least 1 node");
    check(result.depth == 1, "Depth is correct");
}

// Test depth 2 search on start position
void test_search_depth_2() {
    std::cout << "\nTest: Search start position depth 2" << std::endl;

    BoardState board;
    board.reset_to_start_position();

    SearchResult result = search_root(board, 2);

    check(result.best_move != MOVE_NONE, "Depth 2 returns a move");
    check(result.nodes >= 20, "Depth 2 searched at least 20 nodes");
    check(!result.mate_found, "No mate found in start position");
}

// Test terminal position detection - checkmate
void test_checkmate_detection() {
    std::cout << "\nTest: Checkmate detection" << std::endl;

    // Checkmate position: Black king on h8, White queen on h7, White king on g6
    // Queen delivers check from h7, protected by king on g6
    // Black king cannot capture (would be moving into check by White king)
    // Black king has no other escape squares (h7 is controlled by queen, g8/g7 by queen)
    BoardState board = board_from_fen("7k/7Q/6K1/8/8/8/8/8 b - - 0 1");

    // Black to move is checkmated
    bool terminal = is_terminal(board);
    check(terminal, "Checkmate position is terminal");

    int score = get_terminal_score(board, 0);
    check(score < -MATE_THRESHOLD, "Checkmate score is highly negative");
    check(is_mate_score(score), "Checkmate score is recognized as mate");
}

// Test stalemate detection
void test_stalemate_detection() {
    std::cout << "\nTest: Stalemate detection" << std::endl;

    // Classic stalemate position: Black king on h1, White queen on f2, White king on f3
    // Black to move has no legal moves and is not in check
    BoardState board = board_from_fen("8/8/8/8/8/5K2/5Q2/7k b - - 0 1");

    bool terminal = is_terminal(board);
    check(terminal, "Stalemate position is terminal");

    int score = get_terminal_score(board, 0);
    check(score == 0, "Stalemate score is zero (draw)");
    check(!is_mate_score(score), "Stalemate score is not a mate score");
}

// Test mate score conversion
void test_mate_score_conversion() {
    std::cout << "\nTest: Mate score conversion" << std::endl;

    // Test that mate scores are recognized
    check(is_mate_score(MATE_SCORE), "MATE_SCORE is a mate score");
    check(is_mate_score(-MATE_SCORE), "Negative MATE_SCORE is a mate score");
    check(!is_mate_score(1000), "Regular score is not mate score");
    check(!is_mate_score(0), "Zero is not mate score");

    // Test conversion - positive mate scores mean we're mating
    int mate_in_3 = MATE_SCORE - 5;  // Found mate in 5 plies from root
    check(score_to_mate_in(mate_in_3) > 0, "Positive mate score means we win");
}

// Test that search finds mate-in-1
void test_mate_in_one() {
    std::cout << "\nTest: Mate-in-one detection" << std::endl;

    // Position: Black king on h8, White queen on a1, White king on g6
    // White to move can deliver Qa1-a8# or Qa1-g7# (both mate)
    // Note: a1 = square 0, a8 = square 56, g7 = square 54
    BoardState board = board_from_fen("7k/8/6K1/8/8/8/8/Q7 w - - 0 1");

    SearchResult result = search_root(board, 2);

    check(result.mate_found, "Mate-in-one is found");
    check(result.score > MATE_THRESHOLD, "Mate score is positive for side delivering mate");

    // The best move should originate from a1 (square 0) and deliver checkmate
    // Both Qa1-a8# and Qa1-g7# are valid mates
    Move best = result.best_move;
    Square from = from_square(best);
    Square to = to_square(best);
    check(from == SQUARE_A1, "Mate move originates from a1");
    // Either a8 or g7 is a valid checkmate destination
    bool valid_mate_square = (to == SQUARE_A8) || (to == SQUARE_G7);
    check(valid_mate_square, "Mate move goes to a checkmating square (a8 or g7)");
}

// Test alpha-beta produces same result as full minimax
void test_alpha_beta_consistency() {
    std::cout << "\nTest: Alpha-beta consistency" << std::endl;

    // Use a simple tactical position
    BoardState board = board_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    // Search with different depths, verify it doesn't crash
    SearchResult d1 = search_root(board, 1);
    SearchResult d2 = search_root(board, 2);

    check(d1.best_move != MOVE_NONE, "Depth 1 finds a move");
    check(d2.best_move != MOVE_NONE, "Depth 2 finds a move");
    // We don't check exact scores because they may vary with depth,
    // but both should produce valid results
}

// Test that make/unmake is consistent during search
void test_search_state_consistency() {
    std::cout << "\nTest: Search state consistency" << std::endl;

    BoardState board;
    board.reset_to_start_position();

    // Save original state
    BoardState original = board;

    // Perform a deep search
    SearchResult result = search_root(board, 3);

    // Board should be restored to original state
    // Check key properties (can't compare bitboards directly due to potential
    // differences in internal representation)
    check(board.side_to_move == original.side_to_move,
          "Side to move restored after search");
    check(board.castling_rights == original.castling_rights,
          "Castling rights restored after search");
    check(board.en_passant_square == original.en_passant_square,
          "EP square restored after search");
    check(board.all_occupancy == original.all_occupancy,
          "Occupancy restored after search");
}

// Test simple endgame position
void test_simple_endgame() {
    std::cout << "\nTest: Simple endgame evaluation" << std::endl;

    // K+Q vs K - White to move should find mate quickly
    BoardState board = board_from_fen("8/8/8/8/3k4/8/8/3QK3 w - - 0 1");

    SearchResult result = search_root(board, 4);

    check(result.best_move != MOVE_NONE, "Finds move in KQ vs K");
    // Should find some path toward mate
    check(result.score > 0, "Winning position has positive score");
}

// Test that depth 0 returns evaluation
void test_depth_zero() {
    std::cout << "\nTest: Depth zero search" << std::endl;

    BoardState board;
    board.reset_to_start_position();

    SearchResult result = search_root(board, 0);

    check(result.best_move == MOVE_NONE, "Depth 0 returns no move");
    check(result.nodes == 1, "Depth 0 evaluates just 1 node");
    check(result.score == evaluate(board), "Depth 0 returns static eval");
}

// Test multiple searches don't corrupt state
void test_repeated_searches() {
    std::cout << "\nTest: Repeated searches" << std::endl;

    BoardState board;
    board.reset_to_start_position();

    // Perform multiple searches
    for (int i = 1; i <= 3; ++i) {
        SearchResult result = search_root(board, i);
        check(result.best_move != MOVE_NONE, "Search iteration finds move");
        // Verify board is still valid
        check(board.is_valid(), "Board remains valid after search");
    }
}

// Test negamax returns expected values for simple positions
void test_negamax_scores() {
    std::cout << "\nTest: Negamax score consistency" << std::endl;

    // Symmetric position should have near-zero score
    BoardState start = board_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    SearchResult result = search_root(start, 2);

    // At low depth, score should be close to 0 for symmetric position
    check(std::abs(result.score) < 100, "Start position has near-zero score at depth 2");
}

// Test search handles positions with few pieces
void test_limited_material() {
    std::cout << "\nTest: Limited material positions" << std::endl;

    // King vs King - should be draw
    BoardState kvk = board_from_fen("4k3/8/8/8/8/8/8/4K3 w - - 0 1");
    SearchResult result = search_root(kvk, 2);
    check(result.best_move != MOVE_NONE || is_terminal(kvk), "King vs King handled");

    // King and pawn vs king
    BoardState kp_k = board_from_fen("4k3/8/8/8/8/4P3/8/4K3 w - - 0 1");
    result = search_root(kp_k, 2);
    check(result.best_move != MOVE_NONE, "KP vs K finds move");
}

// Test basic quiescence search functionality
void test_quiescence_basic() {
    std::cout << "\nTest: Quiescence search basic" << std::endl;

    BoardState board = board_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    SearchStats stats;
    stats.reset();
    int score = quiescence(board, -INF_SCORE, INF_SCORE, stats);

    check(score != INF_SCORE && score != -INF_SCORE, "Quiescence returns a finite score");
    check(stats.nodes_searched > 0, "Quiescence searched at least one node");
}

// Test that quiescence finds a winning capture
void test_quiescence_finds_winning_capture() {
    std::cout << "\nTest: Quiescence finds winning capture" << std::endl;

    // White knight on f3 attacks Black queen on d4
    // Black queen is undefended
    // Static eval is roughly equal, but capturing the queen wins massively
    BoardState board = board_from_fen("4k3/8/8/3q4/8/5N2/8/4K3 w - - 0 1");

    SearchStats stats;
    stats.reset();
    int q_score = quiescence(board, -INF_SCORE, INF_SCORE, stats);

    // After capturing the queen, White should be massively ahead
    check(q_score > 500, "Quiescence finds winning queen capture (score > 500)");
}

// Test that quiescence resolves a simple capture sequence
void test_quiescence_capture_sequence() {
    std::cout << "\nTest: Quiescence resolves capture sequence" << std::endl;

    // White queen on d4 attacks Black rook on d8
    // Black rook is defended by Black queen on d5
    // White queen capture Qxd8 is met by Qxd8
    // Net: White loses queen for rook (bad)
    BoardState board = board_from_fen("3r4/8/8/3q4/3Q4/8/8/4K2k w - - 0 1");

    SearchStats stats;
    stats.reset();
    int q_score = quiescence(board, -INF_SCORE, INF_SCORE, stats);

    // The quiescence search should see that Qxd8 loses the queen
    // and prefer not to capture, so score should not be hugely positive
    // (it might be slightly negative due to position)
    check(q_score < 400, "Quiescence avoids bad capture sequence (score < 400)");
}

// Test quiescence when in check: must find evasions
void test_quiescence_in_check() {
    std::cout << "\nTest: Quiescence in check" << std::endl;

    // Black rook on a1 gives check to White king on a2
    // White can block with Bb1 or move king
    BoardState board = board_from_fen("r7/8/8/8/8/8/K7/8 w - - 0 1");

    SearchStats stats;
    stats.reset();
    int q_score = quiescence(board, -INF_SCORE, INF_SCORE, stats);

    // Should not be mated - there are legal evasions (Ka1, Ka3, etc.)
    // Since no capture gets out of check, q-search with in-check expansion
    // should find a non-capture evasion and return a finite score
    check(q_score > -MATE_THRESHOLD, "Quiescence finds evasion when in check");
}

// Test that quiescence stand-pat works when no captures are available
void test_quiescence_stand_pat() {
    std::cout << "\nTest: Quiescence stand-pat" << std::endl;

    // King vs King - no captures possible
    BoardState board = board_from_fen("4k3/8/8/8/8/8/8/4K3 w - - 0 1");

    SearchStats stats;
    stats.reset();
    int q_score = quiescence(board, -INF_SCORE, INF_SCORE, stats);

    // Should return near-zero score (drawish)
    check(std::abs(q_score) < 50, "Stand-pat in dead drawn position (score near 0)");
}

int main() {
    std::cout << "=== Search Tests ===" << std::endl;

    // Initialize attack tables
    init_all_attacks();

    test_search_start_position();
    test_search_depth_2();
    test_checkmate_detection();
    test_stalemate_detection();
    test_mate_score_conversion();
    test_mate_in_one();
    test_alpha_beta_consistency();
    test_search_state_consistency();
    test_simple_endgame();
    test_depth_zero();
    test_repeated_searches();
    test_negamax_scores();
    test_limited_material();
    test_quiescence_basic();
    test_quiescence_finds_winning_capture();
    test_quiescence_capture_sequence();
    test_quiescence_in_check();
    test_quiescence_stand_pat();

    std::cout << "\n=== Test Summary ===" << std::endl;
    std::cout << "Passed: " << tests_passed << std::endl;
    std::cout << "Failed: " << tests_failed << std::endl;

    return tests_failed > 0 ? 1 : 0;
}
