#include <iostream>
#include <sstream>
#include <cstdlib>

#include "thinmint/search/search.h"
#include "thinmint/board/board.h"
#include "thinmint/movegen/attacks.h"

using namespace thinmint;
using namespace thinmint::core;
using namespace thinmint::board;
using namespace thinmint::search;
using namespace thinmint::movegen;

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

// Test iterative deepening basic functionality
void test_iterative_deepening_basic() {
    std::cout << "\nTest: Iterative deepening basic" << std::endl;

    BoardState board;
    board.reset_to_start_position();

    // Search to depth 3 using iterative deepening
    SearchResult result = iterative_deepening(board, 3);

    check(result.best_move != MOVE_NONE, "Returns a best move");
    check(result.depth >= 1, "Depth at least 1");
    check(result.nodes > 0, "Searched nodes");
}

// Test iterative deepening at depth 1
void test_iterative_deepening_depth_1() {
    std::cout << "\nTest: Iterative deepening depth 1" << std::endl;

    BoardState board;
    board.reset_to_start_position();

    SearchResult result = iterative_deepening(board, 1);

    check(result.best_move != MOVE_NONE, "Depth 1 returns a move");
    check(result.depth == 1, "Result depth is 1");
    check(result.nodes >= 20, "Depth 1 searched all moves");
}

// Test iterative deepening finds mate-in-one
void test_iterative_deepening_mate_in_one() {
    std::cout << "\nTest: Iterative deepening mate-in-one" << std::endl;

    // Position: White queen on a1 can deliver mate on a8
    BoardState board = board_from_fen("7k/8/6K1/8/8/8/8/Q7 w - - 0 1");

    SearchResult result = iterative_deepening(board, 2);

    check(result.mate_found, "Finds mate");
    check(result.score > MATE_THRESHOLD, "Mate score is positive");
}

// Test iterative deepening with terminal position
void test_iterative_deepening_terminal() {
    std::cout << "\nTest: Iterative deepening terminal position" << std::endl;

    // Checkmate position
    BoardState board = board_from_fen("7k/7Q/6K1/8/8/8/8/8 b - - 0 1");

    SearchResult result = iterative_deepening(board, 3);

    check(result.best_move == MOVE_NONE, "Terminal position returns no move");
    check(is_mate_score(result.score), "Terminal score is mate score");
}

// Test iterative deepening at depth 0
void test_iterative_deepening_depth_zero() {
    std::cout << "\nTest: Iterative deepening depth 0" << std::endl;

    BoardState board;
    board.reset_to_start_position();

    SearchResult result = iterative_deepening(board, 0);

    check(result.best_move == MOVE_NONE, "Depth 0 returns no move");
    check(result.nodes == 1, "Depth 0 evaluates 1 node");
}

// Test iterative deepening preserves board state
void test_iterative_deepening_state_preservation() {
    std::cout << "\nTest: Iterative deepening state preservation" << std::endl;

    BoardState board;
    board.reset_to_start_position();

    // Save original state
    BoardState original = board;

    // Run iterative deepening
    SearchResult result = iterative_deepening(board, 3);

    // Verify board is restored
    check(board.side_to_move == original.side_to_move,
          "Side to move preserved");
    check(board.castling_rights == original.castling_rights,
          "Castling rights preserved");
    check(board.all_occupancy == original.all_occupancy,
          "Occupancy preserved");
}

// Test iterative deepening deeper search
void test_iterative_deepening_progressive() {
    std::cout << "\nTest: Iterative deepening progressive" << std::endl;

    BoardState board;
    board.reset_to_start_position();

    SearchStats stats;

    // Search progressively deeper
    SearchResult d1 = iterative_deepening(board, 1, &stats);
    SearchResult d2 = iterative_deepening(board, 2, &stats);

    // Both should find valid moves
    check(d1.best_move != MOVE_NONE, "Depth 1 finds move");
    check(d2.best_move != MOVE_NONE, "Depth 2 finds move");

    // Deeper search should search more nodes
    check(stats.nodes_searched > d2.nodes, "Stats accumulate nodes");
}

// Test search result statistics
void test_search_result_stats() {
    std::cout << "\nTest: Search result statistics" << std::endl;

    BoardState board;
    board.reset_to_start_position();

    SearchResult result = iterative_deepening(board, 2);

    check(result.nodes > 0, "Nodes reported");
    check(result.depth > 0, "Depth reported");

    // Score should be reasonable for start position
    check(result.score > -500 && result.score < 500,
          "Score is reasonable for start position");
}

int main() {
    std::cout << "=== Iterative Deepening Tests ===" << std::endl;

    // Initialize attack tables
    init_all_attacks();

    test_iterative_deepening_basic();
    test_iterative_deepening_depth_1();
    test_iterative_deepening_mate_in_one();
    test_iterative_deepening_terminal();
    test_iterative_deepening_depth_zero();
    test_iterative_deepening_state_preservation();
    test_iterative_deepening_progressive();
    test_search_result_stats();

    std::cout << "\n=== Test Summary ===" << std::endl;
    std::cout << "Passed: " << tests_passed << std::endl;
    std::cout << "Failed: " << tests_failed << std::endl;

    return tests_failed > 0 ? 1 : 0;
}
