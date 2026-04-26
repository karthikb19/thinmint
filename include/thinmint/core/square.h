#pragma once

#include <cstdint>
#include <string_view>

namespace thinmint::core {

// Square indexing: 0-63 mapped to board squares
// Rank 1 (bottom) to Rank 8 (top), File A to File H
// Square 0 = A1, Square 7 = H1, Square 56 = A8, Square 63 = H8
using Square = uint8_t;

inline constexpr Square SQUARE_A1 = 0;
inline constexpr Square SQUARE_B1 = 1;
inline constexpr Square SQUARE_C1 = 2;
inline constexpr Square SQUARE_D1 = 3;
inline constexpr Square SQUARE_E1 = 4;
inline constexpr Square SQUARE_F1 = 5;
inline constexpr Square SQUARE_G1 = 6;
inline constexpr Square SQUARE_H1 = 7;
inline constexpr Square SQUARE_A2 = 8;
inline constexpr Square SQUARE_B2 = 9;
inline constexpr Square SQUARE_C2 = 10;
inline constexpr Square SQUARE_D2 = 11;
inline constexpr Square SQUARE_E2 = 12;
inline constexpr Square SQUARE_F2 = 13;
inline constexpr Square SQUARE_G2 = 14;
inline constexpr Square SQUARE_H2 = 15;
inline constexpr Square SQUARE_A3 = 16;
inline constexpr Square SQUARE_B3 = 17;
inline constexpr Square SQUARE_C3 = 18;
inline constexpr Square SQUARE_D3 = 19;
inline constexpr Square SQUARE_E3 = 20;
inline constexpr Square SQUARE_F3 = 21;
inline constexpr Square SQUARE_G3 = 22;
inline constexpr Square SQUARE_H3 = 23;
inline constexpr Square SQUARE_A4 = 24;
inline constexpr Square SQUARE_B4 = 25;
inline constexpr Square SQUARE_C4 = 26;
inline constexpr Square SQUARE_D4 = 27;
inline constexpr Square SQUARE_E4 = 28;
inline constexpr Square SQUARE_F4 = 29;
inline constexpr Square SQUARE_G4 = 30;
inline constexpr Square SQUARE_H4 = 31;
inline constexpr Square SQUARE_A5 = 32;
inline constexpr Square SQUARE_B5 = 33;
inline constexpr Square SQUARE_C5 = 34;
inline constexpr Square SQUARE_D5 = 35;
inline constexpr Square SQUARE_E5 = 36;
inline constexpr Square SQUARE_F5 = 37;
inline constexpr Square SQUARE_G5 = 38;
inline constexpr Square SQUARE_H5 = 39;
inline constexpr Square SQUARE_A6 = 40;
inline constexpr Square SQUARE_B6 = 41;
inline constexpr Square SQUARE_C6 = 42;
inline constexpr Square SQUARE_D6 = 43;
inline constexpr Square SQUARE_E6 = 44;
inline constexpr Square SQUARE_F6 = 45;
inline constexpr Square SQUARE_G6 = 46;
inline constexpr Square SQUARE_H6 = 47;
inline constexpr Square SQUARE_A7 = 48;
inline constexpr Square SQUARE_B7 = 49;
inline constexpr Square SQUARE_C7 = 50;
inline constexpr Square SQUARE_D7 = 51;
inline constexpr Square SQUARE_E7 = 52;
inline constexpr Square SQUARE_F7 = 53;
inline constexpr Square SQUARE_G7 = 54;
inline constexpr Square SQUARE_H7 = 55;
inline constexpr Square SQUARE_A8 = 56;
inline constexpr Square SQUARE_B8 = 57;
inline constexpr Square SQUARE_C8 = 58;
inline constexpr Square SQUARE_D8 = 59;
inline constexpr Square SQUARE_E8 = 60;
inline constexpr Square SQUARE_F8 = 61;
inline constexpr Square SQUARE_G8 = 62;
inline constexpr Square SQUARE_H8 = 63;

inline constexpr Square SQUARE_NONE = 64;  // Invalid square
inline constexpr int SQUARE_COUNT = 64;

// File (column): 0-7 = A-H
using File = uint8_t;
inline constexpr File FILE_A = 0;
inline constexpr File FILE_B = 1;
inline constexpr File FILE_C = 2;
inline constexpr File FILE_D = 3;
inline constexpr File FILE_E = 4;
inline constexpr File FILE_F = 5;
inline constexpr File FILE_G = 6;
inline constexpr File FILE_H = 7;
inline constexpr File FILE_NONE = 8;

// Rank (row): 0-7 = 1-8
using Rank = uint8_t;
inline constexpr Rank RANK_1 = 0;
inline constexpr Rank RANK_2 = 1;
inline constexpr Rank RANK_3 = 2;
inline constexpr Rank RANK_4 = 3;
inline constexpr Rank RANK_5 = 4;
inline constexpr Rank RANK_6 = 5;
inline constexpr Rank RANK_7 = 6;
inline constexpr Rank RANK_8 = 7;
inline constexpr Rank RANK_NONE = 8;

// Extract file from square (0-7)
constexpr File file_of(Square sq) {
    return static_cast<File>(sq & 7);
}

// Extract rank from square (0-7)
constexpr Rank rank_of(Square sq) {
    return static_cast<Rank>(sq >> 3);
}

// Create square from file and rank
constexpr Square make_square(File f, Rank r) {
    return static_cast<Square>((r << 3) | f);
}

// Convert square to algebraic notation (e.g., "e4")
// Buffer must be at least 3 bytes
void square_to_algebraic(Square sq, char* buffer);

// Convert algebraic notation to square (e.g., "e4" -> 28)
// Returns SQUARE_NONE on invalid input
Square algebraic_to_square(std::string_view algebraic);

// Check if square is valid (0-63)
constexpr bool is_valid_square(Square sq) {
    return sq < SQUARE_COUNT;
}

}  // namespace thinmint::core
