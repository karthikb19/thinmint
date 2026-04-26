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

TEST(pawn_attacks_white_center) {
    // White pawn on E4 (square 28) attacks D5 and F5
    Square sq = SQUARE_E4;
    Bitboard attacks = pawn_attacks(sq, COLOR_WHITE);
    ASSERT(bb_test(attacks, SQUARE_D5));
    ASSERT(bb_test(attacks, SQUARE_F5));
    ASSERT_EQ(bb_popcount(attacks), 2);
}

TEST(pawn_attacks_black_center) {
    // Black pawn on E5 (square 36) attacks D4 and F4
    Square sq = SQUARE_E5;
    Bitboard attacks = pawn_attacks(sq, COLOR_BLACK);
    ASSERT(bb_test(attacks, SQUARE_D4));
    ASSERT(bb_test(attacks, SQUARE_F4));
    ASSERT_EQ(bb_popcount(attacks), 2);
}

TEST(pawn_attacks_white_edge_a_file) {
    // White pawn on A1 attacks B2 only (no wrap to H2)
    Square sq = SQUARE_A1;
    Bitboard attacks = pawn_attacks(sq, COLOR_WHITE);
    ASSERT(bb_test(attacks, SQUARE_B2));
    ASSERT_EQ(bb_popcount(attacks), 1);
    
    // White pawn on A4 attacks B5 only
    sq = SQUARE_A4;
    attacks = pawn_attacks(sq, COLOR_WHITE);
    ASSERT(bb_test(attacks, SQUARE_B5));
    ASSERT_EQ(bb_popcount(attacks), 1);
}

TEST(pawn_attacks_white_edge_h_file) {
    // White pawn on H1 attacks G2 only (no wrap)
    Square sq = SQUARE_H1;
    Bitboard attacks = pawn_attacks(sq, COLOR_WHITE);
    ASSERT(bb_test(attacks, SQUARE_G2));
    ASSERT_EQ(bb_popcount(attacks), 1);
    
    // White pawn on H4 attacks G5 only
    sq = SQUARE_H4;
    attacks = pawn_attacks(sq, COLOR_WHITE);
    ASSERT(bb_test(attacks, SQUARE_G5));
    ASSERT_EQ(bb_popcount(attacks), 1);
}

TEST(pawn_attacks_black_edge_a_file) {
    // Black pawn on A8 attacks B7 only
    Square sq = SQUARE_A8;
    Bitboard attacks = pawn_attacks(sq, COLOR_BLACK);
    ASSERT(bb_test(attacks, SQUARE_B7));
    ASSERT_EQ(bb_popcount(attacks), 1);
    
    // Black pawn on A5 attacks B4 only
    sq = SQUARE_A5;
    attacks = pawn_attacks(sq, COLOR_BLACK);
    ASSERT(bb_test(attacks, SQUARE_B4));
    ASSERT_EQ(bb_popcount(attacks), 1);
}

TEST(pawn_attacks_black_edge_h_file) {
    // Black pawn on H8 attacks G7 only
    Square sq = SQUARE_H8;
    Bitboard attacks = pawn_attacks(sq, COLOR_BLACK);
    ASSERT(bb_test(attacks, SQUARE_G7));
    ASSERT_EQ(bb_popcount(attacks), 1);
    
    // Black pawn on H5 attacks G4 only
    sq = SQUARE_H5;
    attacks = pawn_attacks(sq, COLOR_BLACK);
    ASSERT(bb_test(attacks, SQUARE_G4));
    ASSERT_EQ(bb_popcount(attacks), 1);
}

TEST(pawn_attacks_white_corner) {
    // White pawn on H8 - can't attack (already on top rank)
    Square sq = SQUARE_H8;
    Bitboard attacks = pawn_attacks(sq, COLOR_WHITE);
    ASSERT_EQ(attacks, BB_EMPTY);
    
    // White pawn on A8 - can't attack
    sq = SQUARE_A8;
    attacks = pawn_attacks(sq, COLOR_WHITE);
    ASSERT_EQ(attacks, BB_EMPTY);
}

TEST(pawn_attacks_black_corner) {
    // Black pawn on H1 - can't attack (already on bottom rank)
    Square sq = SQUARE_H1;
    Bitboard attacks = pawn_attacks(sq, COLOR_BLACK);
    ASSERT_EQ(attacks, BB_EMPTY);
    
    // Black pawn on A1 - can't attack
    sq = SQUARE_A1;
    attacks = pawn_attacks(sq, COLOR_BLACK);
    ASSERT_EQ(attacks, BB_EMPTY);
}

TEST(all_pawn_attacks_white) {
    // Two white pawns on E4 and D4
    Bitboard pawns = BB_SQUARE_E4 | BB_SQUARE_D4;
    Bitboard attacks = all_pawn_attacks(pawns, COLOR_WHITE);
    // E4 attacks D5, F5; D4 attacks C5, E5
    // Combined: C5, D5, E5, F5
    ASSERT(bb_test(attacks, SQUARE_C5));
    ASSERT(bb_test(attacks, SQUARE_D5));
    ASSERT(bb_test(attacks, SQUARE_E5));
    ASSERT(bb_test(attacks, SQUARE_F5));
    ASSERT_EQ(bb_popcount(attacks), 4);
}

TEST(all_pawn_attacks_black) {
    // Two black pawns on E5 and D5
    Bitboard pawns = BB_SQUARE_E5 | BB_SQUARE_D5;
    Bitboard attacks = all_pawn_attacks(pawns, COLOR_BLACK);
    // E5 attacks D4, F4; D5 attacks C4, E4
    // Combined: C4, D4, E4, F4
    ASSERT(bb_test(attacks, SQUARE_C4));
    ASSERT(bb_test(attacks, SQUARE_D4));
    ASSERT(bb_test(attacks, SQUARE_E4));
    ASSERT(bb_test(attacks, SQUARE_F4));
    ASSERT_EQ(bb_popcount(attacks), 4);
}

TEST(pawn_pushes_white) {
    // White pawn on E2, empty board
    Bitboard pawns = BB_SQUARE_E2;
    Bitboard occupancy = BB_EMPTY;
    Bitboard pushes = pawn_pushes(pawns, occupancy, COLOR_WHITE);
    ASSERT(bb_test(pushes, SQUARE_E3));
    ASSERT_EQ(bb_popcount(pushes), 1);
    
    // With blocker on E3 - no push
    occupancy = BB_SQUARE_E3;
    pushes = pawn_pushes(pawns, occupancy, COLOR_WHITE);
    ASSERT_EQ(pushes, BB_EMPTY);
}

TEST(pawn_pushes_black) {
    // Black pawn on E7, empty board
    Bitboard pawns = BB_SQUARE_E7;
    Bitboard occupancy = BB_EMPTY;
    Bitboard pushes = pawn_pushes(pawns, occupancy, COLOR_BLACK);
    ASSERT(bb_test(pushes, SQUARE_E6));
    ASSERT_EQ(bb_popcount(pushes), 1);
    
    // With blocker - no push
    occupancy = BB_SQUARE_E6;
    pushes = pawn_pushes(pawns, occupancy, COLOR_BLACK);
    ASSERT_EQ(pushes, BB_EMPTY);
}

TEST(pawn_double_pushes_white) {
    // White pawn on E2, empty board - can double push to E4
    Bitboard pawns = BB_SQUARE_E2;
    Bitboard occupancy = BB_EMPTY;
    Bitboard pushes = pawn_double_pushes(pawns, occupancy, COLOR_WHITE);
    ASSERT(bb_test(pushes, SQUARE_E4));
    ASSERT_EQ(bb_popcount(pushes), 1);
    
    // With blocker on E3 - no double push
    occupancy = BB_SQUARE_E3;
    pushes = pawn_double_pushes(pawns, occupancy, COLOR_WHITE);
    ASSERT_EQ(pushes, BB_EMPTY);
    
    // With blocker on E4 only - no double push
    occupancy = BB_SQUARE_E4;
    pushes = pawn_double_pushes(pawns, occupancy, COLOR_WHITE);
    ASSERT_EQ(pushes, BB_EMPTY);
}

TEST(pawn_double_pushes_black) {
    // Black pawn on E7, empty board - can double push to E5
    Bitboard pawns = BB_SQUARE_E7;
    Bitboard occupancy = BB_EMPTY;
    Bitboard pushes = pawn_double_pushes(pawns, occupancy, COLOR_BLACK);
    ASSERT(bb_test(pushes, SQUARE_E5));
    ASSERT_EQ(bb_popcount(pushes), 1);
}

TEST(pawn_double_pushes_not_from_start_rank) {
    // White pawn not on rank 2 - no double push
    Bitboard pawns = BB_SQUARE_E3;
    Bitboard occupancy = BB_EMPTY;
    Bitboard pushes = pawn_double_pushes(pawns, occupancy, COLOR_WHITE);
    ASSERT_EQ(pushes, BB_EMPTY);
    
    // Black pawn not on rank 7 - no double push
    pawns = BB_SQUARE_E6;
    pushes = pawn_double_pushes(pawns, occupancy, COLOR_BLACK);
    ASSERT_EQ(pushes, BB_EMPTY);
}

TEST(pawn_promotion_pushes_white) {
    // White pawn on E7, empty board - promotion push to E8
    Bitboard pawns = BB_SQUARE_E7;
    Bitboard occupancy = BB_EMPTY;
    Bitboard pushes = pawn_promotion_pushes(pawns, occupancy, COLOR_WHITE);
    ASSERT(bb_test(pushes, SQUARE_E8));
    ASSERT_EQ(bb_popcount(pushes), 1);
    
    // Pawn not on 7th rank - no promotion push
    pawns = BB_SQUARE_E6;
    pushes = pawn_promotion_pushes(pawns, occupancy, COLOR_WHITE);
    ASSERT_EQ(pushes, BB_EMPTY);
}

TEST(pawn_promotion_pushes_black) {
    // Black pawn on E2, empty board - promotion push to E1
    Bitboard pawns = BB_SQUARE_E2;
    Bitboard occupancy = BB_EMPTY;
    Bitboard pushes = pawn_promotion_pushes(pawns, occupancy, COLOR_BLACK);
    ASSERT(bb_test(pushes, SQUARE_E1));
    ASSERT_EQ(bb_popcount(pushes), 1);
}

TEST(pawn_captures_white) {
    // White pawn on E4, enemy on D5 and F5
    Bitboard pawns = BB_SQUARE_E4;
    Bitboard enemy = BB_SQUARE_D5 | BB_SQUARE_F5;
    Bitboard captures = pawn_captures(pawns, enemy, COLOR_WHITE);
    ASSERT(bb_test(captures, SQUARE_D5));
    ASSERT(bb_test(captures, SQUARE_F5));
    ASSERT_EQ(bb_popcount(captures), 2);
    
    // No enemy pieces - no captures
    enemy = BB_EMPTY;
    captures = pawn_captures(pawns, enemy, COLOR_WHITE);
    ASSERT_EQ(captures, BB_EMPTY);
}

TEST(pawn_captures_black) {
    // Black pawn on E5, enemy on D4 and F4
    Bitboard pawns = BB_SQUARE_E5;
    Bitboard enemy = BB_SQUARE_D4 | BB_SQUARE_F4;
    Bitboard captures = pawn_captures(pawns, enemy, COLOR_BLACK);
    ASSERT(bb_test(captures, SQUARE_D4));
    ASSERT(bb_test(captures, SQUARE_F4));
    ASSERT_EQ(bb_popcount(captures), 2);
}

TEST(pawn_promotion_captures_white) {
    // White pawn on E7, enemy on D8 and F8
    Bitboard pawns = BB_SQUARE_E7;
    Bitboard enemy = BB_SQUARE_D8 | BB_SQUARE_F8;
    Bitboard captures = pawn_promotion_captures(pawns, enemy, COLOR_WHITE);
    ASSERT(bb_test(captures, SQUARE_D8));
    ASSERT(bb_test(captures, SQUARE_F8));
    ASSERT_EQ(bb_popcount(captures), 2);
}

TEST(pawn_promotion_captures_black) {
    // Black pawn on E2, enemy on D1 and F1
    Bitboard pawns = BB_SQUARE_E2;
    Bitboard enemy = BB_SQUARE_D1 | BB_SQUARE_F1;
    Bitboard captures = pawn_promotion_captures(pawns, enemy, COLOR_BLACK);
    ASSERT(bb_test(captures, SQUARE_D1));
    ASSERT(bb_test(captures, SQUARE_F1));
    ASSERT_EQ(bb_popcount(captures), 2);
}

TEST(ep_square_from_double_push_white) {
    // White double push to E4 -> EP square is E3
    Square ep = ep_square_from_double_push(SQUARE_E4, COLOR_WHITE);
    ASSERT_EQ(ep, SQUARE_E3);
    
    // Not a double push destination (not rank 4)
    ep = ep_square_from_double_push(SQUARE_E5, COLOR_WHITE);
    ASSERT_EQ(ep, SQUARE_NONE);
}

TEST(ep_square_from_double_push_black) {
    // Black double push to E5 -> EP square is E6
    Square ep = ep_square_from_double_push(SQUARE_E5, COLOR_BLACK);
    ASSERT_EQ(ep, SQUARE_E6);
    
    // Not a double push destination (not rank 5)
    ep = ep_square_from_double_push(SQUARE_E4, COLOR_BLACK);
    ASSERT_EQ(ep, SQUARE_NONE);
}

TEST(pawn_attacks_no_wrapping_rank) {
    // White pawns on rank 7 should not attack into rank 9 (non-existent)
    // This is implicitly handled by the bitboard shifting
    Bitboard pawns = BB_RANK_7;
    Bitboard attacks = all_pawn_attacks(pawns, COLOR_WHITE);
    // All attacks should be on rank 8
    ASSERT_EQ(attacks & ~BB_RANK_8, BB_EMPTY);
    ASSERT_NE(attacks, BB_EMPTY);  // Should have some attacks
}

int main() {
    printf("Running pawn attack tests...\n");
    
    // Initialize the pawn attack tables
    init_pawn_attacks();
    
    RUN_TEST(pawn_attacks_white_center);
    RUN_TEST(pawn_attacks_black_center);
    RUN_TEST(pawn_attacks_white_edge_a_file);
    RUN_TEST(pawn_attacks_white_edge_h_file);
    RUN_TEST(pawn_attacks_black_edge_a_file);
    RUN_TEST(pawn_attacks_black_edge_h_file);
    RUN_TEST(pawn_attacks_white_corner);
    RUN_TEST(pawn_attacks_black_corner);
    RUN_TEST(all_pawn_attacks_white);
    RUN_TEST(all_pawn_attacks_black);
    RUN_TEST(pawn_pushes_white);
    RUN_TEST(pawn_pushes_black);
    RUN_TEST(pawn_double_pushes_white);
    RUN_TEST(pawn_double_pushes_black);
    RUN_TEST(pawn_double_pushes_not_from_start_rank);
    RUN_TEST(pawn_promotion_pushes_white);
    RUN_TEST(pawn_promotion_pushes_black);
    RUN_TEST(pawn_captures_white);
    RUN_TEST(pawn_captures_black);
    RUN_TEST(pawn_promotion_captures_white);
    RUN_TEST(pawn_promotion_captures_black);
    RUN_TEST(ep_square_from_double_push_white);
    RUN_TEST(ep_square_from_double_push_black);
    RUN_TEST(pawn_attacks_no_wrapping_rank);
    
    printf("\nAll pawn attack tests PASSED!\n");
    return 0;
}
