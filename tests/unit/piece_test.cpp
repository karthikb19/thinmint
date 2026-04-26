#include <cstdlib>
#include <cstring>
#include <iostream>

#include "thinmint/core/piece.h"

using namespace thinmint::core;

// Helper to check condition and report
#define CHECK(cond, msg) \
    do { \
        if (!(cond)) { \
            std::cerr << "FAIL: " << msg << " at line " << __LINE__ << "\n"; \
            return EXIT_FAILURE; \
        } \
    } while(0)

int test_color_enum() {
    // Test color values
    CHECK(static_cast<uint8_t>(COLOR_WHITE) == 0, "WHITE should be 0");
    CHECK(static_cast<uint8_t>(COLOR_BLACK) == 1, "BLACK should be 1");
    CHECK(COLOR_COUNT == 2, "COLOR_COUNT should be 2");

    // Test color toggle
    CHECK(!COLOR_WHITE == COLOR_BLACK, "!WHITE should be BLACK");
    CHECK(!COLOR_BLACK == COLOR_WHITE, "!BLACK should be WHITE");
    CHECK(!!COLOR_WHITE == COLOR_WHITE, "!!WHITE should be WHITE");

    return EXIT_SUCCESS;
}

int test_piece_type_enum() {
    // Test piece type values
    CHECK(static_cast<uint8_t>(PIECE_NONE) == 0, "NONE should be 0");
    CHECK(static_cast<uint8_t>(PIECE_PAWN) == 1, "PAWN should be 1");
    CHECK(static_cast<uint8_t>(PIECE_KNIGHT) == 2, "KNIGHT should be 2");
    CHECK(static_cast<uint8_t>(PIECE_BISHOP) == 3, "BISHOP should be 3");
    CHECK(static_cast<uint8_t>(PIECE_ROOK) == 4, "ROOK should be 4");
    CHECK(static_cast<uint8_t>(PIECE_QUEEN) == 5, "QUEEN should be 5");
    CHECK(static_cast<uint8_t>(PIECE_KING) == 6, "KING should be 6");

    return EXIT_SUCCESS;
}

int test_piece_encoding() {
    // Test that piece encoding follows (color << 3) | type
    // White pieces: type 1-6 -> values 1-6
    CHECK(static_cast<uint8_t>(PIECE_W_PAWN) == 1, "W_PAWN should be 1");
    CHECK(static_cast<uint8_t>(PIECE_W_KNIGHT) == 2, "W_KNIGHT should be 2");
    CHECK(static_cast<uint8_t>(PIECE_W_BISHOP) == 3, "W_BISHOP should be 3");
    CHECK(static_cast<uint8_t>(PIECE_W_ROOK) == 4, "W_ROOK should be 4");
    CHECK(static_cast<uint8_t>(PIECE_W_QUEEN) == 5, "W_QUEEN should be 5");
    CHECK(static_cast<uint8_t>(PIECE_W_KING) == 6, "W_KING should be 6");

    // Black pieces: type 1-6 + 8 -> values 9-14
    CHECK(static_cast<uint8_t>(PIECE_B_PAWN) == 9, "B_PAWN should be 9");
    CHECK(static_cast<uint8_t>(PIECE_B_KNIGHT) == 10, "B_KNIGHT should be 10");
    CHECK(static_cast<uint8_t>(PIECE_B_BISHOP) == 11, "B_BISHOP should be 11");
    CHECK(static_cast<uint8_t>(PIECE_B_ROOK) == 12, "B_ROOK should be 12");
    CHECK(static_cast<uint8_t>(PIECE_B_QUEEN) == 13, "B_QUEEN should be 13");
    CHECK(static_cast<uint8_t>(PIECE_B_KING) == 14, "B_KING should be 14");

    // No piece should be 0
    CHECK(static_cast<uint8_t>(PIECE_NO_PIECE) == 0, "NO_PIECE should be 0");

    return EXIT_SUCCESS;
}

int test_color_of() {
    // Test extracting color from pieces
    CHECK(color_of(PIECE_W_PAWN) == COLOR_WHITE, "color_of(W_PAWN) should be WHITE");
    CHECK(color_of(PIECE_W_KING) == COLOR_WHITE, "color_of(W_KING) should be WHITE");
    CHECK(color_of(PIECE_B_PAWN) == COLOR_BLACK, "color_of(B_PAWN) should be BLACK");
    CHECK(color_of(PIECE_B_KING) == COLOR_BLACK, "color_of(B_KING) should be BLACK");
    CHECK(color_of(PIECE_NO_PIECE) == COLOR_WHITE, "color_of(NO_PIECE) should be WHITE (default)");

    return EXIT_SUCCESS;
}

int test_type_of() {
    // Test extracting type from pieces
    CHECK(type_of(PIECE_W_PAWN) == PIECE_PAWN, "type_of(W_PAWN) should be PAWN");
    CHECK(type_of(PIECE_W_KNIGHT) == PIECE_KNIGHT, "type_of(W_KNIGHT) should be KNIGHT");
    CHECK(type_of(PIECE_W_KING) == PIECE_KING, "type_of(W_KING) should be KING");
    CHECK(type_of(PIECE_B_PAWN) == PIECE_PAWN, "type_of(B_PAWN) should be PAWN");
    CHECK(type_of(PIECE_B_QUEEN) == PIECE_QUEEN, "type_of(B_QUEEN) should be QUEEN");
    CHECK(type_of(PIECE_NO_PIECE) == PIECE_NONE, "type_of(NO_PIECE) should be NONE");

    return EXIT_SUCCESS;
}

int test_make_piece() {
    // Test creating pieces from color and type
    CHECK(make_piece(COLOR_WHITE, PIECE_PAWN) == PIECE_W_PAWN, "make_piece(WHITE, PAWN) should be W_PAWN");
    CHECK(make_piece(COLOR_WHITE, PIECE_KING) == PIECE_W_KING, "make_piece(WHITE, KING) should be W_KING");
    CHECK(make_piece(COLOR_BLACK, PIECE_PAWN) == PIECE_B_PAWN, "make_piece(BLACK, PAWN) should be B_PAWN");
    CHECK(make_piece(COLOR_BLACK, PIECE_QUEEN) == PIECE_B_QUEEN, "make_piece(BLACK, QUEEN) should be B_QUEEN");

    // Test round-trip: make_piece(color_of(p), type_of(p)) should equal p for valid pieces
    Piece pieces[] = {PIECE_W_PAWN, PIECE_W_KNIGHT, PIECE_W_BISHOP, PIECE_W_ROOK, PIECE_W_QUEEN, PIECE_W_KING,
                      PIECE_B_PAWN, PIECE_B_KNIGHT, PIECE_B_BISHOP, PIECE_B_ROOK, PIECE_B_QUEEN, PIECE_B_KING};
    for (Piece p : pieces) {
        CHECK(make_piece(color_of(p), type_of(p)) == p, "make_piece round-trip failed");
    }

    return EXIT_SUCCESS;
}

int test_is_valid_piece() {
    // Valid pieces
    CHECK(is_valid_piece(PIECE_W_PAWN), "W_PAWN should be valid");
    CHECK(is_valid_piece(PIECE_W_KING), "W_KING should be valid");
    CHECK(is_valid_piece(PIECE_B_QUEEN), "B_QUEEN should be valid");

    // Invalid pieces
    CHECK(!is_valid_piece(PIECE_NO_PIECE), "NO_PIECE should not be valid");
    CHECK(!is_valid_piece(static_cast<Piece>(7)), "Piece 7 should not be valid (type NONE)");
    CHECK(!is_valid_piece(static_cast<Piece>(15)), "Piece 15 should not be valid (type NONE)");

    return EXIT_SUCCESS;
}

int test_is_white_is_black() {
    CHECK(is_white(PIECE_W_PAWN), "W_PAWN should be white");
    CHECK(is_white(PIECE_W_KING), "W_KING should be white");
    CHECK(!is_white(PIECE_B_PAWN), "B_PAWN should not be white");
    CHECK(!is_white(PIECE_NO_PIECE), "NO_PIECE should not be white");

    CHECK(is_black(PIECE_B_PAWN), "B_PAWN should be black");
    CHECK(is_black(PIECE_B_KING), "B_KING should be black");
    CHECK(!is_black(PIECE_W_PAWN), "W_PAWN should not be black");
    CHECK(!is_black(PIECE_NO_PIECE), "NO_PIECE should not be black");

    return EXIT_SUCCESS;
}

int test_castling_rights() {
    // Test individual values
    CHECK(static_cast<uint8_t>(CASTLING_NONE) == 0, "NONE should be 0");
    CHECK(static_cast<uint8_t>(CASTLING_WHITE_KINGSIDE) == 1, "WHITE_KINGSIDE should be 1");
    CHECK(static_cast<uint8_t>(CASTLING_WHITE_QUEENSIDE) == 2, "WHITE_QUEENSIDE should be 2");
    CHECK(static_cast<uint8_t>(CASTLING_BLACK_KINGSIDE) == 4, "BLACK_KINGSIDE should be 4");
    CHECK(static_cast<uint8_t>(CASTLING_BLACK_QUEENSIDE) == 8, "BLACK_QUEENSIDE should be 8");
    CHECK(static_cast<uint8_t>(CASTLING_WHITE_BOTH) == 3, "WHITE_BOTH should be 3");
    CHECK(static_cast<uint8_t>(CASTLING_BLACK_BOTH) == 12, "BLACK_BOTH should be 12");
    CHECK(static_cast<uint8_t>(CASTLING_ALL) == 15, "ALL should be 15");

    return EXIT_SUCCESS;
}

int test_castling_bitwise_ops() {
    // Test OR
    CHECK(static_cast<uint8_t>(CASTLING_WHITE_KINGSIDE | CASTLING_WHITE_QUEENSIDE) == 3,
          "WK | WQ should be 3");
    CHECK(static_cast<uint8_t>(CASTLING_BLACK_KINGSIDE | CASTLING_BLACK_QUEENSIDE) == 12,
          "BK | BQ should be 12");

    // Test AND
    CastlingRights combined = CASTLING_WHITE_BOTH | CASTLING_BLACK_KINGSIDE;  // 3 | 4 = 7
    CHECK(static_cast<uint8_t>(combined & CASTLING_WHITE_KINGSIDE) == 1,
          "(WK|WQ|BK) & WK should be WK");
    CHECK(static_cast<uint8_t>(combined & CASTLING_BLACK_QUEENSIDE) == 0,
          "(WK|WQ|BK) & BQ should be NONE");

    // Test |= and &=
    CastlingRights rights = CASTLING_NONE;
    rights |= CASTLING_WHITE_KINGSIDE;
    CHECK(static_cast<uint8_t>(rights) == 1, "After |= WK, rights should be 1");
    rights |= CASTLING_BLACK_BOTH;
    CHECK(static_cast<uint8_t>(rights) == 13, "After |= BLACK_BOTH, rights should be 13");

    return EXIT_SUCCESS;
}

int test_has_castling_right() {
    CHECK(has_castling_right(CASTLING_ALL, CASTLING_WHITE_KINGSIDE), "ALL should have WK");
    CHECK(has_castling_right(CASTLING_ALL, CASTLING_WHITE_QUEENSIDE), "ALL should have WQ");
    CHECK(has_castling_right(CASTLING_ALL, CASTLING_BLACK_KINGSIDE), "ALL should have BK");
    CHECK(has_castling_right(CASTLING_ALL, CASTLING_BLACK_QUEENSIDE), "ALL should have BQ");

    CHECK(has_castling_right(CASTLING_WHITE_BOTH, CASTLING_WHITE_KINGSIDE), "WHITE_BOTH should have WK");
    CHECK(has_castling_right(CASTLING_WHITE_BOTH, CASTLING_WHITE_QUEENSIDE), "WHITE_BOTH should have WQ");
    CHECK(!has_castling_right(CASTLING_WHITE_BOTH, CASTLING_BLACK_KINGSIDE), "WHITE_BOTH should not have BK");

    CHECK(!has_castling_right(CASTLING_NONE, CASTLING_WHITE_KINGSIDE), "NONE should not have WK");

    return EXIT_SUCCESS;
}

int test_remove_castling_right() {
    CHECK(static_cast<uint8_t>(remove_castling_right(CASTLING_ALL, CASTLING_WHITE_KINGSIDE)) == 14,
          "ALL - WK should be 14");
    CHECK(static_cast<uint8_t>(remove_castling_right(CASTLING_WHITE_BOTH, CASTLING_WHITE_QUEENSIDE)) == 1,
          "WHITE_BOTH - WQ should be 1 (WK only)");
    CHECK(static_cast<uint8_t>(remove_castling_right(CASTLING_ALL, CASTLING_ALL)) == 0,
          "ALL - ALL should be NONE");

    return EXIT_SUCCESS;
}

int test_castling_rook_square() {
    CHECK(castling_rook_square(CASTLING_WHITE_KINGSIDE) == 7, "WK rook should be at H1 (7)");
    CHECK(castling_rook_square(CASTLING_WHITE_QUEENSIDE) == 0, "WQ rook should be at A1 (0)");
    CHECK(castling_rook_square(CASTLING_BLACK_KINGSIDE) == 63, "BK rook should be at H8 (63)");
    CHECK(castling_rook_square(CASTLING_BLACK_QUEENSIDE) == 56, "BQ rook should be at A8 (56)");
    CHECK(castling_rook_square(CASTLING_NONE) == 64, "NONE should return SQUARE_NONE");
    CHECK(castling_rook_square(CASTLING_ALL) == 64, "ALL should return SQUARE_NONE (ambiguous)");

    return EXIT_SUCCESS;
}

int test_king_start_square() {
    CHECK(king_start_square(COLOR_WHITE) == 4, "White king should start at E1 (4)");
    CHECK(king_start_square(COLOR_BLACK) == 60, "Black king should start at E8 (60)");

    return EXIT_SUCCESS;
}

int test_piece_to_char() {
    // White pieces (uppercase)
    CHECK(piece_to_char(PIECE_W_PAWN) == 'P', "W_PAWN should be 'P'");
    CHECK(piece_to_char(PIECE_W_KNIGHT) == 'N', "W_KNIGHT should be 'N'");
    CHECK(piece_to_char(PIECE_W_BISHOP) == 'B', "W_BISHOP should be 'B'");
    CHECK(piece_to_char(PIECE_W_ROOK) == 'R', "W_ROOK should be 'R'");
    CHECK(piece_to_char(PIECE_W_QUEEN) == 'Q', "W_QUEEN should be 'Q'");
    CHECK(piece_to_char(PIECE_W_KING) == 'K', "W_KING should be 'K'");

    // Black pieces (lowercase)
    CHECK(piece_to_char(PIECE_B_PAWN) == 'p', "B_PAWN should be 'p'");
    CHECK(piece_to_char(PIECE_B_KNIGHT) == 'n', "B_KNIGHT should be 'n'");
    CHECK(piece_to_char(PIECE_B_BISHOP) == 'b', "B_BISHOP should be 'b'");
    CHECK(piece_to_char(PIECE_B_ROOK) == 'r', "B_ROOK should be 'r'");
    CHECK(piece_to_char(PIECE_B_QUEEN) == 'q', "B_QUEEN should be 'q'");
    CHECK(piece_to_char(PIECE_B_KING) == 'k', "B_KING should be 'k'");

    // Invalid piece
    CHECK(piece_to_char(PIECE_NO_PIECE) == '?', "NO_PIECE should be '?'");

    return EXIT_SUCCESS;
}

int test_char_to_piece() {
    // White pieces (uppercase)
    CHECK(char_to_piece('P') == PIECE_W_PAWN, "'P' should be W_PAWN");
    CHECK(char_to_piece('N') == PIECE_W_KNIGHT, "'N' should be W_KNIGHT");
    CHECK(char_to_piece('B') == PIECE_W_BISHOP, "'B' should be W_BISHOP");
    CHECK(char_to_piece('R') == PIECE_W_ROOK, "'R' should be W_ROOK");
    CHECK(char_to_piece('Q') == PIECE_W_QUEEN, "'Q' should be W_QUEEN");
    CHECK(char_to_piece('K') == PIECE_W_KING, "'K' should be W_KING");

    // Black pieces (lowercase)
    CHECK(char_to_piece('p') == PIECE_B_PAWN, "'p' should be B_PAWN");
    CHECK(char_to_piece('n') == PIECE_B_KNIGHT, "'n' should be B_KNIGHT");
    CHECK(char_to_piece('b') == PIECE_B_BISHOP, "'b' should be B_BISHOP");
    CHECK(char_to_piece('r') == PIECE_B_ROOK, "'r' should be B_ROOK");
    CHECK(char_to_piece('q') == PIECE_B_QUEEN, "'q' should be B_QUEEN");
    CHECK(char_to_piece('k') == PIECE_B_KING, "'k' should be B_KING");

    // Invalid characters
    CHECK(char_to_piece('x') == PIECE_NO_PIECE, "'x' should be NO_PIECE");
    CHECK(char_to_piece('1') == PIECE_NO_PIECE, "'1' should be NO_PIECE");
    CHECK(char_to_piece(' ') == PIECE_NO_PIECE, "' ' should be NO_PIECE");

    return EXIT_SUCCESS;
}

int test_piece_char_round_trip() {
    // Test that piece -> char -> piece round-trips for all valid pieces
    Piece pieces[] = {PIECE_W_PAWN, PIECE_W_KNIGHT, PIECE_W_BISHOP, PIECE_W_ROOK, PIECE_W_QUEEN, PIECE_W_KING,
                      PIECE_B_PAWN, PIECE_B_KNIGHT, PIECE_B_BISHOP, PIECE_B_ROOK, PIECE_B_QUEEN, PIECE_B_KING};
    for (Piece p : pieces) {
        char c = piece_to_char(p);
        Piece back = char_to_piece(c);
        CHECK(p == back, "piece->char->piece round-trip failed");
    }

    return EXIT_SUCCESS;
}

int test_castling_rights_to_string() {
    char buffer[16];

    // All rights
    castling_rights_to_string(CASTLING_ALL, buffer);
    CHECK(std::strcmp(buffer, "KQkq") == 0, "ALL should be 'KQkq'");

    // White only
    castling_rights_to_string(CASTLING_WHITE_BOTH, buffer);
    CHECK(std::strcmp(buffer, "KQ") == 0, "WHITE_BOTH should be 'KQ'");

    // Black only
    castling_rights_to_string(CASTLING_BLACK_BOTH, buffer);
    CHECK(std::strcmp(buffer, "kq") == 0, "BLACK_BOTH should be 'kq'");

    // Just kingside
    castling_rights_to_string(CASTLING_WHITE_KINGSIDE, buffer);
    CHECK(std::strcmp(buffer, "K") == 0, "WK should be 'K'");

    // Just queenside
    castling_rights_to_string(CASTLING_BLACK_QUEENSIDE, buffer);
    CHECK(std::strcmp(buffer, "q") == 0, "BQ should be 'q'");

    // None
    castling_rights_to_string(CASTLING_NONE, buffer);
    CHECK(std::strcmp(buffer, "-") == 0, "NONE should be '-'");

    return EXIT_SUCCESS;
}

int test_parse_castling_rights() {
    // Standard FEN strings
    CHECK(parse_castling_rights("KQkq") == CASTLING_ALL, "'KQkq' should parse to ALL");
    CHECK(parse_castling_rights("KQ") == CASTLING_WHITE_BOTH, "'KQ' should parse to WHITE_BOTH");
    CHECK(parse_castling_rights("kq") == CASTLING_BLACK_BOTH, "'kq' should parse to BLACK_BOTH");
    CHECK(parse_castling_rights("K") == CASTLING_WHITE_KINGSIDE, "'K' should parse to WK");
    CHECK(parse_castling_rights("Q") == CASTLING_WHITE_QUEENSIDE, "'Q' should parse to WQ");
    CHECK(parse_castling_rights("k") == CASTLING_BLACK_KINGSIDE, "'k' should parse to BK");
    CHECK(parse_castling_rights("q") == CASTLING_BLACK_QUEENSIDE, "'q' should parse to BQ");
    CHECK(parse_castling_rights("-") == CASTLING_NONE, "'-' should parse to NONE");
    CHECK(parse_castling_rights("") == CASTLING_NONE, "empty should parse to NONE");

    // Non-standard order (should still work)
    CHECK(parse_castling_rights("kKqQ") == CASTLING_ALL, "'kKqQ' should parse to ALL");
    CHECK(parse_castling_rights("qkq") == CASTLING_BLACK_BOTH, "'qkq' should parse to BLACK_BOTH (ignoring extra)");

    return EXIT_SUCCESS;
}

int test_castling_string_round_trip() {
    // Test round-trip for various rights combinations
    CastlingRights rights[] = {
        CASTLING_NONE,
        CASTLING_WHITE_KINGSIDE,
        CASTLING_WHITE_QUEENSIDE,
        CASTLING_BLACK_KINGSIDE,
        CASTLING_BLACK_QUEENSIDE,
        CASTLING_WHITE_BOTH,
        CASTLING_BLACK_BOTH,
        CASTLING_ALL
    };

    for (CastlingRights r : rights) {
        char buffer[16];
        castling_rights_to_string(r, buffer);
        CastlingRights back = parse_castling_rights(buffer);
        CHECK(r == back, "castling rights round-trip failed");
    }

    return EXIT_SUCCESS;
}

int main() {
    std::cout << "Running piece tests...\n";

    if (test_color_enum() != EXIT_SUCCESS) return EXIT_FAILURE;
    if (test_piece_type_enum() != EXIT_SUCCESS) return EXIT_FAILURE;
    if (test_piece_encoding() != EXIT_SUCCESS) return EXIT_FAILURE;
    if (test_color_of() != EXIT_SUCCESS) return EXIT_FAILURE;
    if (test_type_of() != EXIT_SUCCESS) return EXIT_FAILURE;
    if (test_make_piece() != EXIT_SUCCESS) return EXIT_FAILURE;
    if (test_is_valid_piece() != EXIT_SUCCESS) return EXIT_FAILURE;
    if (test_is_white_is_black() != EXIT_SUCCESS) return EXIT_FAILURE;
    if (test_castling_rights() != EXIT_SUCCESS) return EXIT_FAILURE;
    if (test_castling_bitwise_ops() != EXIT_SUCCESS) return EXIT_FAILURE;
    if (test_has_castling_right() != EXIT_SUCCESS) return EXIT_FAILURE;
    if (test_remove_castling_right() != EXIT_SUCCESS) return EXIT_FAILURE;
    if (test_castling_rook_square() != EXIT_SUCCESS) return EXIT_FAILURE;
    if (test_king_start_square() != EXIT_SUCCESS) return EXIT_FAILURE;
    if (test_piece_to_char() != EXIT_SUCCESS) return EXIT_FAILURE;
    if (test_char_to_piece() != EXIT_SUCCESS) return EXIT_FAILURE;
    if (test_piece_char_round_trip() != EXIT_SUCCESS) return EXIT_FAILURE;
    if (test_castling_rights_to_string() != EXIT_SUCCESS) return EXIT_FAILURE;
    if (test_parse_castling_rights() != EXIT_SUCCESS) return EXIT_FAILURE;
    if (test_castling_string_round_trip() != EXIT_SUCCESS) return EXIT_FAILURE;

    std::cout << "All piece tests passed!\n";
    return EXIT_SUCCESS;
}
