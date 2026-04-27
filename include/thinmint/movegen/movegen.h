#pragma once

#include <cstddef>
#include <cstdint>

#include "thinmint/core/move.h"
#include "thinmint/board/board.h"

namespace thinmint::movegen {

// Maximum number of legal moves in any chess position
// Upper bound is ~218 (typical max is lower, but promotions increase count)
inline constexpr size_t MAX_MOVES = 256;

// Simple fixed-size move list for efficiency
// Uses inline storage to avoid heap allocations
class MoveList {
public:
    // Default constructor: empty list
    MoveList() : count_(0) {}

    // Add a move to the list
    void push(thinmint::core::Move m) {
        if (count_ < MAX_MOVES) {
            moves_[count_++] = m;
        }
    }

    // Get number of moves in the list
    size_t size() const { return count_; }

    // Check if list is empty
    bool empty() const { return count_ == 0; }

    // Clear the list
    void clear() { count_ = 0; }

    // Array subscript access
    thinmint::core::Move operator[](size_t index) const {
        return moves_[index];
    }

    // Iterator support
    const thinmint::core::Move* begin() const { return moves_; }
    const thinmint::core::Move* end() const { return moves_ + count_; }

    // Get last move (useful for pop)
    thinmint::core::Move back() const {
        return (count_ > 0) ? moves_[count_ - 1] : thinmint::core::MOVE_NONE;
    }

    // Remove last move
    void pop() {
        if (count_ > 0) {
            --count_;
        }
    }

private:
    thinmint::core::Move moves_[MAX_MOVES];
    size_t count_;
};

// Generate all pseudo-legal moves for the current side to move
// Pseudo-legal means moves that are legal assuming the king is not in check
// and ignoring pins. These are filtered later to produce fully legal moves.
//
// This generates:
// - All piece moves (pawn, knight, bishop, rook, queen, king)
// - Captures and quiet moves
// - Promotions (all four promotion piece types)
// - En passant (if EP square is set)
// - Castling (if rights allow and path is clear and king is safe)
//
// Returns the number of moves generated
size_t generate_pseudo_legal_moves(const thinmint::board::BoardState& board, MoveList& moves);

// Generate castling moves only
// Checks castling rights, rook presence, clear path, and king safety
// This is separated for testing and potential use in legal move filtering
size_t generate_castling_moves(const thinmint::board::BoardState& board, MoveList& moves);

// Generate only capture moves (for quiescence search later)
// Includes promotion captures
size_t generate_captures(const thinmint::board::BoardState& board, MoveList& moves);

// Generate only legal capture moves (for quiescence search)
// This filters pseudo-legal captures to ensure legality
size_t generate_legal_captures(const thinmint::board::BoardState& board, MoveList& moves);

// Generate only quiet (non-capture) moves
// Includes promotion pushes (these are technically non-captures)
size_t generate_quiet_moves(const thinmint::board::BoardState& board, MoveList& moves);

// Generate all LEGAL moves for the current side to move
// This filters pseudo-legal moves to ensure the king is never left in check
// Handles:
// - Pinned pieces that cannot move off the pin ray
// - Check evasions (king moves, captures of checking piece, blocking sliding checks)
// - Double check (only king moves allowed)
// - En passant that would reveal a check (discovered check via EP capture)
//
// This is the function to use for actual game play and search
size_t generate_legal_moves(const thinmint::board::BoardState& board, MoveList& moves);

// Check if a specific pseudo-legal move is legal
// This is useful for UCI move validation when receiving moves from GUI
// Returns true if the move does not leave the king in check
bool is_move_legal(const thinmint::board::BoardState& board, thinmint::core::Move move);

// Get checkers bitboard - all pieces giving check to the king of the given color
// Returns a bitboard with all checking pieces (0, 1, or 2+ pieces)
thinmint::core::Bitboard get_checkers(const thinmint::board::BoardState& board, thinmint::core::Color king_color);

}  // namespace thinmint::movegen
