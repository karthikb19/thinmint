#pragma once

#include "thinmint/board/board.h"
#include "thinmint/core/move.h"
#include "thinmint/core/piece.h"

#include <vector>

namespace thinmint::board {

// Undo state captures all information needed to restore a board position
// after a move has been made. This enables make/unmake operations for search.
struct UndoState {
    // The move that was made
    thinmint::core::Move move;

    // State that was modified by the move
    thinmint::core::CastlingRights castling_rights;
    thinmint::core::Square en_passant_square;
    uint8_t halfmove_clock;

    // Captured piece type (PIECE_NONE if no capture)
    thinmint::core::PieceType captured_piece_type;

    // Default constructor
    UndoState()
        : move(0),
          castling_rights(thinmint::core::CASTLING_NONE),
          en_passant_square(thinmint::core::SQUARE_NONE),
          halfmove_clock(0),
          captured_piece_type(thinmint::core::PIECE_NONE) {}
};

// Undo move stack for managing position history during search
// This is a lightweight container for UndoState entries
class UndoStack {
public:
    UndoStack();

    // Push an undo state onto the stack
    void push(const UndoState& state);

    // Pop an undo state from the stack (returns by value)
    UndoState pop();

    // Peek at the top state without removing it
    const UndoState& top() const;

    // Check if stack is empty
    bool empty() const { return stack_.empty(); }

    // Get current stack size
    size_t size() const { return stack_.size(); }

    // Clear the stack
    void clear();

private:
    std::vector<UndoState> stack_;
};

// Undo a previously made move, restoring the board to its prior state
//
// Preconditions:
// - The move must have been previously applied via make_move
// - The undo state must contain the state captured before the move
//
// Restores:
// - Piece bitboards (moves piece back, restores captured piece if any)
// - Castling rights (from undo state)
// - En passant square (from undo state)
// - Halfmove clock (from undo state)
// - Side to move (toggles back)
// - Fullmove number (decrements after Black's move)
// - Special handling for castling (moves rook back), EP (restores pawn), promotions (replaces piece)
//
// Handles all move types: quiet, captures, castling, en passant, promotions
void unmake_move(BoardState& board, const UndoState& undo);

// Combined make+undo helper that captures undo state automatically
// Returns the undo state for the caller to manage
UndoState make_move_with_undo(BoardState& board, thinmint::core::Move move);

}  // namespace thinmint::board
