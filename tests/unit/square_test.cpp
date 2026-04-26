#include <cstdlib>
#include <cstring>
#include <iostream>

#include "thinmint/core/square.h"

using namespace thinmint::core;

// Helper to check condition and report
#define CHECK(cond, msg) \
    do { \
        if (!(cond)) { \
            std::cerr << "FAIL: " << msg << " at line " << __LINE__ << "\n"; \
            return EXIT_FAILURE; \
        } \
    } while(0)

int test_square_constants() {
    // Test that corners map correctly
    CHECK(SQUARE_A1 == 0, "A1 should be 0");
    CHECK(SQUARE_H1 == 7, "H1 should be 7");
    CHECK(SQUARE_A8 == 56, "A8 should be 56");
    CHECK(SQUARE_H8 == 63, "H8 should be 63");

    // Test some middle squares
    CHECK(SQUARE_E1 == 4, "E1 should be 4");
    CHECK(SQUARE_E8 == 60, "E8 should be 60");
    CHECK(SQUARE_A4 == 24, "A4 should be 24");
    CHECK(SQUARE_H5 == 39, "H5 should be 39");

    return EXIT_SUCCESS;
}

int test_file_rank_extraction() {
    // Test extracting file and rank from squares
    CHECK(file_of(SQUARE_A1) == FILE_A, "file_of(A1) should be FILE_A");
    CHECK(file_of(SQUARE_H1) == FILE_H, "file_of(H1) should be FILE_H");
    CHECK(file_of(SQUARE_E4) == FILE_E, "file_of(E4) should be FILE_E");

    CHECK(rank_of(SQUARE_A1) == RANK_1, "rank_of(A1) should be RANK_1");
    CHECK(rank_of(SQUARE_A8) == RANK_8, "rank_of(A8) should be RANK_8");
    CHECK(rank_of(SQUARE_E4) == RANK_4, "rank_of(E4) should be RANK_4");

    return EXIT_SUCCESS;
}

int test_make_square() {
    // Test creating squares from file and rank
    CHECK(make_square(FILE_A, RANK_1) == SQUARE_A1, "make_square(A, 1) should be A1");
    CHECK(make_square(FILE_H, RANK_1) == SQUARE_H1, "make_square(H, 1) should be H1");
    CHECK(make_square(FILE_A, RANK_8) == SQUARE_A8, "make_square(A, 8) should be A8");
    CHECK(make_square(FILE_H, RANK_8) == SQUARE_H8, "make_square(H, 8) should be H8");
    CHECK(make_square(FILE_E, RANK_4) == SQUARE_E4, "make_square(E, 4) should be E4");

    // Verify round-trip: make_square(file_of(sq), rank_of(sq)) == sq
    for (int sq = 0; sq < SQUARE_COUNT; ++sq) {
        Square s = static_cast<Square>(sq);
        CHECK(make_square(file_of(s), rank_of(s)) == s,
              "make_square round-trip failed");
    }

    return EXIT_SUCCESS;
}

int test_square_to_algebraic() {
    char buffer[16];

    // Test corners
    square_to_algebraic(SQUARE_A1, buffer);
    CHECK(std::strcmp(buffer, "a1") == 0, "A1 should convert to 'a1'");

    square_to_algebraic(SQUARE_H1, buffer);
    CHECK(std::strcmp(buffer, "h1") == 0, "H1 should convert to 'h1'");

    square_to_algebraic(SQUARE_A8, buffer);
    CHECK(std::strcmp(buffer, "a8") == 0, "A8 should convert to 'a8'");

    square_to_algebraic(SQUARE_H8, buffer);
    CHECK(std::strcmp(buffer, "h8") == 0, "H8 should convert to 'h8'");

    square_to_algebraic(SQUARE_E4, buffer);
    CHECK(std::strcmp(buffer, "e4") == 0, "E4 should convert to 'e4'");

    // Test invalid square
    square_to_algebraic(SQUARE_NONE, buffer);
    CHECK(std::strcmp(buffer, "-") == 0, "SQUARE_NONE should convert to '-'");

    return EXIT_SUCCESS;
}

int test_algebraic_to_square() {
    // Test valid inputs (lowercase)
    CHECK(algebraic_to_square("a1") == SQUARE_A1, "'a1' should parse to A1");
    CHECK(algebraic_to_square("h1") == SQUARE_H1, "'h1' should parse to H1");
    CHECK(algebraic_to_square("a8") == SQUARE_A8, "'a8' should parse to A8");
    CHECK(algebraic_to_square("h8") == SQUARE_H8, "'h8' should parse to H8");
    CHECK(algebraic_to_square("e4") == SQUARE_E4, "'e4' should parse to E4");
    CHECK(algebraic_to_square("d5") == SQUARE_D5, "'d5' should parse to D5");

    // Test uppercase (should work)
    CHECK(algebraic_to_square("E4") == SQUARE_E4, "'E4' should parse to E4");
    CHECK(algebraic_to_square("A1") == SQUARE_A1, "'A1' should parse to A1");

    // Test invalid inputs
    CHECK(algebraic_to_square("") == SQUARE_NONE, "empty string should return SQUARE_NONE");
    CHECK(algebraic_to_square("i1") == SQUARE_NONE, "'i1' should return SQUARE_NONE (invalid file)");
    CHECK(algebraic_to_square("a9") == SQUARE_NONE, "'a9' should return SQUARE_NONE (invalid rank)");
    CHECK(algebraic_to_square("a") == SQUARE_NONE, "'a' should return SQUARE_NONE (too short)");
    CHECK(algebraic_to_square("x1") == SQUARE_NONE, "'x1' should return SQUARE_NONE (invalid file)");

    return EXIT_SUCCESS;
}

int test_round_trip_conversion() {
    // Test that all squares round-trip correctly
    for (int sq = 0; sq < SQUARE_COUNT; ++sq) {
        Square original = static_cast<Square>(sq);
        char buffer[16];
        square_to_algebraic(original, buffer);
        Square parsed = algebraic_to_square(buffer);
        CHECK(original == parsed, "round-trip conversion failed");
    }

    return EXIT_SUCCESS;
}

int test_is_valid_square() {
    CHECK(is_valid_square(SQUARE_A1), "A1 should be valid");
    CHECK(is_valid_square(SQUARE_H8), "H8 should be valid");
    CHECK(is_valid_square(0), "0 should be valid");
    CHECK(is_valid_square(63), "63 should be valid");
    CHECK(!is_valid_square(SQUARE_NONE), "SQUARE_NONE should not be valid");
    CHECK(!is_valid_square(64), "64 should not be valid");
    CHECK(!is_valid_square(255), "255 should not be valid");

    return EXIT_SUCCESS;
}

int main() {
    std::cout << "Running square tests...\n";

    if (test_square_constants() != EXIT_SUCCESS) return EXIT_FAILURE;
    if (test_file_rank_extraction() != EXIT_SUCCESS) return EXIT_FAILURE;
    if (test_make_square() != EXIT_SUCCESS) return EXIT_FAILURE;
    if (test_square_to_algebraic() != EXIT_SUCCESS) return EXIT_FAILURE;
    if (test_algebraic_to_square() != EXIT_SUCCESS) return EXIT_FAILURE;
    if (test_round_trip_conversion() != EXIT_SUCCESS) return EXIT_FAILURE;
    if (test_is_valid_square() != EXIT_SUCCESS) return EXIT_FAILURE;

    std::cout << "All square tests passed!\n";
    return EXIT_SUCCESS;
}
