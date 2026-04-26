#pragma once

#include "thinmint/board/board.h"
#include "thinmint/core/move.h"

namespace thinmint::board {

// Apply a legal move to the board state
// This function modifies the board state in place
//
// Preconditions:
// - The move must be legal in the current position
// - The board state must be valid before calling
//
// Updates made:
// - Piece bitboards (moves piece from source to destination)
// - Captured pieces are removed from opponent's bitboards
// - Occupancy bitboards are updated
// - Side to move is toggled
// - Halfmove clock is incremented (reset on pawn move or capture)
// - Fullmove number is incremented after Black's move
// - En passant square is set/updated for double pawn pushes
// - Castling rights are updated when king or rooks move
//
// Note: Special moves (castling, en passant, promotion) are handled
// by S03-F02. This function handles quiet moves and captures.
void make_move(BoardState& board, thinmint::core::Move move);

}  // namespace thinmint::board
