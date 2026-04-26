#include "thinmint/board/makemove.h"

#include <cassert>

namespace thinmint::board {

using namespace thinmint::core;

namespace {

// Get the castling rook move for a castling move
// Returns the rook's from and to squares
void get_castling_rook_move(Square king_from, Square king_to, Square& rook_from, Square& rook_to) {
    // Determine which side we're castling based on king destination
    if (king_to > king_from) {
        // Kingside castling (king moves to g-file)
        rook_from = static_cast<Square>(king_from + 3);  // H-file
        rook_to = static_cast<Square>(king_from + 1);    // F-file
    } else {
        // Queenside castling (king moves to c-file)
        rook_from = static_cast<Square>(king_from - 4);  // A-file
        rook_to = static_cast<Square>(king_from - 1);    // D-file
    }
}

// Update castling rights when a piece moves from a square
// piece_moved: the type of piece that moved (needed because piece may already be moved from 'from' square)
void update_castling_rights_from(BoardState& board, Square from, PieceType piece_moved) {
    // If king moves, lose all castling rights for that side
    if (piece_moved == PIECE_KING) {
        if (board.side_to_move == COLOR_WHITE) {
            board.castling_rights = static_cast<CastlingRights>(
                board.castling_rights & ~(CASTLING_WHITE_KINGSIDE | CASTLING_WHITE_QUEENSIDE));
        } else {
            board.castling_rights = static_cast<CastlingRights>(
                board.castling_rights & ~(CASTLING_BLACK_KINGSIDE | CASTLING_BLACK_QUEENSIDE));
        }
        return;
    }

    // Check if a rook moves from its starting square
    if (board.side_to_move == COLOR_WHITE) {
        if (from == SQUARE_H1) {
            board.castling_rights = static_cast<CastlingRights>(
                board.castling_rights & ~CASTLING_WHITE_KINGSIDE);
        } else if (from == SQUARE_A1) {
            board.castling_rights = static_cast<CastlingRights>(
                board.castling_rights & ~CASTLING_WHITE_QUEENSIDE);
        }
    } else {
        if (from == SQUARE_H8) {
            board.castling_rights = static_cast<CastlingRights>(
                board.castling_rights & ~CASTLING_BLACK_KINGSIDE);
        } else if (from == SQUARE_A8) {
            board.castling_rights = static_cast<CastlingRights>(
                board.castling_rights & ~CASTLING_BLACK_QUEENSIDE);
        }
    }
}

// Update castling rights when a rook is captured
void update_castling_rights_for_capture(BoardState& board, Square to) {
    Color opponent = !board.side_to_move;

    if (opponent == COLOR_WHITE) {
        if (to == SQUARE_H1) {
            board.castling_rights = static_cast<CastlingRights>(
                board.castling_rights & ~CASTLING_WHITE_KINGSIDE);
        } else if (to == SQUARE_A1) {
            board.castling_rights = static_cast<CastlingRights>(
                board.castling_rights & ~CASTLING_WHITE_QUEENSIDE);
        }
    } else {
        if (to == SQUARE_H8) {
            board.castling_rights = static_cast<CastlingRights>(
                board.castling_rights & ~CASTLING_BLACK_KINGSIDE);
        } else if (to == SQUARE_A8) {
            board.castling_rights = static_cast<CastlingRights>(
                board.castling_rights & ~CASTLING_BLACK_QUEENSIDE);
        }
    }
}

}  // namespace

void make_move(BoardState& board, Move move) {
    // Extract move information
    Square from = from_square(move);
    Square to = to_square(move);
    MoveFlags flags = move_flags(move);

    Color us = board.side_to_move;
    Color them = !us;

    // Determine the piece type being moved
    PieceType moving_pt = board.piece_type_at(from);
    assert(moving_pt != PIECE_NONE && "make_move: no piece at from square");

    // Check if this is a capture (remove captured piece first)
    bool is_cap = is_capture(move);
    bool is_promo = is_promotion(move);
    bool is_cast = is_castling(move);
    bool is_ep = is_en_passant(move);

    // For en passant, the captured pawn is not on the 'to' square
    Square captured_square = to;
    if (is_ep) {
        // The captured pawn is one square behind the EP target
        captured_square = (us == COLOR_WHITE)
            ? static_cast<Square>(to - 8)   // Black pawn on rank 5
            : static_cast<Square>(to + 8);  // White pawn on rank 4
    }

    if (is_cap || is_ep) {

        // Remove captured piece from opponent's bitboards
        for (size_t pt = 1; pt < PIECE_TYPE_COUNT; ++pt) {
            if (bb_test(board.pieces[static_cast<size_t>(them)][pt], captured_square)) {
                board.pieces[static_cast<size_t>(them)][pt] &= ~bb_from_square(captured_square);
                break;
            }
        }

        // Update opponent's occupancy
        if (them == COLOR_WHITE) {
            board.white_occupancy &= ~bb_from_square(captured_square);
        } else {
            board.black_occupancy &= ~bb_from_square(captured_square);
        }

        // Update castling rights if a rook was captured
        if (!is_ep) {
            update_castling_rights_for_capture(board, to);
        }
    }

    // Handle castling: move the rook as well
    if (is_cast) {
        Square rook_from, rook_to;
        get_castling_rook_move(from, to, rook_from, rook_to);

        // Move rook
        board.pieces[static_cast<size_t>(us)][static_cast<size_t>(PIECE_ROOK)] &= ~bb_from_square(rook_from);
        board.pieces[static_cast<size_t>(us)][static_cast<size_t>(PIECE_ROOK)] |= bb_from_square(rook_to);

        // Update occupancy for rook
        Bitboard rook_move_bb = bb_from_square(rook_from) | bb_from_square(rook_to);
        if (us == COLOR_WHITE) {
            board.white_occupancy ^= rook_move_bb;
        } else {
            board.black_occupancy ^= rook_move_bb;
        }
        board.all_occupancy ^= rook_move_bb;
    }

    // Remove piece from source square
    if (!is_promo) {
        // Normal move: move the same piece type
        board.pieces[static_cast<size_t>(us)][static_cast<size_t>(moving_pt)] &= ~bb_from_square(from);
        board.pieces[static_cast<size_t>(us)][static_cast<size_t>(moving_pt)] |= bb_from_square(to);
    } else {
        // Promotion: remove pawn, add promoted piece
        board.pieces[static_cast<size_t>(us)][static_cast<size_t>(PIECE_PAWN)] &= ~bb_from_square(from);
        PieceType promo_pt = promotion_type(move);
        board.pieces[static_cast<size_t>(us)][static_cast<size_t>(promo_pt)] |= bb_from_square(to);
    }

    // Update our occupancy
    Bitboard move_bb = bb_from_square(from) | bb_from_square(to);
    if (us == COLOR_WHITE) {
        board.white_occupancy ^= move_bb;
    } else {
        board.black_occupancy ^= move_bb;
    }

    // Update total occupancy
    if (is_ep) {
        // En passant: captured piece is on a different square
        // Clear 'from', clear captured square, set 'to'
        board.all_occupancy &= ~bb_from_square(from);
        board.all_occupancy &= ~bb_from_square(captured_square);
        board.all_occupancy |= bb_from_square(to);
    } else if (is_cap) {
        // Regular capture: destination square was occupied by opponent
        // We need to clear the 'from' bit and set the 'to' bit
        // The captured piece was cleared above from opponent's occupancy
        board.all_occupancy &= ~bb_from_square(from);
        board.all_occupancy |= bb_from_square(to);
    } else {
        // Non-capture: XOR handles the move
        board.all_occupancy ^= move_bb;
    }

    // Update castling rights if king or rook moved
    // Pass the piece type before it was moved
    update_castling_rights_from(board, from, moving_pt);

    // Update en passant square
    if (moving_pt == PIECE_PAWN && std::abs(static_cast<int>(to) - static_cast<int>(from)) == 16) {
        // Double pawn push: set EP square to the square the pawn passed through
        board.en_passant_square = (us == COLOR_WHITE)
            ? static_cast<Square>(from + 8)   // Passed through rank 3
            : static_cast<Square>(from - 8);  // Passed through rank 6
    } else {
        board.en_passant_square = SQUARE_NONE;
    }

    // Update halfmove clock
    if (moving_pt == PIECE_PAWN || is_cap || is_ep) {
        board.halfmove_clock = 0;  // Reset on pawn move or capture
    } else {
        ++board.halfmove_clock;
    }

    // Update fullmove number
    if (us == COLOR_BLACK) {
        ++board.fullmove_number;
    }

    // Toggle side to move
    board.side_to_move = them;
}

}  // namespace thinmint::board
