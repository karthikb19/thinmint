#include "thinmint/perft/perft.h"

#include <cstdint>
#include <iostream>
#include <string>

#include "thinmint/board/board.h"
#include "thinmint/movegen/attacks.h"

using namespace thinmint;
using namespace thinmint::board;
using namespace thinmint::movegen;

// Simple test framework
#define TEST(name) void test_##name()
#define RUN_TEST(name) run_test(#name, test_##name)

int tests_run = 0;
int tests_passed = 0;

void run_test(const char* name, void (*test_func)()) {
    tests_run++;
    try {
        test_func();
        tests_passed++;
        std::cout << "  [PASS] " << name << "\n";
    } catch (const std::exception& e) {
        std::cout << "  [FAIL] " << name << ": " << e.what() << "\n";
    }
}

#define ASSERT_EQ(expected, actual) \
    if ((expected) != (actual)) { \
        throw std::runtime_error(std::string("Expected ") + std::to_string(expected) + \
                                 " but got " + std::to_string(actual)); \
    }

#define ASSERT_TRUE(expr) \
    if (!(expr)) { \
        throw std::runtime_error("Assertion failed: " #expr); \
    }

// Test perft on starting position depth 0-4
TEST(startpos_perft_depth_0) {
    BoardState board;
    board.reset_to_start_position();
    ASSERT_EQ(1ULL, thinmint::perft::perft(board, 0));
}

TEST(startpos_perft_depth_1) {
    BoardState board;
    board.reset_to_start_position();
    ASSERT_EQ(20ULL, thinmint::perft::perft(board, 1));
}

TEST(startpos_perft_depth_2) {
    BoardState board;
    board.reset_to_start_position();
    ASSERT_EQ(400ULL, thinmint::perft::perft(board, 2));
}

TEST(startpos_perft_depth_3) {
    BoardState board;
    board.reset_to_start_position();
    ASSERT_EQ(8902ULL, thinmint::perft::perft(board, 3));
}

TEST(startpos_perft_depth_4) {
    BoardState board;
    board.reset_to_start_position();
    ASSERT_EQ(197281ULL, thinmint::perft::perft(board, 4));
}

// Test perft via FEN helper
TEST(perft_fen_helper) {
    uint64_t nodes = thinmint::perft::perft_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 3);
    ASSERT_EQ(8902ULL, nodes);
}

// Test perft fixtures are defined correctly
TEST(perft_fixtures_exist) {
    ASSERT_TRUE(thinmint::perft::PERFT_FIXTURE_COUNT > 0);
    ASSERT_TRUE(thinmint::perft::PERFT_FIXTURES[0].expected_nodes != nullptr);
    ASSERT_EQ(20ULL, thinmint::perft::PERFT_FIXTURES[0].expected_nodes[1]);  // Start pos depth 1
}

// Test Kiwipete position (Position 2)
TEST(kiwipete_perft_depth_1) {
    BoardState board;
    ASSERT_TRUE(board.parse_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"));
    ASSERT_EQ(48ULL, thinmint::perft::perft(board, 1));
}

TEST(kiwipete_perft_depth_2) {
    BoardState board;
    ASSERT_TRUE(board.parse_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"));
    ASSERT_EQ(2039ULL, thinmint::perft::perft(board, 2));
}

TEST(kiwipete_perft_depth_3) {
    BoardState board;
    ASSERT_TRUE(board.parse_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"));
    ASSERT_EQ(97862ULL, thinmint::perft::perft(board, 3));
}

// Test Position 3 - promotions and pins
TEST(position3_perft_depth_1) {
    BoardState board;
    ASSERT_TRUE(board.parse_fen("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1"));
    ASSERT_EQ(14ULL, thinmint::perft::perft(board, 1));
}

TEST(position3_perft_depth_2) {
    BoardState board;
    ASSERT_TRUE(board.parse_fen("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1"));
    ASSERT_EQ(191ULL, thinmint::perft::perft(board, 2));
}

TEST(position3_perft_depth_3) {
    BoardState board;
    ASSERT_TRUE(board.parse_fen("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1"));
    ASSERT_EQ(2812ULL, thinmint::perft::perft(board, 3));
}

// Test position with EP capture
TEST(en_passant_perft) {
    // Position after 1. e4 e5 2. d4 exd4
    BoardState board;
    ASSERT_TRUE(board.parse_fen("rnbqkbnr/pppp1ppp/8/8/3Pp3/8/PPP2PPP/RNBQKBNR b KQkq d3 0 2"));
    ASSERT_TRUE(board.en_passant_square != SQUARE_NONE);
    uint64_t nodes = thinmint::perft::perft(board, 1);
    // Just verify we get some moves including EP capture
    ASSERT_TRUE(nodes > 20);
}

// Test position with castling
TEST(castling_perft) {
    BoardState board;
    ASSERT_TRUE(board.parse_fen("r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1"));
    uint64_t nodes = thinmint::perft::perft(board, 1);
    // Position has rooks (not knights) on a1/h1, so 25 moves total
    // (16 pawn + 5 rook + 2 king + 2 castling = 25)
    ASSERT_EQ(25ULL, nodes);
}

// Test fixture structure
TEST(fixture_startpos_depths) {
    const thinmint::perft::PerftFixture& fixture = thinmint::perft::PERFT_FIXTURES[0];
    ASSERT_TRUE(std::string(fixture.name) == "Start Position");
    ASSERT_EQ(1ULL, fixture.expected_nodes[0]);
    ASSERT_EQ(20ULL, fixture.expected_nodes[1]);
    ASSERT_EQ(400ULL, fixture.expected_nodes[2]);
    ASSERT_EQ(8902ULL, fixture.expected_nodes[3]);
    ASSERT_EQ(197281ULL, fixture.expected_nodes[4]);
}

// Test that perft can run on all fixtures to depth 2 with exact expected values
TEST(all_fixtures_depth_2) {
    for (size_t i = 0; i < thinmint::perft::PERFT_FIXTURE_COUNT; ++i) {
        const thinmint::perft::PerftFixture& fixture = thinmint::perft::PERFT_FIXTURES[i];
        BoardState board;
        ASSERT_TRUE(board.parse_fen(fixture.fen));
        uint64_t nodes = thinmint::perft::perft(board, 2);
        ASSERT_EQ(fixture.expected_nodes[2], nodes);
    }
}

// Test perft divide (just verify it doesn't crash and returns correct total)
TEST(perft_divide_basic) {
    BoardState board;
    board.reset_to_start_position();
    // Suppress output by passing false
    uint64_t total = thinmint::perft::perft_divide(board, 2, false);
    ASSERT_EQ(400ULL, total);
}

int main() {
    std::cout << "Running perft tests...\n\n";

    // Initialize attack tables
    init_all_attacks();

    RUN_TEST(startpos_perft_depth_0);
    RUN_TEST(startpos_perft_depth_1);
    RUN_TEST(startpos_perft_depth_2);
    RUN_TEST(startpos_perft_depth_3);
    RUN_TEST(startpos_perft_depth_4);
    RUN_TEST(perft_fen_helper);
    RUN_TEST(perft_fixtures_exist);
    RUN_TEST(kiwipete_perft_depth_1);
    RUN_TEST(kiwipete_perft_depth_2);
    RUN_TEST(kiwipete_perft_depth_3);
    RUN_TEST(position3_perft_depth_1);
    RUN_TEST(position3_perft_depth_2);
    RUN_TEST(position3_perft_depth_3);
    RUN_TEST(en_passant_perft);
    RUN_TEST(castling_perft);
    RUN_TEST(fixture_startpos_depths);
    RUN_TEST(all_fixtures_depth_2);
    RUN_TEST(perft_divide_basic);

    std::cout << "\n==============================\n";
    std::cout << "Results: " << tests_passed << "/" << tests_run << " tests passed\n";

    return (tests_passed == tests_run) ? 0 : 1;
}
