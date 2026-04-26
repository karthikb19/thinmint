#include "thinmint/board/unmakemove.h"
#include "thinmint/board/makemove.h"

#include <cassert>

namespace thinmint::board {

using namespace thinmint::core;

namespace {

// Get the castling rook move for a castling move
// Returns the rook's from and to squares
// Works for both make and unmake - uses king's displacement direction
void get_castling_rook_move(Square king_from, Square king_to, Square& rook_from, Square& rook_to) {
    // Compare king's destination to origin to determine castling direction
    if (king_to > king_from) {
        // Kingside castling (king moved right)
        // Rook was on H-file (relative to king start), goes to F-file
        rook_from = static_cast<Square>(king_from + 3);  // H-file
        rook_to = static_cast<Square>(king_from + 1);    // F-file
    } else {
        // Queenside castling (king moved left)
        // Rook was on A-file (relative to king start), goes to D-file
        rook_from = static_cast<Square>(king_from - 4);  // A-file
        rook_to = static_cast<Square>(king_from - 1);    // D-file
    }
}

}  // namespace

// UndoStack implementation
UndoStack::UndoStack() = default;

void UndoStack::push(const UndoState& state) {
    stack_.push_back(state);
}

UndoState UndoStack::pop() {
    assert(!stack_.empty() && "UndoStack::pop called on empty stack");
    UndoState state = stack_.back();
    stack_.pop_back();
    return state;
}

const UndoState& UndoStack::top() const {
    assert(!stack_.empty() && "UndoStack::top called on empty stack");
    return stack_.back();
}

void UndoStack::clear() {
    stack_.clear();
}

// unmake_move implementation
void unmake_move(BoardState& board, const UndoState& undo) {
    Move move = undo.move;
    Square from = from_square(move);
    Square to = to_square(move);

    // We are undoing, so "us" is now the side that just moved (the opponent of current side_to_move)
    // When undoing, "us" is the side that just made the move
    // The current side_to_move is the opponent who will move next
    Color us = !board.side_to_move;  // Side that made the move (being undone)
    Color them = board.side_to_move; // Side that will move next (opponent of the move being undone)

    bool is_cap = is_capture(move);
    bool is_promo = is_promotion(move);
    bool is_cast = is_castling(move);
    bool is_ep = is_en_passant(move);

    // Step 1: Restore board state fields
    board.castling_rights = undo.castling_rights;
    board.en_passant_square = undo.en_passant_square;
    board.halfmove_clock = undo.halfmove_clock;

    // Step 2: Decrement fullmove number if Black was the side that moved
    if (us == COLOR_BLACK) {
        --board.fullmove_number;
    }

    // Step 3: Restore the moving piece
    // For promotions: remove promoted piece, restore pawn
    // For normal moves: move piece back
    if (!is_promo) {
        // Normal move: move piece back from 'to' to 'from'
        PieceType moving_pt = board.piece_type_at(to);
        assert(moving_pt != PIECE_NONE && "unmake_move: no piece at destination square");

        board.pieces[static_cast<size_t>(us)][static_cast<size_t>(moving_pt)] &= ~bb_from_square(to);
        board.pieces[static_cast<size_t>(us)][static_cast<size_t>(moving_pt)] |= bb_from_square(from);
    } else {
        // Promotion: remove promoted piece, restore pawn at 'from'
        PieceType promo_pt = board.piece_type_at(to);
        assert(promo_pt != PIECE_NONE && promo_pt != PIECE_PAWN && "unmake_move: invalid promoted piece");

        board.pieces[static_cast<size_t>(us)][static_cast<size_t>(promo_pt)] &= ~bb_from_square(to);
        board.pieces[static_cast<size_t>(us)][static_cast<size_t>(PIECE_PAWN)] |= bb_from_square(from);
    }

    // Step 4: Restore captured piece if any
    if (is_cap || is_ep) {
        // Determine where the captured piece was
        Square captured_square = to;
        if (is_ep) {
            // En passant: captured pawn is one square behind EP target
            captured_square = (us == COLOR_WHITE)
                ? static_cast<Square>(to - 8)   // Black pawn on rank 5
                : static_cast<Square>(to + 8);  // White pawn on rank 4
        }

        // Restore the captured piece
        assert(undo.captured_piece_type != PIECE_NONE && "unmake_move: capture move but no captured piece type recorded");
        board.pieces[static_cast<size_t>(them)][static_cast<size_t>(undo.captured_piece_type)] |= bb_from_square(captured_square);

        // Update opponent's occupancy
        if (them == COLOR_WHITE) {
            board.white_occupancy |= bb_from_square(captured_square);
        } else {
            board.black_occupancy |= bb_from_square(captured_square);
        }
    }

    // Step 5: Handle castling (move rook back)
    if (is_cast) {
        Square rook_from, rook_to;
        get_castling_rook_move(from, to, rook_from, rook_to);

        // Move rook back
        board.pieces[static_cast<size_t>(us)][static_cast<size_t>(PIECE_ROOK)] &= ~bb_from_square(rook_to);
        board.pieces[static_cast<size_t>(us)][static_cast<size_t>(PIECE_ROOK)] |= bb_from_square(rook_from);

        // Update occupancy for rook
        Bitboard rook_move_bb = bb_from_square(rook_from) | bb_from_square(rook_to);
        if (us == COLOR_WHITE) {
            board.white_occupancy ^= rook_move_bb;
        } else {
            board.black_occupancy ^= rook_move_bb;
        }
        board.all_occupancy ^= rook_move_bb;
    }

    // Step 6: Update our occupancy
    Bitboard move_bb = bb_from_square(from) | bb_from_square(to);
    if (us == COLOR_WHITE) {
        board.white_occupancy ^= move_bb;
    } else {
        board.black_occupancy ^= move_bb;
    }

    // Step 7: Update total occupancy
    if (is_ep) {
        // En passant: we need to restore from, captured_square, and clear to
        Square captured_square = (us == COLOR_WHITE)
            ? static_cast<Square>(to - 8)
            : static_cast<Square>(to + 8);
        board.all_occupancy |= bb_from_square(from);
        board.all_occupancy |= bb_from_square(captured_square);
        board.all_occupancy &= ~bb_from_square(to);
    } else if (is_promo && is_cap) {
        // Promotion capture: promoted piece removed from 'to', pawn restored at 'from',
        // captured piece restored at 'to'
        board.all_occupancy |= bb_from_square(from);
        board.all_occupancy |= bb_from_square(to);
    } else if (is_promo) {
        // Quiet promotion: promoted piece removed from 'to', pawn restored at 'from'
        board.all_occupancy |= bb_from_square(from);
        board.all_occupancy &= ~bb_from_square(to);
    } else if (is_cap) {
        // Regular capture: piece moved back from 'to' to 'from',
        // captured piece restored at 'to'
        // After step 3, 'from' is occupied, 'to' is empty
        // After step 4, 'to' is occupied by captured piece
        // So both squares should be set
        board.all_occupancy |= bb_from_square(from);
        // 'to' should already be set from make_move, stays set after captured piece restored
    } else {
        // Non-capture: XOR handles the move back
        board.all_occupancy ^= move_bb;
    }

    // Step 8: Toggle side to move back
    board.side_to_move = us;
}

// make_move_with_undo helper
UndoState make_move_with_undo(BoardState& board, Move move) {
    UndoState undo;
    undo.move = move;
    undo.castling_rights = board.castling_rights;
    undo.en_passant_square = board.en_passant_square;
    undo.halfmove_clock = board.halfmove_clock;

    // Determine captured piece type before making the move
    Square to = to_square(move);
    Color them = !board.side_to_move;

    if (is_en_passant(move)) {
        // En passant: captured pawn is not on 'to' square
        undo.captured_piece_type = PIECE_PAWN;
    } else if (is_capture(move)) {
        // Regular capture: check what's on the destination square
        undo.captured_piece_type = board.piece_type_at(to);
    } else {
        undo.captured_piece_type = PIECE_NONE;
    }

    // Now make the move
    make_move(board, move);

    return undo;
}

}  // namespace thinmint::board
