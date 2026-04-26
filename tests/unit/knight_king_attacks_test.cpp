#include "thinmint/movegen/attacks.h"
#include "thinmint/core/bitboard.h"

#include <cassert>
#include <cstdio>

// Simple test framework
#define TEST(name) void test_##name()
#define RUN_TEST(name) \
    do { \
        printf("  Running " #name "... "); \
        test_##name(); \
        printf("PASSED\n"); \
    } while(0)
#define ASSERT(cond) \
    do { \
        if (!(cond)) { \
            printf("FAILED at line %d: %s\n", __LINE__, #cond); \
            return; \
        } \
    } while(0)
#define ASSERT_EQ(a, b) ASSERT((a) == (b))
#define ASSERT_NE(a, b) ASSERT((a) != (b))

using namespace thinmint::core;
using namespace thinmint::movegen;

TEST(knight_attacks_center) {
    // Knight on D4 (square 27) - center of board
    // Should attack: B3, B5, C2, C6, E2, E6, F3, F5
    Square sq = SQUARE_D4;
    Bitboard attacks = knight_attacks(sq);
    
    ASSERT(bb_test(attacks, SQUARE_B3));
    ASSERT(bb_test(attacks, SQUARE_B5));
    ASSERT(bb_test(attacks, SQUARE_C2));
    ASSERT(bb_test(attacks, SQUARE_C6));
    ASSERT(bb_test(attacks, SQUARE_E2));
    ASSERT(bb_test(attacks, SQUARE_E6));
    ASSERT(bb_test(attacks, SQUARE_F3));
    ASSERT(bb_test(attacks, SQUARE_F5));
    ASSERT_EQ(bb_popcount(attacks), 8);
}

TEST(knight_attacks_near_center) {
    // Knight on E4 (square 28)
    // Should attack: C3, C5, D2, D6, F2, F6, G3, G5
    Square sq = SQUARE_E4;
    Bitboard attacks = knight_attacks(sq);
    
    ASSERT(bb_test(attacks, SQUARE_C3));
    ASSERT(bb_test(attacks, SQUARE_C5));
    ASSERT(bb_test(attacks, SQUARE_D2));
    ASSERT(bb_test(attacks, SQUARE_D6));
    ASSERT(bb_test(attacks, SQUARE_F2));
    ASSERT(bb_test(attacks, SQUARE_F6));
    ASSERT(bb_test(attacks, SQUARE_G3));
    ASSERT(bb_test(attacks, SQUARE_G5));
    ASSERT_EQ(bb_popcount(attacks), 8);
}

TEST(knight_attacks_corner_a1) {
    // Knight on A1 (square 0) - corner
    // Should attack: B3, C2 only
    Square sq = SQUARE_A1;
    Bitboard attacks = knight_attacks(sq);
    
    ASSERT(bb_test(attacks, SQUARE_B3));
    ASSERT(bb_test(attacks, SQUARE_C2));
    ASSERT_EQ(bb_popcount(attacks), 2);
}

TEST(knight_attacks_corner_h8) {
    // Knight on H8 (square 63) - corner
    // Should attack: F7, G6 only
    Square sq = SQUARE_H8;
    Bitboard attacks = knight_attacks(sq);
    
    ASSERT(bb_test(attacks, SQUARE_F7));
    ASSERT(bb_test(attacks, SQUARE_G6));
    ASSERT_EQ(bb_popcount(attacks), 2);
}

TEST(knight_attacks_corner_a8) {
    // Knight on A8 (square 56) - corner
    // Should attack: B6, C7 only
    Square sq = SQUARE_A8;
    Bitboard attacks = knight_attacks(sq);
    
    ASSERT(bb_test(attacks, SQUARE_B6));
    ASSERT(bb_test(attacks, SQUARE_C7));
    ASSERT_EQ(bb_popcount(attacks), 2);
}

TEST(knight_attacks_corner_h1) {
    // Knight on H1 (square 7) - corner
    // Should attack: F2, G3 only
    Square sq = SQUARE_H1;
    Bitboard attacks = knight_attacks(sq);
    
    ASSERT(bb_test(attacks, SQUARE_F2));
    ASSERT(bb_test(attacks, SQUARE_G3));
    ASSERT_EQ(bb_popcount(attacks), 2);
}

TEST(knight_attacks_edge_a_file) {
    // Knight on A4 (square 24) - A-file edge
    // Should attack: B2, B6, C3, C5 only (4 attacks)
    Square sq = SQUARE_A4;
    Bitboard attacks = knight_attacks(sq);
    
    ASSERT(bb_test(attacks, SQUARE_B2));
    ASSERT(bb_test(attacks, SQUARE_B6));
    ASSERT(bb_test(attacks, SQUARE_C3));
    ASSERT(bb_test(attacks, SQUARE_C5));
    ASSERT_EQ(bb_popcount(attacks), 4);
}

TEST(knight_attacks_edge_h_file) {
    // Knight on H5 (square 39) - H-file edge
    // Should attack: F4, F6, G3, G7 only (4 attacks)
    Square sq = SQUARE_H5;
    Bitboard attacks = knight_attacks(sq);
    
    ASSERT(bb_test(attacks, SQUARE_F4));
    ASSERT(bb_test(attacks, SQUARE_F6));
    ASSERT(bb_test(attacks, SQUARE_G3));
    ASSERT(bb_test(attacks, SQUARE_G7));
    ASSERT_EQ(bb_popcount(attacks), 4);
}

TEST(knight_attacks_edge_rank_1) {
    // Knight on D1 (square 3) - 1st rank edge
    // Should attack: B2, C3, E3, F2 only (4 attacks)
    Square sq = SQUARE_D1;
    Bitboard attacks = knight_attacks(sq);
    
    ASSERT(bb_test(attacks, SQUARE_B2));
    ASSERT(bb_test(attacks, SQUARE_C3));
    ASSERT(bb_test(attacks, SQUARE_E3));
    ASSERT(bb_test(attacks, SQUARE_F2));
    ASSERT_EQ(bb_popcount(attacks), 4);
}

TEST(knight_attacks_edge_rank_8) {
    // Knight on E8 (square 60) - 8th rank edge
    // Should attack: C7, D6, F6, G7 only (4 attacks)
    Square sq = SQUARE_E8;
    Bitboard attacks = knight_attacks(sq);
    
    ASSERT(bb_test(attacks, SQUARE_C7));
    ASSERT(bb_test(attacks, SQUARE_D6));
    ASSERT(bb_test(attacks, SQUARE_F6));
    ASSERT(bb_test(attacks, SQUARE_G7));
    ASSERT_EQ(bb_popcount(attacks), 4);
}

TEST(knight_attacks_no_wrapping) {
    // Knights should not wrap around the board
    // Knight on B1 - should not attack anything on A-file that's 2 files away
    Square sq = SQUARE_B1;
    Bitboard attacks = knight_attacks(sq);
    
    // Should attack: A3, C3, D2 only
    ASSERT(bb_test(attacks, SQUARE_A3));
    ASSERT(bb_test(attacks, SQUARE_C3));
    ASSERT(bb_test(attacks, SQUARE_D2));
    ASSERT_EQ(bb_popcount(attacks), 3);
    
    // Make sure it doesn't wrap to H-file or similar
    ASSERT(!bb_test(attacks, SQUARE_H2));
    ASSERT(!bb_test(attacks, SQUARE_H3));
}

TEST(king_attacks_center) {
    // King on E4 (square 28) - center
    // Should attack all 8 surrounding squares
    Square sq = SQUARE_E4;
    Bitboard attacks = king_attacks(sq);
    
    ASSERT(bb_test(attacks, SQUARE_D3));
    ASSERT(bb_test(attacks, SQUARE_E3));
    ASSERT(bb_test(attacks, SQUARE_F3));
    ASSERT(bb_test(attacks, SQUARE_D4));
    ASSERT(bb_test(attacks, SQUARE_F4));
    ASSERT(bb_test(attacks, SQUARE_D5));
    ASSERT(bb_test(attacks, SQUARE_E5));
    ASSERT(bb_test(attacks, SQUARE_F5));
    ASSERT_EQ(bb_popcount(attacks), 8);
}

TEST(king_attacks_corner_a1) {
    // King on A1 (square 0) - corner
    // Should attack: A2, B1, B2 only
    Square sq = SQUARE_A1;
    Bitboard attacks = king_attacks(sq);
    
    ASSERT(bb_test(attacks, SQUARE_A2));
    ASSERT(bb_test(attacks, SQUARE_B1));
    ASSERT(bb_test(attacks, SQUARE_B2));
    ASSERT_EQ(bb_popcount(attacks), 3);
}

TEST(king_attacks_corner_h8) {
    // King on H8 (square 63) - corner
    // Should attack: G7, G8, H7 only
    Square sq = SQUARE_H8;
    Bitboard attacks = king_attacks(sq);
    
    ASSERT(bb_test(attacks, SQUARE_G7));
    ASSERT(bb_test(attacks, SQUARE_G8));
    ASSERT(bb_test(attacks, SQUARE_H7));
    ASSERT_EQ(bb_popcount(attacks), 3);
}

TEST(king_attacks_corner_a8) {
    // King on A8 (square 56) - corner
    // Should attack: A7, B7, B8 only
    Square sq = SQUARE_A8;
    Bitboard attacks = king_attacks(sq);
    
    ASSERT(bb_test(attacks, SQUARE_A7));
    ASSERT(bb_test(attacks, SQUARE_B7));
    ASSERT(bb_test(attacks, SQUARE_B8));
    ASSERT_EQ(bb_popcount(attacks), 3);
}

TEST(king_attacks_corner_h1) {
    // King on H1 (square 7) - corner
    // Should attack: G1, G2, H2 only
    Square sq = SQUARE_H1;
    Bitboard attacks = king_attacks(sq);
    
    ASSERT(bb_test(attacks, SQUARE_G1));
    ASSERT(bb_test(attacks, SQUARE_G2));
    ASSERT(bb_test(attacks, SQUARE_H2));
    ASSERT_EQ(bb_popcount(attacks), 3);
}

TEST(king_attacks_edge_a_file) {
    // King on A4 (square 24) - A-file edge
    // Should attack: A3, A5, B3, B4, B5 only (5 attacks)
    Square sq = SQUARE_A4;
    Bitboard attacks = king_attacks(sq);
    
    ASSERT(bb_test(attacks, SQUARE_A3));
    ASSERT(bb_test(attacks, SQUARE_A5));
    ASSERT(bb_test(attacks, SQUARE_B3));
    ASSERT(bb_test(attacks, SQUARE_B4));
    ASSERT(bb_test(attacks, SQUARE_B5));
    ASSERT_EQ(bb_popcount(attacks), 5);
}

TEST(king_attacks_edge_h_file) {
    // King on H5 (square 39) - H-file edge
    // Should attack: G4, G5, G6, H4, H6 only (5 attacks)
    Square sq = SQUARE_H5;
    Bitboard attacks = king_attacks(sq);
    
    ASSERT(bb_test(attacks, SQUARE_G4));
    ASSERT(bb_test(attacks, SQUARE_G5));
    ASSERT(bb_test(attacks, SQUARE_G6));
    ASSERT(bb_test(attacks, SQUARE_H4));
    ASSERT(bb_test(attacks, SQUARE_H6));
    ASSERT_EQ(bb_popcount(attacks), 5);
}

TEST(king_attacks_edge_rank_1) {
    // King on D1 (square 3) - 1st rank edge
    // Should attack: C1, C2, D2, E1, E2 only (5 attacks)
    Square sq = SQUARE_D1;
    Bitboard attacks = king_attacks(sq);
    
    ASSERT(bb_test(attacks, SQUARE_C1));
    ASSERT(bb_test(attacks, SQUARE_C2));
    ASSERT(bb_test(attacks, SQUARE_D2));
    ASSERT(bb_test(attacks, SQUARE_E1));
    ASSERT(bb_test(attacks, SQUARE_E2));
    ASSERT_EQ(bb_popcount(attacks), 5);
}

TEST(king_attacks_edge_rank_8) {
    // King on E8 (square 60) - 8th rank edge
    // Should attack: D7, D8, E7, F7, F8 only (5 attacks)
    Square sq = SQUARE_E8;
    Bitboard attacks = king_attacks(sq);
    
    ASSERT(bb_test(attacks, SQUARE_D7));
    ASSERT(bb_test(attacks, SQUARE_D8));
    ASSERT(bb_test(attacks, SQUARE_E7));
    ASSERT(bb_test(attacks, SQUARE_F7));
    ASSERT(bb_test(attacks, SQUARE_F8));
    ASSERT_EQ(bb_popcount(attacks), 5);
}

TEST(king_attacks_no_wrapping) {
    // King should not wrap around the board
    // King on A1 - already tested in corner test
    // Let's verify a few more edge cases
    
    // King on B1 - should not wrap to A0 or H1
    Square sq = SQUARE_B1;
    Bitboard attacks = king_attacks(sq);
    
    // Should attack: A1, A2, B2, C1, C2
    ASSERT(bb_test(attacks, SQUARE_A1));
    ASSERT(bb_test(attacks, SQUARE_A2));
    ASSERT(bb_test(attacks, SQUARE_B2));
    ASSERT(bb_test(attacks, SQUARE_C1));
    ASSERT(bb_test(attacks, SQUARE_C2));
    ASSERT_EQ(bb_popcount(attacks), 5);
    
    // Should NOT attack anything on H-file
    ASSERT(!bb_test(attacks, SQUARE_H1));
    ASSERT(!bb_test(attacks, SQUARE_H2));
}

TEST(knight_and_king_no_overlap_same_square) {
    // A piece on a square doesn't attack itself
    Square sq = SQUARE_E4;
    Bitboard knight = knight_attacks(sq);
    Bitboard king = king_attacks(sq);
    
    // Neither should contain the original square
    ASSERT(!bb_test(knight, sq));
    ASSERT(!bb_test(king, sq));
}

int main() {
    printf("Running knight and king attack tests...\n");
    
    // Initialize attack tables
    init_knight_attacks();
    init_king_attacks();
    
    RUN_TEST(knight_attacks_center);
    RUN_TEST(knight_attacks_near_center);
    RUN_TEST(knight_attacks_corner_a1);
    RUN_TEST(knight_attacks_corner_h8);
    RUN_TEST(knight_attacks_corner_a8);
    RUN_TEST(knight_attacks_corner_h1);
    RUN_TEST(knight_attacks_edge_a_file);
    RUN_TEST(knight_attacks_edge_h_file);
    RUN_TEST(knight_attacks_edge_rank_1);
    RUN_TEST(knight_attacks_edge_rank_8);
    RUN_TEST(knight_attacks_no_wrapping);
    
    RUN_TEST(king_attacks_center);
    RUN_TEST(king_attacks_corner_a1);
    RUN_TEST(king_attacks_corner_h8);
    RUN_TEST(king_attacks_corner_a8);
    RUN_TEST(king_attacks_corner_h1);
    RUN_TEST(king_attacks_edge_a_file);
    RUN_TEST(king_attacks_edge_h_file);
    RUN_TEST(king_attacks_edge_rank_1);
    RUN_TEST(king_attacks_edge_rank_8);
    RUN_TEST(king_attacks_no_wrapping);
    
    RUN_TEST(knight_and_king_no_overlap_same_square);
    
    printf("\nAll knight and king attack tests PASSED!\n");
    return 0;
}
