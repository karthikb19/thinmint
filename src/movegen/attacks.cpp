#include "thinmint/movegen/attacks.h"

#include "thinmint/core/bitboard.h"

using namespace thinmint::core;

namespace thinmint::movegen {

// Static storage for precomputed pawn attacks
Bitboard PAWN_ATTACKS[2][64];

// Initialize pawn attack lookup table
void init_pawn_attacks() {
    using namespace thinmint::core;
    
    for (Square sq = 0; sq < 64; ++sq) {
        Bitboard bb = bb_from_square(sq);
        
        // White pawns attack northeast and northwest
        PAWN_ATTACKS[0][sq] = bb_shift_northeast(bb) | bb_shift_northwest(bb);
        
        // Black pawns attack southeast and southwest
        PAWN_ATTACKS[1][sq] = bb_shift_southeast(bb) | bb_shift_southwest(bb);
    }
}

// Get attacks for all pawns of a given color from a bitboard of pawn positions
Bitboard all_pawn_attacks(Bitboard pawns, Color c) {
    if (c == COLOR_WHITE) {
        return bb_shift_northeast(pawns) | bb_shift_northwest(pawns);
    } else {
        return bb_shift_southeast(pawns) | bb_shift_southwest(pawns);
    }
}

// Get pawn single push destinations
// White pushes north, Black pushes south
// Pawns can only push to empty squares
Bitboard pawn_pushes(Bitboard pawns, Bitboard occupancy, Color c) {
    if (c == COLOR_WHITE) {
        // White pushes from ranks 2-6 (not 7 - that would be promotion)
        Bitboard non_promoting = pawns & ~BB_RANK_7;
        Bitboard pushes = bb_shift_north(non_promoting) & ~occupancy;
        return pushes;
    } else {
        // Black pushes from ranks 7-3 (not 2 - that would be promotion)
        Bitboard non_promoting = pawns & ~BB_RANK_2;
        Bitboard pushes = bb_shift_south(non_promoting) & ~occupancy;
        return pushes;
    }
}

// Get pawn double push destinations (from starting rank only)
// White starts on rank 2, Black on rank 7
// Both the intermediate square and destination must be empty
Bitboard pawn_double_pushes(Bitboard pawns, Bitboard occupancy, Color c) {
    if (c == COLOR_WHITE) {
        // White double push: from rank 2 to rank 4
        Bitboard on_start_rank = pawns & BB_RANK_2;
        // First push must be valid (rank 3 is empty)
        Bitboard first_push = bb_shift_north(on_start_rank) & ~occupancy;
        // Second push must also be valid (rank 4 is empty)
        Bitboard second_push = bb_shift_north(first_push) & ~occupancy;
        return second_push;
    } else {
        // Black double push: from rank 7 to rank 5
        Bitboard on_start_rank = pawns & BB_RANK_7;
        Bitboard first_push = bb_shift_south(on_start_rank) & ~occupancy;
        Bitboard second_push = bb_shift_south(first_push) & ~occupancy;
        return second_push;
    }
}

// Get pawn promotion push destinations (from 7th rank for white, 2nd rank for black)
Bitboard pawn_promotion_pushes(Bitboard pawns, Bitboard occupancy, Color c) {
    if (c == COLOR_WHITE) {
        Bitboard on_promotion_rank = pawns & BB_RANK_7;
        return bb_shift_north(on_promotion_rank) & ~occupancy;
    } else {
        Bitboard on_promotion_rank = pawns & BB_RANK_2;
        return bb_shift_south(on_promotion_rank) & ~occupancy;
    }
}

// Get pawn capture destinations (non-promotion captures only)
Bitboard pawn_captures(Bitboard pawns, Bitboard enemy_pieces, Color c) {
    if (c == COLOR_WHITE) {
        Bitboard non_promoting = pawns & ~BB_RANK_7;
        Bitboard attacks = bb_shift_northeast(non_promoting) | bb_shift_northwest(non_promoting);
        return attacks & enemy_pieces;
    } else {
        Bitboard non_promoting = pawns & ~BB_RANK_2;
        Bitboard attacks = bb_shift_southeast(non_promoting) | bb_shift_southwest(non_promoting);
        return attacks & enemy_pieces;
    }
}

// Get pawn promotion capture destinations (captures from 7th/2nd rank)
Bitboard pawn_promotion_captures(Bitboard pawns, Bitboard enemy_pieces, Color c) {
    if (c == COLOR_WHITE) {
        Bitboard on_promotion_rank = pawns & BB_RANK_7;
        Bitboard attacks = bb_shift_northeast(on_promotion_rank) | bb_shift_northwest(on_promotion_rank);
        return attacks & enemy_pieces;
    } else {
        Bitboard on_promotion_rank = pawns & BB_RANK_2;
        Bitboard attacks = bb_shift_southeast(on_promotion_rank) | bb_shift_southwest(on_promotion_rank);
        return attacks & enemy_pieces;
    }
}

}  // namespace thinmint::movegen
