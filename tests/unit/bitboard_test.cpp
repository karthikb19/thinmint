#include "thinmint/core/bitboard.h"

#include <cassert>
#include <cstdio>
#include <cstring>

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

using namespace thinmint::core;

TEST(bitboard_constants) {
    ASSERT_EQ(BB_EMPTY, 0ULL);
    ASSERT_EQ(BB_ALL, 0xFFFFFFFFFFFFFFFFULL);
    ASSERT_EQ(BB_FILE_A, 0x0101010101010101ULL);
    ASSERT_EQ(BB_FILE_H, 0x8080808080808080ULL);
    ASSERT_EQ(BB_RANK_1, 0x00000000000000FFULL);
    ASSERT_EQ(BB_RANK_8, 0xFF00000000000000ULL);
}

TEST(bitboard_square_constants) {
    // Check individual square bitboards
    ASSERT_EQ(BB_SQUARE_A1, 1ULL << 0);
    ASSERT_EQ(BB_SQUARE_H1, 1ULL << 7);
    ASSERT_EQ(BB_SQUARE_A8, 1ULL << 56);
    ASSERT_EQ(BB_SQUARE_H8, 1ULL << 63);
    ASSERT_EQ(BB_SQUARE_E4, 1ULL << 28);
    
    // Check array indexing
    ASSERT_EQ(BB_SQUARE[0], BB_SQUARE_A1);
    ASSERT_EQ(BB_SQUARE[7], BB_SQUARE_H1);
    ASSERT_EQ(BB_SQUARE[60], BB_SQUARE_E8);
}

TEST(bb_set_clear_test) {
    Bitboard bb = BB_EMPTY;
    
    // Test setting bits
    bb = bb_set(bb, SQUARE_E4);
    ASSERT_EQ(bb, BB_SQUARE_E4);
    
    bb = bb_set(bb, SQUARE_D5);
    ASSERT_EQ(bb, BB_SQUARE_E4 | BB_SQUARE_D5);
    
    // Test clearing bits
    bb = bb_clear(bb, SQUARE_E4);
    ASSERT_EQ(bb, BB_SQUARE_D5);
    
    bb = bb_clear(bb, SQUARE_D5);
    ASSERT_EQ(bb, BB_EMPTY);
}

TEST(bb_test) {
    Bitboard bb = BB_SQUARE_A1 | BB_SQUARE_H8 | BB_SQUARE_E4;
    
    ASSERT(bb_test(bb, SQUARE_A1));
    ASSERT(bb_test(bb, SQUARE_H8));
    ASSERT(bb_test(bb, SQUARE_E4));
    ASSERT(!bb_test(bb, SQUARE_B1));
    ASSERT(!bb_test(bb, SQUARE_E5));
}

TEST(bb_toggle) {
    Bitboard bb = BB_SQUARE_A1;
    
    bb = bb_toggle(bb, SQUARE_H8);
    ASSERT_EQ(bb, BB_SQUARE_A1 | BB_SQUARE_H8);
    
    bb = bb_toggle(bb, SQUARE_A1);
    ASSERT_EQ(bb, BB_SQUARE_H8);
}

TEST(bb_empty_any) {
    ASSERT(bb_is_empty(BB_EMPTY));
    ASSERT(!bb_is_empty(BB_SQUARE_E4));
    ASSERT(!bb_is_empty(BB_ALL));
    
    ASSERT(!bb_any(BB_EMPTY));
    ASSERT(bb_any(BB_SQUARE_E4));
    ASSERT(bb_any(BB_ALL));
}

TEST(bb_popcount) {
    ASSERT_EQ(bb_popcount(BB_EMPTY), 0);
    ASSERT_EQ(bb_popcount(BB_SQUARE_A1), 1);
    ASSERT_EQ(bb_popcount(BB_SQUARE_A1 | BB_SQUARE_H8), 2);
    ASSERT_EQ(bb_popcount(BB_RANK_1), 8);
    ASSERT_EQ(bb_popcount(BB_FILE_A), 8);
    ASSERT_EQ(bb_popcount(BB_ALL), 64);
}

TEST(bb_lsb_index) {
    ASSERT_EQ(bb_lsb_index(BB_SQUARE_A1), 0);
    ASSERT_EQ(bb_lsb_index(BB_SQUARE_H1), 7);
    ASSERT_EQ(bb_lsb_index(BB_SQUARE_A8), 56);
    ASSERT_EQ(bb_lsb_index(BB_SQUARE_H8), 63);
    ASSERT_EQ(bb_lsb_index(BB_SQUARE_E4), 28);
}

TEST(bb_lsb) {
    ASSERT_EQ(bb_lsb(BB_SQUARE_A1), BB_SQUARE_A1);
    ASSERT_EQ(bb_lsb(BB_SQUARE_A1 | BB_SQUARE_H8), BB_SQUARE_A1);
    ASSERT_EQ(bb_lsb(BB_SQUARE_E4 | BB_SQUARE_D5), BB_SQUARE_E4);
}

TEST(bb_clear_lsb) {
    Bitboard bb = BB_SQUARE_A1 | BB_SQUARE_H8;
    bb = bb_clear_lsb(bb);
    ASSERT_EQ(bb, BB_SQUARE_H8);
    
    bb = bb_clear_lsb(bb);
    ASSERT_EQ(bb, BB_EMPTY);
}

TEST(bb_msb_index) {
    ASSERT_EQ(bb_msb_index(BB_SQUARE_A1), 0);
    ASSERT_EQ(bb_msb_index(BB_SQUARE_H1), 7);
    ASSERT_EQ(bb_msb_index(BB_SQUARE_A8), 56);
    ASSERT_EQ(bb_msb_index(BB_SQUARE_H8), 63);
    ASSERT_EQ(bb_msb_index(BB_SQUARE_A1 | BB_SQUARE_H8), 63);
}

TEST(bb_shifts) {
    // North shift
    ASSERT_EQ(bb_shift_north(BB_RANK_1), BB_RANK_2);
    ASSERT_EQ(bb_shift_north(BB_RANK_7), BB_RANK_8);
    ASSERT_EQ(bb_shift_north(BB_RANK_8), BB_EMPTY);
    
    // South shift
    ASSERT_EQ(bb_shift_south(BB_RANK_8), BB_RANK_7);
    ASSERT_EQ(bb_shift_south(BB_RANK_2), BB_RANK_1);
    ASSERT_EQ(bb_shift_south(BB_RANK_1), BB_EMPTY);
    
    // East shift (no wrapping)
    ASSERT_EQ(bb_shift_east(BB_FILE_A), BB_FILE_B);
    ASSERT_EQ(bb_shift_east(BB_FILE_G), BB_FILE_H);
    ASSERT_EQ(bb_shift_east(BB_FILE_H), BB_EMPTY);
    
    // West shift (no wrapping)
    ASSERT_EQ(bb_shift_west(BB_FILE_H), BB_FILE_G);
    ASSERT_EQ(bb_shift_west(BB_FILE_B), BB_FILE_A);
    ASSERT_EQ(bb_shift_west(BB_FILE_A), BB_EMPTY);
}

TEST(bb_first_square) {
    ASSERT_EQ(bb_first_square(BB_EMPTY), SQUARE_NONE);
    ASSERT_EQ(bb_first_square(BB_SQUARE_A1), SQUARE_A1);
    ASSERT_EQ(bb_first_square(BB_SQUARE_E4), SQUARE_E4);
    ASSERT_EQ(bb_first_square(BB_SQUARE_H8 | BB_SQUARE_A1), SQUARE_A1);
}

TEST(bb_pop_square) {
    Bitboard bb = BB_SQUARE_A1 | BB_SQUARE_E4 | BB_SQUARE_H8;
    
    Square sq1 = bb_pop_square(bb);
    ASSERT_EQ(sq1, SQUARE_A1);
    
    Square sq2 = bb_pop_square(bb);
    ASSERT_EQ(sq2, SQUARE_E4);
    
    Square sq3 = bb_pop_square(bb);
    ASSERT_EQ(sq3, SQUARE_H8);
    
    ASSERT(bb_is_empty(bb));
}

TEST(bb_iteration) {
    Bitboard bb = BB_SQUARE_A1 | BB_SQUARE_H1;
    int count = 0;
    
    while (bb) {
        Square sq = bb_pop_square(bb);
        ASSERT(sq == SQUARE_A1 || sq == SQUARE_H1);
        count++;
    }
    
    ASSERT_EQ(count, 2);
}

TEST(file_rank_arrays) {
    // Test file array
    for (int f = 0; f < 8; ++f) {
        Bitboard expected = 1ULL << f;
        for (int r = 1; r < 8; ++r) {
            expected |= 1ULL << (r * 8 + f);
        }
        ASSERT_EQ(BB_FILE[f], expected);
    }
    
    // Test rank array
    for (int r = 0; r < 8; ++r) {
        ASSERT_EQ(BB_RANK[r], 0xFFULL << (r * 8));
    }
}

TEST(diagonal_shifts) {
    // Northeast: A1 -> B2
    ASSERT_EQ(bb_shift_northeast(BB_SQUARE_A1), BB_SQUARE_B2);
    // H1 should wrap to nothing (file A)
    ASSERT_EQ(bb_shift_northeast(BB_SQUARE_H1), BB_EMPTY);
    
    // Northwest: H1 -> G2
    ASSERT_EQ(bb_shift_northwest(BB_SQUARE_H1), BB_SQUARE_G2);
    // A1 should wrap to nothing (file H)
    ASSERT_EQ(bb_shift_northwest(BB_SQUARE_A1), BB_EMPTY);
    
    // Southeast: A8 -> B7
    ASSERT_EQ(bb_shift_southeast(BB_SQUARE_A8), BB_SQUARE_B7);
    // H8 should wrap to nothing
    ASSERT_EQ(bb_shift_southeast(BB_SQUARE_H8), BB_EMPTY);
    
    // Southwest: H8 -> G7
    ASSERT_EQ(bb_shift_southwest(BB_SQUARE_H8), BB_SQUARE_G7);
    // A8 should wrap to nothing
    ASSERT_EQ(bb_shift_southwest(BB_SQUARE_A8), BB_EMPTY);
}

int main() {
    printf("Running bitboard tests...\n");
    
    RUN_TEST(bitboard_constants);
    RUN_TEST(bitboard_square_constants);
    RUN_TEST(bb_set_clear_test);
    RUN_TEST(bb_test);
    RUN_TEST(bb_toggle);
    RUN_TEST(bb_empty_any);
    RUN_TEST(bb_popcount);
    RUN_TEST(bb_lsb_index);
    RUN_TEST(bb_lsb);
    RUN_TEST(bb_clear_lsb);
    RUN_TEST(bb_msb_index);
    RUN_TEST(bb_shifts);
    RUN_TEST(bb_first_square);
    RUN_TEST(bb_pop_square);
    RUN_TEST(bb_iteration);
    RUN_TEST(file_rank_arrays);
    RUN_TEST(diagonal_shifts);
    
    printf("\nAll bitboard tests PASSED!\n");
    return 0;
}
