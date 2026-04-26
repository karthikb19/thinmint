#include "thinmint/movegen/movegen.h"

#include "thinmint/movegen/attacks.h"

namespace thinmint::movegen {

using namespace thinmint::core;
using namespace thinmint::board;

namespace {

// Helper to add a move to the list
inline void add_move(MoveList& moves, Square from, Square to, MoveFlags flags) {
    moves.push(make_move(from, to, flags));
}

// Generate pawn moves (pushes and captures)
void generate_pawn_moves(const BoardState& board, MoveList& moves, Color us, Color them) {
    Bitboard our_pawns = board.pieces_of(us, PIECE_PAWN);
    Bitboard their_pieces = board.occupancy_of(them);
    Bitboard empty_squares = ~board.all_occupancy;

    if (us == COLOR_WHITE) {
        // White pawn pushes (single and double)
        // Single pushes from ranks 2-6 (not promoting)
        Bitboard non_promoting_pawns = our_pawns & ~BB_RANK_7;
        Bitboard single_pushes = bb_shift_north(non_promoting_pawns) & empty_squares;

        // Generate single push moves
        while (single_pushes) {
            Square to_sq = bb_pop_square(single_pushes);
            Square from_sq = static_cast<Square>(to_sq - 8);
            add_move(moves, from_sq, to_sq, MoveFlags::QUIET);
        }

        // Double pushes from rank 2
        Bitboard rank2_pawns = our_pawns & BB_RANK_2;
        Bitboard push1 = bb_shift_north(rank2_pawns) & empty_squares;
        Bitboard double_pushes = bb_shift_north(push1) & empty_squares;

        while (double_pushes) {
            Square to_sq = bb_pop_square(double_pushes);
            Square from_sq = static_cast<Square>(to_sq - 16);
            add_move(moves, from_sq, to_sq, MoveFlags::QUIET);
        }

        // Promotion pushes from rank 7
        Bitboard promoting_pawns = our_pawns & BB_RANK_7;
        Bitboard promo_pushes = bb_shift_north(promoting_pawns) & empty_squares;

        while (promo_pushes) {
            Square to_sq = bb_pop_square(promo_pushes);
            Square from_sq = static_cast<Square>(to_sq - 8);
            // Add all four promotion types
            add_move(moves, from_sq, to_sq, MoveFlags::PROMOTE_KNIGHT);
            add_move(moves, from_sq, to_sq, MoveFlags::PROMOTE_BISHOP);
            add_move(moves, from_sq, to_sq, MoveFlags::PROMOTE_ROOK);
            add_move(moves, from_sq, to_sq, MoveFlags::PROMOTE_QUEEN);
        }

        // Pawn captures (non-promoting)
        Bitboard captures_northeast = bb_shift_northeast(non_promoting_pawns) & their_pieces;
        Bitboard captures_northwest = bb_shift_northwest(non_promoting_pawns) & their_pieces;

        while (captures_northeast) {
            Square to_sq = bb_pop_square(captures_northeast);
            Square from_sq = static_cast<Square>(to_sq - 9);
            add_move(moves, from_sq, to_sq, MoveFlags::CAPTURE);
        }

        while (captures_northwest) {
            Square to_sq = bb_pop_square(captures_northwest);
            Square from_sq = static_cast<Square>(to_sq - 7);
            add_move(moves, from_sq, to_sq, MoveFlags::CAPTURE);
        }

        // Promotion captures
        Bitboard promo_captures_ne = bb_shift_northeast(promoting_pawns) & their_pieces;
        Bitboard promo_captures_nw = bb_shift_northwest(promoting_pawns) & their_pieces;

        while (promo_captures_ne) {
            Square to_sq = bb_pop_square(promo_captures_ne);
            Square from_sq = static_cast<Square>(to_sq - 9);
            add_move(moves, from_sq, to_sq, MoveFlags::PROMOTE_KNIGHT_CAPTURE);
            add_move(moves, from_sq, to_sq, MoveFlags::PROMOTE_BISHOP_CAPTURE);
            add_move(moves, from_sq, to_sq, MoveFlags::PROMOTE_ROOK_CAPTURE);
            add_move(moves, from_sq, to_sq, MoveFlags::PROMOTE_QUEEN_CAPTURE);
        }

        while (promo_captures_nw) {
            Square to_sq = bb_pop_square(promo_captures_nw);
            Square from_sq = static_cast<Square>(to_sq - 7);
            add_move(moves, from_sq, to_sq, MoveFlags::PROMOTE_KNIGHT_CAPTURE);
            add_move(moves, from_sq, to_sq, MoveFlags::PROMOTE_BISHOP_CAPTURE);
            add_move(moves, from_sq, to_sq, MoveFlags::PROMOTE_ROOK_CAPTURE);
            add_move(moves, from_sq, to_sq, MoveFlags::PROMOTE_QUEEN_CAPTURE);
        }

    } else {
        // Black pawn pushes
        // Single pushes from ranks 7-3 (not promoting)
        Bitboard non_promoting_pawns = our_pawns & ~BB_RANK_2;
        Bitboard single_pushes = bb_shift_south(non_promoting_pawns) & empty_squares;

        while (single_pushes) {
            Square to_sq = bb_pop_square(single_pushes);
            Square from_sq = static_cast<Square>(to_sq + 8);
            add_move(moves, from_sq, to_sq, MoveFlags::QUIET);
        }

        // Double pushes from rank 7
        Bitboard rank7_pawns = our_pawns & BB_RANK_7;
        Bitboard push1 = bb_shift_south(rank7_pawns) & empty_squares;
        Bitboard double_pushes = bb_shift_south(push1) & empty_squares;

        while (double_pushes) {
            Square to_sq = bb_pop_square(double_pushes);
            Square from_sq = static_cast<Square>(to_sq + 16);
            add_move(moves, from_sq, to_sq, MoveFlags::QUIET);
        }

        // Promotion pushes from rank 2
        Bitboard promoting_pawns = our_pawns & BB_RANK_2;
        Bitboard promo_pushes = bb_shift_south(promoting_pawns) & empty_squares;

        while (promo_pushes) {
            Square to_sq = bb_pop_square(promo_pushes);
            Square from_sq = static_cast<Square>(to_sq + 8);
            add_move(moves, from_sq, to_sq, MoveFlags::PROMOTE_KNIGHT);
            add_move(moves, from_sq, to_sq, MoveFlags::PROMOTE_BISHOP);
            add_move(moves, from_sq, to_sq, MoveFlags::PROMOTE_ROOK);
            add_move(moves, from_sq, to_sq, MoveFlags::PROMOTE_QUEEN);
        }

        // Pawn captures (non-promoting)
        Bitboard captures_southeast = bb_shift_southeast(non_promoting_pawns) & their_pieces;
        Bitboard captures_southwest = bb_shift_southwest(non_promoting_pawns) & their_pieces;

        while (captures_southeast) {
            Square to_sq = bb_pop_square(captures_southeast);
            Square from_sq = static_cast<Square>(to_sq + 7);
            add_move(moves, from_sq, to_sq, MoveFlags::CAPTURE);
        }

        while (captures_southwest) {
            Square to_sq = bb_pop_square(captures_southwest);
            Square from_sq = static_cast<Square>(to_sq + 9);
            add_move(moves, from_sq, to_sq, MoveFlags::CAPTURE);
        }

        // Promotion captures
        Bitboard promo_captures_se = bb_shift_southeast(promoting_pawns) & their_pieces;
        Bitboard promo_captures_sw = bb_shift_southwest(promoting_pawns) & their_pieces;

        while (promo_captures_se) {
            Square to_sq = bb_pop_square(promo_captures_se);
            Square from_sq = static_cast<Square>(to_sq + 7);
            add_move(moves, from_sq, to_sq, MoveFlags::PROMOTE_KNIGHT_CAPTURE);
            add_move(moves, from_sq, to_sq, MoveFlags::PROMOTE_BISHOP_CAPTURE);
            add_move(moves, from_sq, to_sq, MoveFlags::PROMOTE_ROOK_CAPTURE);
            add_move(moves, from_sq, to_sq, MoveFlags::PROMOTE_QUEEN_CAPTURE);
        }

        while (promo_captures_sw) {
            Square to_sq = bb_pop_square(promo_captures_sw);
            Square from_sq = static_cast<Square>(to_sq + 9);
            add_move(moves, from_sq, to_sq, MoveFlags::PROMOTE_KNIGHT_CAPTURE);
            add_move(moves, from_sq, to_sq, MoveFlags::PROMOTE_BISHOP_CAPTURE);
            add_move(moves, from_sq, to_sq, MoveFlags::PROMOTE_ROOK_CAPTURE);
            add_move(moves, from_sq, to_sq, MoveFlags::PROMOTE_QUEEN_CAPTURE);
        }
    }

    // En passant captures
    if (board.en_passant_square != SQUARE_NONE) {
        Square ep_sq = board.en_passant_square;
        File ep_file = file_of(ep_sq);

        // EP captures can come from adjacent files
        if (ep_file > FILE_A) {
            Square left_sq = make_square(static_cast<File>(ep_file - 1), rank_of(ep_sq));
            if (board.piece_type_at(left_sq) == PIECE_PAWN &&
                board.color_at(left_sq) == us) {
                add_move(moves, left_sq, ep_sq, MoveFlags::EP_CAPTURE);
            }
        }
        if (ep_file < FILE_H) {
            Square right_sq = make_square(static_cast<File>(ep_file + 1), rank_of(ep_sq));
            if (board.piece_type_at(right_sq) == PIECE_PAWN &&
                board.color_at(right_sq) == us) {
                add_move(moves, right_sq, ep_sq, MoveFlags::EP_CAPTURE);
            }
        }
    }
}

// Generate moves for leapers (knight, king)
void generate_leaper_moves(const BoardState& board, MoveList& moves, Color us, Color them,
                           PieceType pt, const Bitboard attack_table[]) {
    Bitboard our_pieces = board.pieces_of(us, pt);
    Bitboard their_pieces = board.occupancy_of(them);
    Bitboard empty_or_enemy = ~board.occupancy_of(us);  // All squares not occupied by our pieces

    while (our_pieces) {
        Square from_sq = bb_pop_square(our_pieces);
        Bitboard attacks = attack_table[from_sq] & empty_or_enemy;

        while (attacks) {
            Square to_sq = bb_pop_square(attacks);
            MoveFlags flags = bb_test(their_pieces, to_sq) ? MoveFlags::CAPTURE : MoveFlags::QUIET;
            add_move(moves, from_sq, to_sq, flags);
        }
    }
}

// Generate moves for sliding pieces (bishop, rook, queen)
void generate_sliding_moves(const BoardState& board, MoveList& moves, Color us, Color them,
                            PieceType pt) {
    Bitboard our_pieces = board.pieces_of(us, pt);
    Bitboard their_pieces = board.occupancy_of(them);
    Bitboard empty_or_enemy = ~board.occupancy_of(us);
    Bitboard occupancy = board.all_occupancy;

    while (our_pieces) {
        Square from_sq = bb_pop_square(our_pieces);

        Bitboard attacks;
        if (pt == PIECE_BISHOP) {
            attacks = bishop_attacks(from_sq, occupancy);
        } else if (pt == PIECE_ROOK) {
            attacks = rook_attacks(from_sq, occupancy);
        } else {  // PIECE_QUEEN
            attacks = queen_attacks(from_sq, occupancy);
        }

        attacks &= empty_or_enemy;

        while (attacks) {
            Square to_sq = bb_pop_square(attacks);
            MoveFlags flags = bb_test(their_pieces, to_sq) ? MoveFlags::CAPTURE : MoveFlags::QUIET;
            add_move(moves, from_sq, to_sq, flags);
        }
    }
}

}  // namespace

// Generate all pseudo-legal moves for the current side to move
size_t generate_pseudo_legal_moves(const BoardState& board, MoveList& moves) {
    moves.clear();

    Color us = board.side_to_move;
    Color them = static_cast<Color>(!us);

    // Generate pawn moves
    generate_pawn_moves(board, moves, us, them);

    // Generate knight moves
    generate_leaper_moves(board, moves, us, them, PIECE_KNIGHT, KNIGHT_ATTACKS);

    // Generate bishop moves
    generate_sliding_moves(board, moves, us, them, PIECE_BISHOP);

    // Generate rook moves
    generate_sliding_moves(board, moves, us, them, PIECE_ROOK);

    // Generate queen moves
    generate_sliding_moves(board, moves, us, them, PIECE_QUEEN);

    // Generate king moves
    generate_leaper_moves(board, moves, us, them, PIECE_KING, KING_ATTACKS);

    // Note: Castling is not included here - it will be added in S02-F06

    return moves.size();
}

// Generate only capture moves
size_t generate_captures(const BoardState& board, MoveList& moves) {
    MoveList all_moves;
    generate_pseudo_legal_moves(board, all_moves);

    moves.clear();
    for (size_t i = 0; i < all_moves.size(); ++i) {
        Move m = all_moves[i];
        if (is_capture(m) || is_en_passant(m) || is_promotion(m)) {
            moves.push(m);
        }
    }

    return moves.size();
}

// Generate only quiet (non-capture) moves
size_t generate_quiet_moves(const BoardState& board, MoveList& moves) {
    MoveList all_moves;
    generate_pseudo_legal_moves(board, all_moves);

    moves.clear();
    for (size_t i = 0; i < all_moves.size(); ++i) {
        Move m = all_moves[i];
        if (!is_capture(m) && !is_en_passant(m) && !is_promotion(m)) {
            moves.push(m);
        }
    }

    return moves.size();
}

}  // namespace thinmint::movegen
