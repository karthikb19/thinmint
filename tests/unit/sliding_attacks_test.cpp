#include <iostream>
#include <cassert>
#include <cstdlib>

#include "thinmint/movegen/attacks.h"
#include "thinmint/core/bitboard.h"
#include "thinmint/core/square.h"

using namespace thinmint::core;
using namespace thinmint::movegen;

// Test bishop attacks from center square (D4 = square 27)
// D4 is at file 3, rank 3 (0-indexed)
void test_bishop_attacks_center_no_blockers() {
    Square sq = SQUARE_D4;
    Bitboard occupancy = BB_EMPTY;
    Bitboard attacks = bishop_attacks(sq, occupancy);

    // Bishop on D4 can attack along 4 diagonals:
    // Northeast: E5, F6, G7, H8 (squares 36, 45, 54, 63)
    // Northwest: C5, B6, A7 (squares 34, 41, 48)
    // Southeast: E3, F2, G1 (squares 20, 13, 6)
    // Southwest: C3, B2, A1 (squares 18, 9, 0)

    Bitboard expected = BB_EMPTY;
    expected |= bb_from_square(SQUARE_E5);
    expected |= bb_from_square(SQUARE_F6);
    expected |= bb_from_square(SQUARE_G7);
    expected |= bb_from_square(SQUARE_H8);
    expected |= bb_from_square(SQUARE_C5);
    expected |= bb_from_square(SQUARE_B6);
    expected |= bb_from_square(SQUARE_A7);
    expected |= bb_from_square(SQUARE_E3);
    expected |= bb_from_square(SQUARE_F2);
    expected |= bb_from_square(SQUARE_G1);
    expected |= bb_from_square(SQUARE_C3);
    expected |= bb_from_square(SQUARE_B2);
    expected |= bb_from_square(SQUARE_A1);

    if (attacks != expected) {
        std::cerr << "FAIL: test_bishop_attacks_center_no_blockers" << std::endl;
        std::cerr << "  Expected: 0x" << std::hex << expected << std::dec << std::endl;
        std::cerr << "  Got:      0x" << std::hex << attacks << std::dec << std::endl;
        std::exit(1);
    }
    std::cout << "PASS: test_bishop_attacks_center_no_blockers" << std::endl;
}

// Test bishop attacks from corner (A1 = square 0)
void test_bishop_attacks_corner() {
    Square sq = SQUARE_A1;
    Bitboard occupancy = BB_EMPTY;
    Bitboard attacks = bishop_attacks(sq, occupancy);

    // Bishop on A1 can only attack northeast: B2, C3, D4, E5, F6, G7, H8
    Bitboard expected = BB_EMPTY;
    expected |= bb_from_square(SQUARE_B2);
    expected |= bb_from_square(SQUARE_C3);
    expected |= bb_from_square(SQUARE_D4);
    expected |= bb_from_square(SQUARE_E5);
    expected |= bb_from_square(SQUARE_F6);
    expected |= bb_from_square(SQUARE_G7);
    expected |= bb_from_square(SQUARE_H8);

    if (attacks != expected) {
        std::cerr << "FAIL: test_bishop_attacks_corner" << std::endl;
        std::cerr << "  Expected: 0x" << std::hex << expected << std::dec << std::endl;
        std::cerr << "  Got:      0x" << std::hex << attacks << std::dec << std::endl;
        std::exit(1);
    }
    std::cout << "PASS: test_bishop_attacks_corner" << std::endl;
}

// Test bishop attacks with blockers
void test_bishop_attacks_with_blockers() {
    Square sq = SQUARE_D4;
    // Put a blocker on F6 (northeast diagonal)
    Bitboard occupancy = bb_from_square(SQUARE_F6);
    Bitboard attacks = bishop_attacks(sq, occupancy);

    // Bishop on D4 with blocker on F6:
    // Northeast: E5, F6 (stops at F6, cannot reach G7, H8)
    // Other directions unchanged
    Bitboard expected = BB_EMPTY;
    expected |= bb_from_square(SQUARE_E5);
    expected |= bb_from_square(SQUARE_F6);  // Includes the blocker square
    expected |= bb_from_square(SQUARE_C5);
    expected |= bb_from_square(SQUARE_B6);
    expected |= bb_from_square(SQUARE_A7);
    expected |= bb_from_square(SQUARE_E3);
    expected |= bb_from_square(SQUARE_F2);
    expected |= bb_from_square(SQUARE_G1);
    expected |= bb_from_square(SQUARE_C3);
    expected |= bb_from_square(SQUARE_B2);
    expected |= bb_from_square(SQUARE_A1);

    if (attacks != expected) {
        std::cerr << "FAIL: test_bishop_attacks_with_blockers" << std::endl;
        std::cerr << "  Expected: 0x" << std::hex << expected << std::dec << std::endl;
        std::cerr << "  Got:      0x" << std::hex << attacks << std::dec << std::endl;
        std::exit(1);
    }
    std::cout << "PASS: test_bishop_attacks_with_blockers" << std::endl;
}

// Test rook attacks from center (D4)
void test_rook_attacks_center_no_blockers() {
    Square sq = SQUARE_D4;
    Bitboard occupancy = BB_EMPTY;
    Bitboard attacks = rook_attacks(sq, occupancy);

    // Rook on D4 attacks orthogonally:
    // North: D5, D6, D7, D8 (squares 35, 43, 51, 59)
    // South: D3, D2, D1 (squares 19, 11, 3)
    // East: E4, F4, G4, H4 (squares 28, 29, 30, 31)
    // West: C4, B4, A4 (squares 26, 25, 24)

    Bitboard expected = BB_EMPTY;
    expected |= bb_from_square(SQUARE_D5);
    expected |= bb_from_square(SQUARE_D6);
    expected |= bb_from_square(SQUARE_D7);
    expected |= bb_from_square(SQUARE_D8);
    expected |= bb_from_square(SQUARE_D3);
    expected |= bb_from_square(SQUARE_D2);
    expected |= bb_from_square(SQUARE_D1);
    expected |= bb_from_square(SQUARE_E4);
    expected |= bb_from_square(SQUARE_F4);
    expected |= bb_from_square(SQUARE_G4);
    expected |= bb_from_square(SQUARE_H4);
    expected |= bb_from_square(SQUARE_C4);
    expected |= bb_from_square(SQUARE_B4);
    expected |= bb_from_square(SQUARE_A4);

    if (attacks != expected) {
        std::cerr << "FAIL: test_rook_attacks_center_no_blockers" << std::endl;
        std::cerr << "  Expected: 0x" << std::hex << expected << std::dec << std::endl;
        std::cerr << "  Got:      0x" << std::hex << attacks << std::dec << std::endl;
        std::exit(1);
    }
    std::cout << "PASS: test_rook_attacks_center_no_blockers" << std::endl;
}

// Test rook attacks from edge (A1)
void test_rook_attacks_edge() {
    Square sq = SQUARE_A1;
    Bitboard occupancy = BB_EMPTY;
    Bitboard attacks = rook_attacks(sq, occupancy);

    // Rook on A1 attacks:
    // North: A2, A3, A4, A5, A6, A7, A8
    // East: B1, C1, D1, E1, F1, G1, H1
    Bitboard expected = BB_EMPTY;
    // North
    expected |= bb_from_square(SQUARE_A2);
    expected |= bb_from_square(SQUARE_A3);
    expected |= bb_from_square(SQUARE_A4);
    expected |= bb_from_square(SQUARE_A5);
    expected |= bb_from_square(SQUARE_A6);
    expected |= bb_from_square(SQUARE_A7);
    expected |= bb_from_square(SQUARE_A8);
    // East
    expected |= bb_from_square(SQUARE_B1);
    expected |= bb_from_square(SQUARE_C1);
    expected |= bb_from_square(SQUARE_D1);
    expected |= bb_from_square(SQUARE_E1);
    expected |= bb_from_square(SQUARE_F1);
    expected |= bb_from_square(SQUARE_G1);
    expected |= bb_from_square(SQUARE_H1);

    if (attacks != expected) {
        std::cerr << "FAIL: test_rook_attacks_edge" << std::endl;
        std::cerr << "  Expected: 0x" << std::hex << expected << std::dec << std::endl;
        std::cerr << "  Got:      0x" << std::hex << attacks << std::dec << std::endl;
        std::exit(1);
    }
    std::cout << "PASS: test_rook_attacks_edge" << std::endl;
}

// Test rook attacks with multiple blockers
void test_rook_attacks_with_blockers() {
    Square sq = SQUARE_D4;
    // Blockers on D6 (north) and F4 (east)
    Bitboard occupancy = bb_from_square(SQUARE_D6) | bb_from_square(SQUARE_F4);
    Bitboard attacks = rook_attacks(sq, occupancy);

    // Rook on D4 with blockers:
    // North: D5, D6 (blocked at D6)
    // South: D3, D2, D1 (unchanged)
    // East: E4, F4 (blocked at F4)
    // West: C4, B4, A4 (unchanged)
    Bitboard expected = BB_EMPTY;
    expected |= bb_from_square(SQUARE_D5);
    expected |= bb_from_square(SQUARE_D6);  // Includes blocker
    expected |= bb_from_square(SQUARE_D3);
    expected |= bb_from_square(SQUARE_D2);
    expected |= bb_from_square(SQUARE_D1);
    expected |= bb_from_square(SQUARE_E4);
    expected |= bb_from_square(SQUARE_F4);  // Includes blocker
    expected |= bb_from_square(SQUARE_C4);
    expected |= bb_from_square(SQUARE_B4);
    expected |= bb_from_square(SQUARE_A4);

    if (attacks != expected) {
        std::cerr << "FAIL: test_rook_attacks_with_blockers" << std::endl;
        std::cerr << "  Expected: 0x" << std::hex << expected << std::dec << std::endl;
        std::cerr << "  Got:      0x" << std::hex << attacks << std::dec << std::endl;
        std::exit(1);
    }
    std::cout << "PASS: test_rook_attacks_with_blockers" << std::endl;
}

// Test queen attacks (combines bishop and rook)
void test_queen_attacks() {
    Square sq = SQUARE_D4;
    Bitboard occupancy = BB_EMPTY;
    Bitboard attacks = queen_attacks(sq, occupancy);

    // Queen combines bishop and rook attacks
    Bitboard bishop = bishop_attacks(sq, occupancy);
    Bitboard rook = rook_attacks(sq, occupancy);
    Bitboard expected = bishop | rook;

    if (attacks != expected) {
        std::cerr << "FAIL: test_queen_attacks" << std::endl;
        std::cerr << "  Expected: 0x" << std::hex << expected << std::dec << std::endl;
        std::cerr << "  Got:      0x" << std::hex << attacks << std::dec << std::endl;
        std::exit(1);
    }
    std::cout << "PASS: test_queen_attacks" << std::endl;
}

// Test queen attacks with blockers
void test_queen_attacks_with_blockers() {
    Square sq = SQUARE_E4;
    // Blocker on diagonal and orthogonal
    Bitboard occupancy = bb_from_square(SQUARE_F5) | bb_from_square(SQUARE_E6);
    Bitboard attacks = queen_attacks(sq, occupancy);

    Bitboard bishop = bishop_attacks(sq, occupancy);
    Bitboard rook = rook_attacks(sq, occupancy);
    Bitboard expected = bishop | rook;

    if (attacks != expected) {
        std::cerr << "FAIL: test_queen_attacks_with_blockers" << std::endl;
        std::cerr << "  Expected: 0x" << std::hex << expected << std::dec << std::endl;
        std::cerr << "  Got:      0x" << std::hex << attacks << std::dec << std::endl;
        std::exit(1);
    }
    std::cout << "PASS: test_queen_attacks_with_blockers" << std::endl;
}

// Test sliding attacks from various edge squares
void test_sliding_attacks_edge_cases() {
    // Test bishop on H8 (corner)
    Bitboard bishop_h8 = bishop_attacks(SQUARE_H8, BB_EMPTY);
    // Should only attack southwest: G7, F6, E5, D4, C3, B2, A1
    Bitboard expected_bishop_h8 = BB_EMPTY;
    expected_bishop_h8 |= bb_from_square(SQUARE_G7);
    expected_bishop_h8 |= bb_from_square(SQUARE_F6);
    expected_bishop_h8 |= bb_from_square(SQUARE_E5);
    expected_bishop_h8 |= bb_from_square(SQUARE_D4);
    expected_bishop_h8 |= bb_from_square(SQUARE_C3);
    expected_bishop_h8 |= bb_from_square(SQUARE_B2);
    expected_bishop_h8 |= bb_from_square(SQUARE_A1);

    if (bishop_h8 != expected_bishop_h8) {
        std::cerr << "FAIL: test_sliding_attacks_edge_cases - bishop H8" << std::endl;
        std::exit(1);
    }

    // Test rook on H8
    Bitboard rook_h8 = rook_attacks(SQUARE_H8, BB_EMPTY);
    // Should attack west along rank 8 and south along H-file
    Bitboard expected_rook_h8 = BB_EMPTY;
    // West
    expected_rook_h8 |= bb_from_square(SQUARE_G8);
    expected_rook_h8 |= bb_from_square(SQUARE_F8);
    expected_rook_h8 |= bb_from_square(SQUARE_E8);
    expected_rook_h8 |= bb_from_square(SQUARE_D8);
    expected_rook_h8 |= bb_from_square(SQUARE_C8);
    expected_rook_h8 |= bb_from_square(SQUARE_B8);
    expected_rook_h8 |= bb_from_square(SQUARE_A8);
    // South
    expected_rook_h8 |= bb_from_square(SQUARE_H7);
    expected_rook_h8 |= bb_from_square(SQUARE_H6);
    expected_rook_h8 |= bb_from_square(SQUARE_H5);
    expected_rook_h8 |= bb_from_square(SQUARE_H4);
    expected_rook_h8 |= bb_from_square(SQUARE_H3);
    expected_rook_h8 |= bb_from_square(SQUARE_H2);
    expected_rook_h8 |= bb_from_square(SQUARE_H1);

    if (rook_h8 != expected_rook_h8) {
        std::cerr << "FAIL: test_sliding_attacks_edge_cases - rook H8" << std::endl;
        std::exit(1);
    }

    std::cout << "PASS: test_sliding_attacks_edge_cases" << std::endl;
}

int main() {
    std::cout << "Running sliding attacks tests..." << std::endl;

    test_bishop_attacks_center_no_blockers();
    test_bishop_attacks_corner();
    test_bishop_attacks_with_blockers();
    test_rook_attacks_center_no_blockers();
    test_rook_attacks_edge();
    test_rook_attacks_with_blockers();
    test_queen_attacks();
    test_queen_attacks_with_blockers();
    test_sliding_attacks_edge_cases();

    std::cout << "All sliding attacks tests passed!" << std::endl;
    return 0;
}
