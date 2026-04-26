#include "thinmint/perft/perft.h"

#include <iostream>

#include "thinmint/core/move.h"
#include "thinmint/movegen/movegen.h"
#include "thinmint/movegen/attacks.h"

namespace thinmint::perft_ns {

using namespace thinmint::core;
using namespace thinmint::board;
using namespace thinmint::movegen;

namespace {

// Make a move on a board and return the new state
// This is a simplified make_move for perft only
// Full make/unmake with undo stack comes in Sprint 03
BoardState make_move_copy(const BoardState& board, Move move) {
    BoardState new_board = board;

    Square from_sq = from_square(move);
    Square to_sq = to_square(move);
    PieceType moved_pt = board.piece_type_at(from_sq);
    Color us = board.side_to_move;
    Color them = !us;

    Bitboard from_bb = bb_from_square(from_sq);
    Bitboard to_bb = bb_from_square(to_sq);

    // Handle special moves
    bool is_ep = is_en_passant(move);
    bool is_castle = is_castling(move);
    bool is_promo = is_promotion(move);

    // Remove piece from source
    new_board.pieces[static_cast<size_t>(us)][static_cast<size_t>(moved_pt)] &= ~from_bb;

    // Handle capture at destination (except EP which captures elsewhere)
    if (!is_ep && board.is_square_occupied(to_sq)) {
        PieceType captured = board.piece_type_at(to_sq);
        new_board.pieces[static_cast<size_t>(them)][static_cast<size_t>(captured)] &= ~to_bb;
    }

    // Place piece at destination
    if (is_promo) {
        PieceType promo_pt = promotion_type(move);
        new_board.pieces[static_cast<size_t>(us)][static_cast<size_t>(promo_pt)] |= to_bb;
    } else if (is_castle) {
        // Place king
        new_board.pieces[static_cast<size_t>(us)][static_cast<size_t>(PIECE_KING)] |= to_bb;

        // Move rook
        Square rook_from, rook_to;
        if (to_sq == SQUARE_G1) {  // White O-O
            rook_from = SQUARE_H1;
            rook_to = SQUARE_F1;
        } else if (to_sq == SQUARE_C1) {  // White O-O-O
            rook_from = SQUARE_A1;
            rook_to = SQUARE_D1;
        } else if (to_sq == SQUARE_G8) {  // Black O-O
            rook_from = SQUARE_H8;
            rook_to = SQUARE_F8;
        } else {  // Black O-O-O
            rook_from = SQUARE_A8;
            rook_to = SQUARE_D8;
        }

        Bitboard rook_from_bb = bb_from_square(rook_from);
        Bitboard rook_to_bb = bb_from_square(rook_to);

        new_board.pieces[static_cast<size_t>(us)][static_cast<size_t>(PIECE_ROOK)] &= ~rook_from_bb;
        new_board.pieces[static_cast<size_t>(us)][static_cast<size_t>(PIECE_ROOK)] |= rook_to_bb;
    } else if (is_ep) {
        // Place pawn at EP square
        new_board.pieces[static_cast<size_t>(us)][static_cast<size_t>(PIECE_PAWN)] |= to_bb;

        // Remove captured pawn
        Rank cap_rank = (us == COLOR_WHITE) ? RANK_5 : RANK_4;
        Square captured_sq = make_square(file_of(to_sq), cap_rank);
        Bitboard captured_bb = bb_from_square(captured_sq);
        new_board.pieces[static_cast<size_t>(them)][static_cast<size_t>(PIECE_PAWN)] &= ~captured_bb;
    } else {
        // Normal move - place moved piece
        new_board.pieces[static_cast<size_t>(us)][static_cast<size_t>(moved_pt)] |= to_bb;
    }

    // Update occupancy
    new_board.white_occupancy = BB_EMPTY;
    new_board.black_occupancy = BB_EMPTY;
    for (size_t pt = 1; pt < PIECE_TYPE_COUNT; ++pt) {
        new_board.white_occupancy |= new_board.pieces[0][pt];
        new_board.black_occupancy |= new_board.pieces[1][pt];
    }
    new_board.all_occupancy = new_board.white_occupancy | new_board.black_occupancy;

    // Update side to move
    new_board.side_to_move = them;

    // Update castling rights
    // King move removes all castling rights for that side
    if (moved_pt == PIECE_KING) {
        if (us == COLOR_WHITE) {
            new_board.castling_rights = static_cast<CastlingRights>(
                new_board.castling_rights & ~(CASTLING_WHITE_KINGSIDE | CASTLING_WHITE_QUEENSIDE));
        } else {
            new_board.castling_rights = static_cast<CastlingRights>(
                new_board.castling_rights & ~(CASTLING_BLACK_KINGSIDE | CASTLING_BLACK_QUEENSIDE));
        }
    }

    // Rook move removes corresponding castling right
    if (moved_pt == PIECE_ROOK) {
        if (us == COLOR_WHITE) {
            if (from_sq == SQUARE_H1) {
                new_board.castling_rights = static_cast<CastlingRights>(
                    new_board.castling_rights & ~CASTLING_WHITE_KINGSIDE);
            } else if (from_sq == SQUARE_A1) {
                new_board.castling_rights = static_cast<CastlingRights>(
                    new_board.castling_rights & ~CASTLING_WHITE_QUEENSIDE);
            }
        } else {
            if (from_sq == SQUARE_H8) {
                new_board.castling_rights = static_cast<CastlingRights>(
                    new_board.castling_rights & ~CASTLING_BLACK_KINGSIDE);
            } else if (from_sq == SQUARE_A8) {
                new_board.castling_rights = static_cast<CastlingRights>(
                    new_board.castling_rights & ~CASTLING_BLACK_QUEENSIDE);
            }
        }
    }

    // Rook capture removes opponent's castling right
    if (board.is_square_occupied(to_sq) && !is_ep) {
        PieceType captured = board.piece_type_at(to_sq);
        if (captured == PIECE_ROOK) {
            if (them == COLOR_WHITE) {
                if (to_sq == SQUARE_H1) {
                    new_board.castling_rights = static_cast<CastlingRights>(
                        new_board.castling_rights & ~CASTLING_WHITE_KINGSIDE);
                } else if (to_sq == SQUARE_A1) {
                    new_board.castling_rights = static_cast<CastlingRights>(
                        new_board.castling_rights & ~CASTLING_WHITE_QUEENSIDE);
                }
            } else {
                if (to_sq == SQUARE_H8) {
                    new_board.castling_rights = static_cast<CastlingRights>(
                        new_board.castling_rights & ~CASTLING_BLACK_KINGSIDE);
                } else if (to_sq == SQUARE_A8) {
                    new_board.castling_rights = static_cast<CastlingRights>(
                        new_board.castling_rights & ~CASTLING_BLACK_QUEENSIDE);
                }
            }
        }
    }

    // Update en passant square
    if (moved_pt == PIECE_PAWN) {
        int rank_diff = static_cast<int>(rank_of(to_sq)) - static_cast<int>(rank_of(from_sq));
        if (abs(rank_diff) == 2) {
            // Double pawn push - set EP square
            Rank ep_rank = (us == COLOR_WHITE) ? RANK_3 : RANK_6;
            File ep_file = file_of(from_sq);
            new_board.en_passant_square = make_square(ep_file, ep_rank);
        } else {
            new_board.en_passant_square = SQUARE_NONE;
        }
    } else {
        new_board.en_passant_square = SQUARE_NONE;
    }

    // Update halfmove clock
    if (moved_pt == PIECE_PAWN || board.is_square_occupied(to_sq)) {
        new_board.halfmove_clock = 0;
    } else {
        new_board.halfmove_clock++;
    }

    // Update fullmove number
    if (us == COLOR_BLACK) {
        new_board.fullmove_number++;
    }

    return new_board;
}

// Internal recursive perft function
uint64_t perft_recursive(BoardState& board, int depth) {
    if (depth == 0) {
        return 1ULL;
    }

    MoveList moves;
    generate_legal_moves(board, moves);

    if (depth == 1) {
        return static_cast<uint64_t>(moves.size());
    }

    uint64_t nodes = 0;
    for (size_t i = 0; i < moves.size(); ++i) {
        BoardState new_board = make_move_copy(board, moves[i]);
        nodes += perft_recursive(new_board, depth - 1);
    }

    return nodes;
}

}  // namespace

// Standard perft: count all leaf nodes at the given depth
uint64_t perft(BoardState& board, int depth) {
    if (depth < 0) {
        return 0;
    }
    if (depth == 0) {
        return 1ULL;
    }
    return perft_recursive(board, depth);
}

// Perft with node counting but no output (same as perft for now)
uint64_t perft_bulk(BoardState& board, int depth) {
    return perft(board, depth);
}

// Perft "divide": show count per first move
uint64_t perft_divide(BoardState& board, int depth, bool print) {
    if (depth <= 0) {
        return 1ULL;
    }

    MoveList moves;
    generate_legal_moves(board, moves);

    if (depth == 1) {
        if (print) {
            for (size_t i = 0; i < moves.size(); ++i) {
                char buf[6];
                std::string uci = move_to_uci(moves[i]);
                std::copy(uci.begin(), uci.end(), buf);
                buf[uci.length()] = '\0';
                std::cout << buf << ": 1\n";
            }
        }
        return static_cast<uint64_t>(moves.size());
    }

    uint64_t total = 0;

    for (size_t i = 0; i < moves.size(); ++i) {
        BoardState new_board = make_move_copy(board, moves[i]);
        uint64_t nodes = perft_recursive(new_board, depth - 1);
        total += nodes;

        if (print) {
            std::cout << move_to_uci(moves[i]) << ": " << nodes << "\n";
        }
    }

    if (print) {
        std::cout << "\nNodes searched: " << total << "\n";
    }

    return total;
}

// Helper to run perft on a FEN string
uint64_t perft_fen(const std::string& fen, int depth) {
    BoardState board;
    if (!board.parse_fen(fen)) {
        return 0;
    }
    return perft(board, depth);
}

// Detailed perft (simplified - just returns node count for now)
PerftResult perft_detailed(BoardState& board, int depth) {
    PerftResult result;
    result.nodes = perft(board, depth);
    return result;
}

}  // namespace thinmint::perft_ns
