#pragma once

#include "thinmint/core/bitboard.h"
#include "thinmint/core/piece.h"

namespace thinmint::movegen {

// Precomputed pawn attack tables
// Index by [color][square] where color: 0=White, 1=Black
extern thinmint::core::Bitboard PAWN_ATTACKS[2][64];

// Initialize pawn attack tables (call once at startup)
void init_pawn_attacks();

// Get pawn attacks for a single pawn on a given square
inline thinmint::core::Bitboard pawn_attacks(thinmint::core::Square sq, thinmint::core::Color c) {
    return PAWN_ATTACKS[static_cast<int>(c)][sq];
}

// Get attacks for all pawns of a given color from a bitboard of pawn positions
thinmint::core::Bitboard all_pawn_attacks(thinmint::core::Bitboard pawns, thinmint::core::Color c);

// Get pawn single push destinations (non-promoting only)
thinmint::core::Bitboard pawn_pushes(thinmint::core::Bitboard pawns, thinmint::core::Bitboard occupancy, thinmint::core::Color c);

// Get pawn double push destinations (from starting rank only)
thinmint::core::Bitboard pawn_double_pushes(thinmint::core::Bitboard pawns, thinmint::core::Bitboard occupancy, thinmint::core::Color c);

// Get pawn promotion push destinations (from 7th rank)
thinmint::core::Bitboard pawn_promotion_pushes(thinmint::core::Bitboard pawns, thinmint::core::Bitboard occupancy, thinmint::core::Color c);

// Get pawn capture destinations (excluding promotion captures)
thinmint::core::Bitboard pawn_captures(thinmint::core::Bitboard pawns, thinmint::core::Bitboard enemy_pieces, thinmint::core::Color c);

// Get pawn promotion capture destinations (captures from 7th rank)
thinmint::core::Bitboard pawn_promotion_captures(thinmint::core::Bitboard pawns, thinmint::core::Bitboard enemy_pieces, thinmint::core::Color c);

// Get en passant target square from a double push destination
// Returns SQUARE_NONE if not a valid double push destination
inline thinmint::core::Square ep_square_from_double_push(thinmint::core::Square to_sq, thinmint::core::Color c) {
    using namespace thinmint::core;
    // For white: push goes from rank 2 to rank 4, ep square is on rank 3
    // For black: push goes from rank 7 to rank 5, ep square is on rank 6
    if (c == COLOR_WHITE) {
        return (rank_of(to_sq) == RANK_4) ? make_square(file_of(to_sq), RANK_3) : SQUARE_NONE;
    } else {
        return (rank_of(to_sq) == RANK_5) ? make_square(file_of(to_sq), RANK_6) : SQUARE_NONE;
    }
}

}  // namespace thinmint::movegen
