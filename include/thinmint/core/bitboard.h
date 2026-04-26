#pragma once

#include <cstdint>
#include <type_traits>

#include "thinmint/core/square.h"

namespace thinmint::core {

// Bitboard type: 64-bit unsigned integer representing a chess board
// Each bit corresponds to a square (bit 0 = A1, bit 63 = H8)
using Bitboard = uint64_t;

// Bitboard with all squares set
inline constexpr Bitboard BB_ALL = 0xFFFFFFFFFFFFFFFFULL;

// Bitboard with no squares set (empty)
inline constexpr Bitboard BB_EMPTY = 0x0ULL;

// Individual square bitboards
inline constexpr Bitboard BB_SQUARE_A1 = 1ULL << 0;
inline constexpr Bitboard BB_SQUARE_B1 = 1ULL << 1;
inline constexpr Bitboard BB_SQUARE_C1 = 1ULL << 2;
inline constexpr Bitboard BB_SQUARE_D1 = 1ULL << 3;
inline constexpr Bitboard BB_SQUARE_E1 = 1ULL << 4;
inline constexpr Bitboard BB_SQUARE_F1 = 1ULL << 5;
inline constexpr Bitboard BB_SQUARE_G1 = 1ULL << 6;
inline constexpr Bitboard BB_SQUARE_H1 = 1ULL << 7;
inline constexpr Bitboard BB_SQUARE_A2 = 1ULL << 8;
inline constexpr Bitboard BB_SQUARE_B2 = 1ULL << 9;
inline constexpr Bitboard BB_SQUARE_C2 = 1ULL << 10;
inline constexpr Bitboard BB_SQUARE_D2 = 1ULL << 11;
inline constexpr Bitboard BB_SQUARE_E2 = 1ULL << 12;
inline constexpr Bitboard BB_SQUARE_F2 = 1ULL << 13;
inline constexpr Bitboard BB_SQUARE_G2 = 1ULL << 14;
inline constexpr Bitboard BB_SQUARE_H2 = 1ULL << 15;
inline constexpr Bitboard BB_SQUARE_A3 = 1ULL << 16;
inline constexpr Bitboard BB_SQUARE_B3 = 1ULL << 17;
inline constexpr Bitboard BB_SQUARE_C3 = 1ULL << 18;
inline constexpr Bitboard BB_SQUARE_D3 = 1ULL << 19;
inline constexpr Bitboard BB_SQUARE_E3 = 1ULL << 20;
inline constexpr Bitboard BB_SQUARE_F3 = 1ULL << 21;
inline constexpr Bitboard BB_SQUARE_G3 = 1ULL << 22;
inline constexpr Bitboard BB_SQUARE_H3 = 1ULL << 23;
inline constexpr Bitboard BB_SQUARE_A4 = 1ULL << 24;
inline constexpr Bitboard BB_SQUARE_B4 = 1ULL << 25;
inline constexpr Bitboard BB_SQUARE_C4 = 1ULL << 26;
inline constexpr Bitboard BB_SQUARE_D4 = 1ULL << 27;
inline constexpr Bitboard BB_SQUARE_E4 = 1ULL << 28;
inline constexpr Bitboard BB_SQUARE_F4 = 1ULL << 29;
inline constexpr Bitboard BB_SQUARE_G4 = 1ULL << 30;
inline constexpr Bitboard BB_SQUARE_H4 = 1ULL << 31;
inline constexpr Bitboard BB_SQUARE_A5 = 1ULL << 32;
inline constexpr Bitboard BB_SQUARE_B5 = 1ULL << 33;
inline constexpr Bitboard BB_SQUARE_C5 = 1ULL << 34;
inline constexpr Bitboard BB_SQUARE_D5 = 1ULL << 35;
inline constexpr Bitboard BB_SQUARE_E5 = 1ULL << 36;
inline constexpr Bitboard BB_SQUARE_F5 = 1ULL << 37;
inline constexpr Bitboard BB_SQUARE_G5 = 1ULL << 38;
inline constexpr Bitboard BB_SQUARE_H5 = 1ULL << 39;
inline constexpr Bitboard BB_SQUARE_A6 = 1ULL << 40;
inline constexpr Bitboard BB_SQUARE_B6 = 1ULL << 41;
inline constexpr Bitboard BB_SQUARE_C6 = 1ULL << 42;
inline constexpr Bitboard BB_SQUARE_D6 = 1ULL << 43;
inline constexpr Bitboard BB_SQUARE_E6 = 1ULL << 44;
inline constexpr Bitboard BB_SQUARE_F6 = 1ULL << 45;
inline constexpr Bitboard BB_SQUARE_G6 = 1ULL << 46;
inline constexpr Bitboard BB_SQUARE_H6 = 1ULL << 47;
inline constexpr Bitboard BB_SQUARE_A7 = 1ULL << 48;
inline constexpr Bitboard BB_SQUARE_B7 = 1ULL << 49;
inline constexpr Bitboard BB_SQUARE_C7 = 1ULL << 50;
inline constexpr Bitboard BB_SQUARE_D7 = 1ULL << 51;
inline constexpr Bitboard BB_SQUARE_E7 = 1ULL << 52;
inline constexpr Bitboard BB_SQUARE_F7 = 1ULL << 53;
inline constexpr Bitboard BB_SQUARE_G7 = 1ULL << 54;
inline constexpr Bitboard BB_SQUARE_H7 = 1ULL << 55;
inline constexpr Bitboard BB_SQUARE_A8 = 1ULL << 56;
inline constexpr Bitboard BB_SQUARE_B8 = 1ULL << 57;
inline constexpr Bitboard BB_SQUARE_C8 = 1ULL << 58;
inline constexpr Bitboard BB_SQUARE_D8 = 1ULL << 59;
inline constexpr Bitboard BB_SQUARE_E8 = 1ULL << 60;
inline constexpr Bitboard BB_SQUARE_F8 = 1ULL << 61;
inline constexpr Bitboard BB_SQUARE_G8 = 1ULL << 62;
inline constexpr Bitboard BB_SQUARE_H8 = 1ULL << 63;

// File bitboards
inline constexpr Bitboard BB_FILE_A = 0x0101010101010101ULL;
inline constexpr Bitboard BB_FILE_B = 0x0202020202020202ULL;
inline constexpr Bitboard BB_FILE_C = 0x0404040404040404ULL;
inline constexpr Bitboard BB_FILE_D = 0x0808080808080808ULL;
inline constexpr Bitboard BB_FILE_E = 0x1010101010101010ULL;
inline constexpr Bitboard BB_FILE_F = 0x2020202020202020ULL;
inline constexpr Bitboard BB_FILE_G = 0x4040404040404040ULL;
inline constexpr Bitboard BB_FILE_H = 0x8080808080808080ULL;

// Rank bitboards
inline constexpr Bitboard BB_RANK_1 = 0x00000000000000FFULL;
inline constexpr Bitboard BB_RANK_2 = 0x000000000000FF00ULL;
inline constexpr Bitboard BB_RANK_3 = 0x0000000000FF0000ULL;
inline constexpr Bitboard BB_RANK_4 = 0x00000000FF000000ULL;
inline constexpr Bitboard BB_RANK_5 = 0x000000FF00000000ULL;
inline constexpr Bitboard BB_RANK_6 = 0x0000FF0000000000ULL;
inline constexpr Bitboard BB_RANK_7 = 0x00FF000000000000ULL;
inline constexpr Bitboard BB_RANK_8 = 0xFF00000000000000ULL;

// Array of file bitboards indexed by file (0-7)
inline constexpr Bitboard BB_FILE[8] = {
    BB_FILE_A, BB_FILE_B, BB_FILE_C, BB_FILE_D,
    BB_FILE_E, BB_FILE_F, BB_FILE_G, BB_FILE_H
};

// Array of rank bitboards indexed by rank (0-7)
inline constexpr Bitboard BB_RANK[8] = {
    BB_RANK_1, BB_RANK_2, BB_RANK_3, BB_RANK_4,
    BB_RANK_5, BB_RANK_6, BB_RANK_7, BB_RANK_8
};

// Array of square bitboards indexed by square (0-63)
inline constexpr Bitboard BB_SQUARE[64] = {
    BB_SQUARE_A1, BB_SQUARE_B1, BB_SQUARE_C1, BB_SQUARE_D1,
    BB_SQUARE_E1, BB_SQUARE_F1, BB_SQUARE_G1, BB_SQUARE_H1,
    BB_SQUARE_A2, BB_SQUARE_B2, BB_SQUARE_C2, BB_SQUARE_D2,
    BB_SQUARE_E2, BB_SQUARE_F2, BB_SQUARE_G2, BB_SQUARE_H2,
    BB_SQUARE_A3, BB_SQUARE_B3, BB_SQUARE_C3, BB_SQUARE_D3,
    BB_SQUARE_E3, BB_SQUARE_F3, BB_SQUARE_G3, BB_SQUARE_H3,
    BB_SQUARE_A4, BB_SQUARE_B4, BB_SQUARE_C4, BB_SQUARE_D4,
    BB_SQUARE_E4, BB_SQUARE_F4, BB_SQUARE_G4, BB_SQUARE_H4,
    BB_SQUARE_A5, BB_SQUARE_B5, BB_SQUARE_C5, BB_SQUARE_D5,
    BB_SQUARE_E5, BB_SQUARE_F5, BB_SQUARE_G5, BB_SQUARE_H5,
    BB_SQUARE_A6, BB_SQUARE_B6, BB_SQUARE_C6, BB_SQUARE_D6,
    BB_SQUARE_E6, BB_SQUARE_F6, BB_SQUARE_G6, BB_SQUARE_H6,
    BB_SQUARE_A7, BB_SQUARE_B7, BB_SQUARE_C7, BB_SQUARE_D7,
    BB_SQUARE_E7, BB_SQUARE_F7, BB_SQUARE_G7, BB_SQUARE_H7,
    BB_SQUARE_A8, BB_SQUARE_B8, BB_SQUARE_C8, BB_SQUARE_D8,
    BB_SQUARE_E8, BB_SQUARE_F8, BB_SQUARE_G8, BB_SQUARE_H8
};

// Set a bit on the bitboard (returns new bitboard with bit set)
constexpr Bitboard bb_set(Bitboard bb, Square sq) {
    return bb | (1ULL << sq);
}

// Clear a bit on the bitboard (returns new bitboard with bit cleared)
constexpr Bitboard bb_clear(Bitboard bb, Square sq) {
    return bb & ~(1ULL << sq);
}

// Test if a bit is set on the bitboard
constexpr bool bb_test(Bitboard bb, Square sq) {
    return (bb >> sq) & 1ULL;
}

// Toggle a bit on the bitboard
constexpr Bitboard bb_toggle(Bitboard bb, Square sq) {
    return bb ^ (1ULL << sq);
}

// Check if bitboard is empty
constexpr bool bb_is_empty(Bitboard bb) {
    return bb == BB_EMPTY;
}

// Check if bitboard has any bits set
constexpr bool bb_any(Bitboard bb) {
    return bb != BB_EMPTY;
}

// Count the number of set bits (population count)
// Uses compiler intrinsics when available
inline int bb_popcount(Bitboard bb) {
#if defined(__GNUC__) || defined(__clang__)
    return __builtin_popcountll(bb);
#elif defined(_MSC_VER)
    #include <intrin.h>
    return static_cast<int>(__popcnt64(bb));
#else
    // Fallback: Brian Kernighan's algorithm
    int count = 0;
    while (bb) {
        bb &= bb - 1;
        ++count;
    }
    return count;
#endif
}

// Get the index of the least significant bit (LSB)
// Returns -1 if bitboard is empty
// Note: This assumes bb != 0
inline int bb_lsb_index(Bitboard bb) {
#if defined(__GNUC__) || defined(__clang__)
    return __builtin_ctzll(bb);
#elif defined(_MSC_VER)
    #include <intrin.h>
    unsigned long index;
    _BitScanForward64(&index, bb);
    return static_cast<int>(index);
#else
    // Fallback: De Bruijn sequence
    static constexpr int debruijn_index[64] = {
        0, 1, 48, 2, 57, 49, 28, 3,
        61, 58, 50, 42, 38, 29, 17, 4,
        62, 55, 59, 36, 53, 51, 43, 22,
        45, 39, 33, 30, 24, 18, 12, 5,
        63, 47, 56, 27, 60, 41, 37, 16,
        54, 35, 52, 21, 44, 32, 23, 11,
        46, 26, 40, 15, 34, 20, 31, 10,
        25, 14, 19, 9, 13, 8, 7, 6
    };
    return debruijn_index[((bb ^ (bb - 1)) * 0x03f79d71b4cb0a89ULL) >> 58];
#endif
}

// Get the least significant bit as a bitboard
// Returns 0 if bitboard is empty
constexpr Bitboard bb_lsb(Bitboard bb) {
    return bb & (~bb + 1);
}

// Clear the least significant bit and return the result
constexpr Bitboard bb_clear_lsb(Bitboard bb) {
    return bb & (bb - 1);
}

// Get the index of the most significant bit (MSB)
// Returns -1 if bitboard is empty
inline int bb_msb_index(Bitboard bb) {
#if defined(__GNUC__) || defined(__clang__)
    return 63 - __builtin_clzll(bb);
#elif defined(_MSC_VER)
    #include <intrin.h>
    unsigned long index;
    _BitScanReverse64(&index, bb);
    return static_cast<int>(index);
#else
    // Fallback: binary search for MSB
    int result = 0;
    if (bb > 0xFFFFFFFFULL) { bb >>= 32; result += 32; }
    if (bb > 0xFFFFULL) { bb >>= 16; result += 16; }
    if (bb > 0xFFULL) { bb >>= 8; result += 8; }
    if (bb > 0xFULL) { bb >>= 4; result += 4; }
    if (bb > 0x3ULL) { bb >>= 2; result += 2; }
    if (bb > 0x1ULL) { result += 1; }
    return result;
#endif
}

// Shift bitboard north (towards rank 8)
constexpr Bitboard bb_shift_north(Bitboard bb) {
    return bb << 8;
}

// Shift bitboard south (towards rank 1)
constexpr Bitboard bb_shift_south(Bitboard bb) {
    return bb >> 8;
}

// Shift bitboard east (towards file H), wrapping not allowed
constexpr Bitboard bb_shift_east(Bitboard bb) {
    return (bb << 1) & ~BB_FILE_A;
}

// Shift bitboard west (towards file A), wrapping not allowed
constexpr Bitboard bb_shift_west(Bitboard bb) {
    return (bb >> 1) & ~BB_FILE_H;
}

// Shift bitboard northeast
constexpr Bitboard bb_shift_northeast(Bitboard bb) {
    return (bb << 9) & ~BB_FILE_A;
}

// Shift bitboard northwest
constexpr Bitboard bb_shift_northwest(Bitboard bb) {
    return (bb << 7) & ~BB_FILE_H;
}

// Shift bitboard southeast
constexpr Bitboard bb_shift_southeast(Bitboard bb) {
    return (bb >> 7) & ~BB_FILE_A;
}

// Shift bitboard southwest
constexpr Bitboard bb_shift_southwest(Bitboard bb) {
    return (bb >> 9) & ~BB_FILE_H;
}

// Get the first square from a bitboard (LSB index as Square)
// Returns SQUARE_NONE if bitboard is empty
inline Square bb_first_square(Bitboard bb) {
    return bb_is_empty(bb) ? SQUARE_NONE : static_cast<Square>(bb_lsb_index(bb));
}

// Iterate through all set bits in a bitboard
// Usage: while (bb) { Square sq = bb_pop_square(bb); ... }
// This pops the LSB and returns its index
inline Square bb_pop_square(Bitboard& bb) {
    Square sq = bb_first_square(bb);
    bb = bb_clear_lsb(bb);
    return sq;
}

}  // namespace thinmint::core
