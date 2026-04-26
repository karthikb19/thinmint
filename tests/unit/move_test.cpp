#include <cassert>
#include <iostream>
#include <string>

#include "thinmint/core/move.h"
#include "thinmint/core/piece.h"
#include "thinmint/core/square.h"

using namespace thinmint::core;

// Test basic move creation and extraction
void test_basic_move() {
    std::cout << "Testing basic move creation and extraction..." << std::endl;

    // Test simple quiet move e2->e4
    Move m = make_move(SQUARE_E2, SQUARE_E4);
    assert(from_square(m) == SQUARE_E2);
    assert(to_square(m) == SQUARE_E4);
    assert(!is_capture(m));
    assert(!is_promotion(m));
    assert(!is_castling(m));
    assert(!is_en_passant(m));

    // Test another move a1->h8
    Move m2 = make_move(SQUARE_A1, SQUARE_H8);
    assert(from_square(m2) == SQUARE_A1);
    assert(to_square(m2) == SQUARE_H8);

    // Test capture move
    Move cap = make_capture(SQUARE_E4, SQUARE_D5);
    assert(from_square(cap) == SQUARE_E4);
    assert(to_square(cap) == SQUARE_D5);
    assert(is_capture(cap));
    assert(!is_promotion(cap));

    std::cout << "  PASS: Basic move creation and extraction" << std::endl;
}

// Test promotion moves
void test_promotion_moves() {
    std::cout << "Testing promotion moves..." << std::endl;

    // Test all promotion types (quiet)
    Square from = SQUARE_E7;
    Square to = SQUARE_E8;

    Move knight_promo = make_promotion(from, to, PIECE_KNIGHT, false);
    assert(from_square(knight_promo) == from);
    assert(to_square(knight_promo) == to);
    assert(is_promotion(knight_promo));
    assert(!is_capture(knight_promo));
    assert(promotion_type(knight_promo) == PIECE_KNIGHT);

    Move bishop_promo = make_promotion(from, to, PIECE_BISHOP, false);
    assert(is_promotion(bishop_promo));
    assert(promotion_type(bishop_promo) == PIECE_BISHOP);

    Move rook_promo = make_promotion(from, to, PIECE_ROOK, false);
    assert(is_promotion(rook_promo));
    assert(promotion_type(rook_promo) == PIECE_ROOK);

    Move queen_promo = make_promotion(from, to, PIECE_QUEEN, false);
    assert(is_promotion(queen_promo));
    assert(promotion_type(queen_promo) == PIECE_QUEEN);

    // Test promotion captures
    Move queen_capture_promo = make_promotion(from, to, PIECE_QUEEN, true);
    assert(is_promotion(queen_capture_promo));
    assert(is_capture(queen_capture_promo));
    assert(promotion_type(queen_capture_promo) == PIECE_QUEEN);

    std::cout << "  PASS: Promotion moves" << std::endl;
}

// Test castling moves
void test_castling_moves() {
    std::cout << "Testing castling moves..." << std::endl;

    // White kingside castling (e1->g1)
    Move wk_castle = make_castling(SQUARE_E1, SQUARE_G1);
    assert(from_square(wk_castle) == SQUARE_E1);
    assert(to_square(wk_castle) == SQUARE_G1);
    assert(is_castling(wk_castle));
    assert(!is_promotion(wk_castle));
    assert(!is_capture(wk_castle));

    // White queenside castling (e1->c1)
    Move wq_castle = make_castling(SQUARE_E1, SQUARE_C1);
    assert(from_square(wq_castle) == SQUARE_E1);
    assert(to_square(wq_castle) == SQUARE_C1);
    assert(is_castling(wq_castle));

    // Black kingside castling (e8->g8)
    Move bk_castle = make_castling(SQUARE_E8, SQUARE_G8);
    assert(from_square(bk_castle) == SQUARE_E8);
    assert(to_square(bk_castle) == SQUARE_G8);
    assert(is_castling(bk_castle));

    // Black queenside castling (e8->c8)
    Move bq_castle = make_castling(SQUARE_E8, SQUARE_C8);
    assert(from_square(bq_castle) == SQUARE_E8);
    assert(to_square(bq_castle) == SQUARE_C8);
    assert(is_castling(bq_castle));

    std::cout << "  PASS: Castling moves" << std::endl;
}

// Test en passant moves
void test_en_passant_moves() {
    std::cout << "Testing en passant moves..." << std::endl;

    // White capturing en passant (e5->d6)
    Move ep = make_en_passant(SQUARE_E5, SQUARE_D6);
    assert(from_square(ep) == SQUARE_E5);
    assert(to_square(ep) == SQUARE_D6);
    assert(is_en_passant(ep));
    assert(!is_promotion(ep));
    assert(!is_castling(ep));
    // Note: EP captures are technically captures but encoded differently

    // Black capturing en passant (d4->e3)
    Move ep2 = make_en_passant(SQUARE_D4, SQUARE_E3);
    assert(from_square(ep2) == SQUARE_D4);
    assert(to_square(ep2) == SQUARE_E3);
    assert(is_en_passant(ep2));

    std::cout << "  PASS: En passant moves" << std::endl;
}

// Test MOVE_NONE
void test_move_none() {
    std::cout << "Testing MOVE_NONE..." << std::endl;

    assert(MOVE_NONE == 0);
    assert(from_square(MOVE_NONE) == 0);
    assert(to_square(MOVE_NONE) == 0);
    assert(!is_capture(MOVE_NONE));
    assert(!is_promotion(MOVE_NONE));
    assert(!is_castling(MOVE_NONE));
    assert(!is_en_passant(MOVE_NONE));

    std::cout << "  PASS: MOVE_NONE" << std::endl;
}

// Test UCI conversion
void test_uci_conversion() {
    std::cout << "Testing UCI conversion..." << std::endl;

    // Test quiet move
    Move m1 = make_move(SQUARE_E2, SQUARE_E4);
    std::string uci1 = move_to_uci(m1);
    assert(uci1 == "e2e4");
    Move parsed1 = uci_to_move(uci1);
    assert(parsed1 == m1);

    // Test capture (not visible in UCI)
    Move m2 = make_capture(SQUARE_E4, SQUARE_D5);
    std::string uci2 = move_to_uci(m2);
    assert(uci2 == "e4d5");

    // Test castling (king destination square)
    Move m3 = make_castling(SQUARE_E1, SQUARE_G1);
    std::string uci3 = move_to_uci(m3);
    assert(uci3 == "e1g1");

    // Test promotion
    Move m4 = make_promotion(SQUARE_E7, SQUARE_E8, PIECE_QUEEN, false);
    std::string uci4 = move_to_uci(m4);
    assert(uci4 == "e7e8q");
    Move parsed4 = uci_to_move(uci4);
    assert(is_promotion(parsed4));
    assert(promotion_type(parsed4) == PIECE_QUEEN);

    // Test knight promotion
    Move m5 = make_promotion(SQUARE_A7, SQUARE_A8, PIECE_KNIGHT, false);
    std::string uci5 = move_to_uci(m5);
    assert(uci5 == "a7a8n");
    Move parsed5 = uci_to_move(uci5);
    assert(promotion_type(parsed5) == PIECE_KNIGHT);

    // Test bishop promotion
    Move m6 = make_promotion(SQUARE_B7, SQUARE_B8, PIECE_BISHOP, false);
    assert(move_to_uci(m6) == "b7b8b");

    // Test rook promotion
    Move m7 = make_promotion(SQUARE_C7, SQUARE_C8, PIECE_ROOK, false);
    assert(move_to_uci(m7) == "c7c8r");

    // Test invalid UCI strings
    assert(uci_to_move("") == MOVE_NONE);
    assert(uci_to_move("e2") == MOVE_NONE);          // Too short
    assert(uci_to_move("i2e4") == MOVE_NONE);        // Invalid file
    assert(uci_to_move("e9e4") == MOVE_NONE);        // Invalid rank
    assert(uci_to_move("e2e4x") == MOVE_NONE);       // Invalid promotion piece

    // Note: UCI parser validates format, not chess legality
    // e2e4q parses as a syntactically valid (but illegal) promotion move

    // Test MOVE_NONE UCI conversion
    assert(move_to_uci(MOVE_NONE) == "");

    std::cout << "  PASS: UCI conversion" << std::endl;
}

// Test move flags extraction
void test_move_flags() {
    std::cout << "Testing move flags extraction..." << std::endl;

    Move quiet = make_move(SQUARE_A1, SQUARE_A2);
    MoveFlags quiet_flags = move_flags(quiet);
    assert(quiet_flags == MoveFlags::QUIET);

    Move capture = make_capture(SQUARE_A1, SQUARE_A2);
    MoveFlags capture_flags = move_flags(capture);
    assert(capture_flags == MoveFlags::CAPTURE);

    Move promo = make_promotion(SQUARE_A7, SQUARE_A8, PIECE_QUEEN, false);
    MoveFlags promo_flags = move_flags(promo);
    assert(promo_flags == MoveFlags::PROMOTE_QUEEN);

    Move castle = make_castling(SQUARE_E1, SQUARE_G1);
    MoveFlags castle_flags = move_flags(castle);
    assert(castle_flags == MoveFlags::CASTLING);

    Move ep = make_en_passant(SQUARE_E5, SQUARE_D6);
    MoveFlags ep_flags = move_flags(ep);
    assert(ep_flags == MoveFlags::EP_CAPTURE);

    std::cout << "  PASS: Move flags extraction" << std::endl;
}

// Test move string representation
void test_move_string() {
    std::cout << "Testing move string representation..." << std::endl;

    // Test that move_to_string produces non-empty output for valid moves
    Move m1 = make_move(SQUARE_E2, SQUARE_E4);
    std::string s1 = move_to_string(m1);
    assert(!s1.empty());
    assert(s1.find("e2") != std::string::npos);
    assert(s1.find("e4") != std::string::npos);

    Move m2 = make_capture(SQUARE_E4, SQUARE_D5);
    std::string s2 = move_to_string(m2);
    assert(s2.find("x") != std::string::npos);

    Move m3 = make_castling(SQUARE_E1, SQUARE_G1);
    std::string s3 = move_to_string(m3);
    assert(s3 == "O-O");

    Move m4 = make_castling(SQUARE_E1, SQUARE_C1);
    std::string s4 = move_to_string(m4);
    assert(s4 == "O-O-O");

    Move m5 = make_promotion(SQUARE_E7, SQUARE_E8, PIECE_QUEEN, false);
    std::string s5 = move_to_string(m5);
    assert(s5.find("=") != std::string::npos);
    assert(s5.find("Q") != std::string::npos);

    Move m6 = make_en_passant(SQUARE_E5, SQUARE_D6);
    std::string s6 = move_to_string(m6);
    assert(s6.find("ep") != std::string::npos);

    // Test MOVE_NONE
    assert(move_to_string(MOVE_NONE) == "(none)");

    std::cout << "  PASS: Move string representation" << std::endl;
}

int main() {
    std::cout << "=== Move Encoding Tests ===" << std::endl;

    test_move_none();
    test_basic_move();
    test_promotion_moves();
    test_castling_moves();
    test_en_passant_moves();
    test_uci_conversion();
    test_move_flags();
    test_move_string();

    std::cout << "\nAll move encoding tests passed!" << std::endl;
    return 0;
}
