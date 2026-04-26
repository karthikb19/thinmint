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
// - Castling is NOT included here (added in S02-F06)
//
// Returns the number of moves generated
size_t generate_pseudo_legal_moves(const thinmint::board::BoardState& board, MoveList& moves);

// Generate only capture moves (for quiescence search later)
// Includes promotion captures
size_t generate_captures(const thinmint::board::BoardState& board, MoveList& moves);

// Generate only quiet (non-capture) moves
// Includes promotion pushes (these are technically non-captures)
size_t generate_quiet_moves(const thinmint::board::BoardState& board, MoveList& moves);

}  // namespace thinmint::movegen
