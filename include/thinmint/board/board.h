#pragma once

#include <array>
#include <cstdint>
#include <string>

#include "thinmint/core/bitboard.h"
#include "thinmint/core/piece.h"
#include "thinmint/core/square.h"

namespace thinmint::board {

using namespace thinmint::core;

// Board state represents a complete chess position
// Uses bitboards for piece placement and occupancy tracking
class BoardState {
public:
    // Piece bitboards indexed by [color][piece_type]
    // Access as pieces[color][piece_type]
    // Valid indices: pieces[0-1][1-6] (PIECE_NONE = 0 is unused)
    std::array<std::array<Bitboard, PIECE_TYPE_COUNT>, COLOR_COUNT> pieces;

    // Occupancy bitboards
    Bitboard white_occupancy;   // All white pieces
    Bitboard black_occupancy;   // All black pieces
    Bitboard all_occupancy;     // Combined occupancy

    // Game state
    Color side_to_move;         // Color to move next
    CastlingRights castling_rights;  // Available castling rights
    Square en_passant_square;   // Target square for EP capture, or SQUARE_NONE
    uint8_t halfmove_clock;     // Moves since last capture/pawn move (for 50-move rule)
    uint16_t fullmove_number;   // Full move counter (starts at 1, increments after Black moves)

    // Default constructor: creates empty board state (not a valid chess position)
    BoardState();

    // Reset to empty state (no pieces, no rights, White to move)
    void clear();

    // Reset to standard starting position
    void reset_to_start_position();

    // Get bitboard of a specific piece type and color
    Bitboard pieces_of(Color c, PieceType pt) const {
        return pieces[static_cast<size_t>(c)][static_cast<size_t>(pt)];
    }

    // Get occupancy for a specific color
    Bitboard occupancy_of(Color c) const {
        return (c == COLOR_WHITE) ? white_occupancy : black_occupancy;
    }

    // Check if a square is occupied
    bool is_square_occupied(Square sq) const {
        return bb_test(all_occupancy, sq);
    }

    // Check if a square is occupied by a specific color
    bool is_square_occupied_by(Square sq, Color c) const {
        return bb_test(occupancy_of(c), sq);
    }

    // Get piece at a square (returns PIECE_NO_PIECE if empty)
    Piece piece_at(Square sq) const;

    // Get piece type at a square (returns PIECE_NONE if empty)
    PieceType piece_type_at(Square sq) const;

    // Get color of piece at a square (returns WHITE for empty - check is_square_occupied first)
    Color color_at(Square sq) const;

    // Check if the position is empty (no pieces)
    bool is_empty() const {
        return all_occupancy == BB_EMPTY;
    }

    // Get king square for a color (returns SQUARE_NONE if no king - invalid position)
    Square king_square(Color c) const;

    // Check if king is in check (requires king to exist)
    bool is_check(Color c) const;

    // Check if position is valid (for debugging/validation)
    // Validates: exactly one king per side, no overlapping pieces, consistent occupancy
    bool is_valid() const;

    // Count total pieces on board
    int piece_count() const {
        return bb_popcount(all_occupancy);
    }

    // Count pieces of specific type and color
    int piece_count(Color c, PieceType pt) const {
        return bb_popcount(pieces_of(c, pt));
    }

    // Toggle side to move
    void toggle_side_to_move() {
        side_to_move = !side_to_move;
    }

    // Convert board to debug string representation
    std::string to_string() const;
};

// Standard starting position FEN
inline constexpr const char* START_POSITION_FEN =
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

}  // namespace thinmint::board
