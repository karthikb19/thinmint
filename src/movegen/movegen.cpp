#include "thinmint/movegen/movegen.h"

#include <cstdlib>  // For abs

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
        // Capturing pawn is one rank BEHIND the EP square (from our perspective)
        Rank pawn_rank = (us == COLOR_WHITE) ?
                         static_cast<Rank>(rank_of(ep_sq) - 1) :  // White: EP on rank 6, pawn on rank 5
                         static_cast<Rank>(rank_of(ep_sq) + 1);   // Black: EP on rank 3, pawn on rank 4

        // EP captures can come from adjacent files on the pawn's rank
        if (ep_file > FILE_A) {
            Square left_sq = make_square(static_cast<File>(ep_file - 1), pawn_rank);
            if (board.piece_type_at(left_sq) == PIECE_PAWN &&
                board.color_at(left_sq) == us) {
                add_move(moves, left_sq, ep_sq, MoveFlags::EP_CAPTURE);
            }
        }
        if (ep_file < FILE_H) {
            Square right_sq = make_square(static_cast<File>(ep_file + 1), pawn_rank);
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

// Generate castling moves
// Castling is pseudo-legal if:
// 1. The side has the castling right
// 2. King and rook are in their starting positions
// 3. The path between king and rook is clear
// 4. King is not in check
// 5. King does not pass through or end up on an attacked square
void generate_castling_moves_internal(const BoardState& board, MoveList& moves, Color us) {
    using namespace thinmint::movegen;

    CastlingRights rights = board.castling_rights;
    if (rights == CASTLING_NONE) {
        return;
    }

    // Get king square
    Square king_sq = board.king_square(us);
    if (king_sq == SQUARE_NONE) {
        return;
    }

    // Check that king is in starting position
    Square king_start = king_start_square(us);
    if (king_sq != king_start) {
        return;
    }

    // Check if king is in check - cannot castle while in check
    if (board.is_check(us)) {
        return;
    }

    Bitboard occupancy = board.all_occupancy;
    Color them = !us;

    if (us == COLOR_WHITE) {
        // White kingside castling (O-O): E1 -> G1, rook H1 -> F1
        if (has_castling_right(rights, CASTLING_WHITE_KINGSIDE)) {
            Square rook_sq = SQUARE_H1;
            // Check rook is present
            if (board.piece_type_at(rook_sq) == PIECE_ROOK &&
                board.color_at(rook_sq) == COLOR_WHITE) {
                // Check path is clear: F1 and G1 must be empty
                if (!bb_test(occupancy, SQUARE_F1) && !bb_test(occupancy, SQUARE_G1)) {
                    // Check king doesn't pass through check (F1 must not be attacked)
                    // and doesn't end up in check (G1 must not be attacked)
                    if (!is_square_attacked(board, SQUARE_F1, them) &&
                        !is_square_attacked(board, SQUARE_G1, them)) {
                        add_move(moves, SQUARE_E1, SQUARE_G1, MoveFlags::CASTLING);
                    }
                }
            }
        }

        // White queenside castling (O-O-O): E1 -> C1, rook A1 -> D1
        if (has_castling_right(rights, CASTLING_WHITE_QUEENSIDE)) {
            Square rook_sq = SQUARE_A1;
            // Check rook is present
            if (board.piece_type_at(rook_sq) == PIECE_ROOK &&
                board.color_at(rook_sq) == COLOR_WHITE) {
                // Check path is clear: B1, C1, and D1 must be empty
                if (!bb_test(occupancy, SQUARE_B1) &&
                    !bb_test(occupancy, SQUARE_C1) &&
                    !bb_test(occupancy, SQUARE_D1)) {
                    // Check king doesn't pass through check (D1 must not be attacked)
                    // and doesn't end up in check (C1 must not be attacked)
                    // Note: B1 can be attacked (king doesn't pass through it)
                    if (!is_square_attacked(board, SQUARE_D1, them) &&
                        !is_square_attacked(board, SQUARE_C1, them)) {
                        add_move(moves, SQUARE_E1, SQUARE_C1, MoveFlags::CASTLING);
                    }
                }
            }
        }
    } else {
        // Black kingside castling (O-O): E8 -> G8, rook H8 -> F8
        if (has_castling_right(rights, CASTLING_BLACK_KINGSIDE)) {
            Square rook_sq = SQUARE_H8;
            // Check rook is present
            if (board.piece_type_at(rook_sq) == PIECE_ROOK &&
                board.color_at(rook_sq) == COLOR_BLACK) {
                // Check path is clear: F8 and G8 must be empty
                if (!bb_test(occupancy, SQUARE_F8) && !bb_test(occupancy, SQUARE_G8)) {
                    // Check king doesn't pass through check (F8 must not be attacked)
                    // and doesn't end up in check (G8 must not be attacked)
                    if (!is_square_attacked(board, SQUARE_F8, them) &&
                        !is_square_attacked(board, SQUARE_G8, them)) {
                        add_move(moves, SQUARE_E8, SQUARE_G8, MoveFlags::CASTLING);
                    }
                }
            }
        }

        // Black queenside castling (O-O-O): E8 -> C8, rook A8 -> D8
        if (has_castling_right(rights, CASTLING_BLACK_QUEENSIDE)) {
            Square rook_sq = SQUARE_A8;
            // Check rook is present
            if (board.piece_type_at(rook_sq) == PIECE_ROOK &&
                board.color_at(rook_sq) == COLOR_BLACK) {
                // Check path is clear: B8, C8, and D8 must be empty
                if (!bb_test(occupancy, SQUARE_B8) &&
                    !bb_test(occupancy, SQUARE_C8) &&
                    !bb_test(occupancy, SQUARE_D8)) {
                    // Check king doesn't pass through check (D8 must not be attacked)
                    // and doesn't end up in check (C8 must not be attacked)
                    // Note: B8 can be attacked (king doesn't pass through it)
                    if (!is_square_attacked(board, SQUARE_D8, them) &&
                        !is_square_attacked(board, SQUARE_C8, them)) {
                        add_move(moves, SQUARE_E8, SQUARE_C8, MoveFlags::CASTLING);
                    }
                }
            }
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

    // Generate castling moves
    generate_castling_moves_internal(board, moves, us);

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

// Generate castling moves only
size_t generate_castling_moves(const BoardState& board, MoveList& moves) {
    moves.clear();
    generate_castling_moves_internal(board, moves, board.side_to_move);
    return moves.size();
}

// Get all pieces giving check to the specified king
Bitboard get_checkers(const BoardState& board, Color king_color) {
    Color them = !king_color;
    Square king_sq = board.king_square(king_color);

    if (king_sq == SQUARE_NONE) {
        return BB_EMPTY;
    }

    Bitboard checkers = BB_EMPTY;
    Bitboard occupancy = board.all_occupancy;

    // Pawn checks
    Bitboard their_pawns = board.pieces_of(them, PIECE_PAWN);
    checkers |= their_pawns & pawn_attacks(king_sq, king_color);

    // Knight checks
    Bitboard their_knights = board.pieces_of(them, PIECE_KNIGHT);
    checkers |= their_knights & knight_attacks(king_sq);

    // Bishop/Queen diagonal checks
    Bitboard their_bishops = board.pieces_of(them, PIECE_BISHOP);
    Bitboard their_queens = board.pieces_of(them, PIECE_QUEEN);
    Bitboard diagonal_attackers = their_bishops | their_queens;
    Bitboard bishop_attacks_from_king = bishop_attacks(king_sq, occupancy);
    checkers |= diagonal_attackers & bishop_attacks_from_king;

    // Rook/Queen orthogonal checks
    Bitboard their_rooks = board.pieces_of(them, PIECE_ROOK);
    Bitboard orthogonal_attackers = their_rooks | their_queens;
    Bitboard rook_attacks_from_king = rook_attacks(king_sq, occupancy);
    checkers |= orthogonal_attackers & rook_attacks_from_king;

    // King checks (for completeness - though kings can't give "check" in a legal position)
    // We don't check for king attacks here as two kings can never be adjacent in legal chess

    return checkers;
}

// Get the pin ray for a piece at the given square
// Returns the ray from the king through the piece, or empty if not pinned
// A piece is pinned if:
// 1. It's on the same ray (diagonal or orthogonal) as the enemy sliding piece
// 2. The king is behind it along that ray
// 3. There are no other pieces between the enemy slider and this piece
Bitboard get_pin_ray(const BoardState& board, Color us, Square piece_sq) {
    Square king_sq = board.king_square(us);
    if (king_sq == SQUARE_NONE) {
        return BB_EMPTY;
    }

    // Must be on same diagonal or file/rank as king
    if (king_sq == piece_sq) {
        return BB_EMPTY;
    }

    int king_file = file_of(king_sq);
    int king_rank = rank_of(king_sq);
    int piece_file = file_of(piece_sq);
    int piece_rank = rank_of(piece_sq);

    int file_diff = piece_file - king_file;
    int rank_diff = piece_rank - king_rank;

    // Check if on same rank, file, or diagonal
    bool same_rank = (rank_diff == 0);
    bool same_file = (file_diff == 0);
    bool same_diagonal = (abs(file_diff) == abs(rank_diff));

    if (!same_rank && !same_file && !same_diagonal) {
        return BB_EMPTY;  // Not on a ray from king
    }

    Color them = !us;
    Bitboard occupancy = board.all_occupancy;

    // Direction from king to piece
    int file_step = (file_diff == 0) ? 0 : (file_diff > 0 ? 1 : -1);
    int rank_step = (rank_diff == 0) ? 0 : (rank_diff > 0 ? 1 : -1);

    // Check for enemy sliding piece beyond this piece
    int curr_file = piece_file + file_step;
    int curr_rank = piece_rank + rank_step;

    // First, check there are no other pieces between king and this piece
    int check_file = king_file + file_step;
    int check_rank = king_rank + rank_step;
    while (check_file != piece_file || check_rank != piece_rank) {
        Square check_sq = make_square(static_cast<File>(check_file), static_cast<Rank>(check_rank));
        if (bb_test(occupancy, check_sq)) {
            return BB_EMPTY;  // Another piece blocks the ray
        }
        check_file += file_step;
        check_rank += rank_step;
    }

    // Now look beyond this piece for an enemy sliding attacker
    while (curr_file >= 0 && curr_file <= 7 && curr_rank >= 0 && curr_rank <= 7) {
        Square curr_sq = make_square(static_cast<File>(curr_file), static_cast<Rank>(curr_rank));

        if (bb_test(occupancy, curr_sq)) {
            // Found a piece - check if it's an enemy slider
            PieceType pt = board.piece_type_at(curr_sq);
            Color pc = board.color_at(curr_sq);

            if (pc == them) {
                // Check if it's a sliding piece that attacks along this ray
                bool is_bishop_like = (abs(file_step) == 1 && abs(rank_step) == 1);
                bool is_rook_like = (abs(file_step) == 0 || abs(rank_step) == 0);

                if ((is_bishop_like && (pt == PIECE_BISHOP || pt == PIECE_QUEEN)) ||
                    (is_rook_like && (pt == PIECE_ROOK || pt == PIECE_QUEEN))) {
                    // This piece is pinned - return the pin ray (including both ends)
                    Bitboard ray = BB_EMPTY;
                    int f = king_file;
                    int r = king_rank;
                    while (true) {
                        Square sq = make_square(static_cast<File>(f), static_cast<Rank>(r));
                        ray = bb_set(ray, sq);
                        if (f == curr_file && r == curr_rank) break;
                        f += file_step;
                        r += rank_step;
                    }
                    return ray;
                }
            }
            // Piece found but not an enemy slider - no pin
            return BB_EMPTY;
        }

        curr_file += file_step;
        curr_rank += rank_step;
    }

    return BB_EMPTY;  // No enemy slider found
}

// Generate legal moves
size_t generate_legal_moves(const BoardState& board, MoveList& moves) {
    Color us = board.side_to_move;
    Color them = !us;
    Square king_sq = board.king_square(us);

    if (king_sq == SQUARE_NONE) {
        moves.clear();
        return 0;
    }

    // Generate pseudo-legal moves first
    MoveList pseudo_legal;
    generate_pseudo_legal_moves(board, pseudo_legal);

    // Find checkers
    Bitboard checkers = get_checkers(board, us);
    int num_checkers = bb_popcount(checkers);

    moves.clear();

    if (num_checkers == 0) {
        // Not in check - filter for pins only
        for (size_t i = 0; i < pseudo_legal.size(); ++i) {
            Move m = pseudo_legal[i];
            Square from_sq = from_square(m);
            Square to_sq = to_square(m);

            // King moves - verify destination is not attacked and not adjacent to enemy king
            if (from_sq == king_sq) {
                // First check: king cannot move adjacent to opponent's king
                Square enemy_king_sq = board.king_square(them);
                if (enemy_king_sq != SQUARE_NONE) {
                    int enemy_king_file = file_of(enemy_king_sq);
                    int enemy_king_rank = rank_of(enemy_king_sq);
                    int to_file = file_of(to_sq);
                    int to_rank = rank_of(to_sq);

                    // Check if destination is adjacent to enemy king
                    int file_diff = abs(to_file - enemy_king_file);
                    int rank_diff = abs(to_rank - enemy_king_rank);
                    if (file_diff <= 1 && rank_diff <= 1) {
                        continue;  // Cannot move adjacent to enemy king
                    }
                }

                // Create temp board with king moved
                BoardState temp_board = board;
                Bitboard from_bb = bb_from_square(from_sq);
                Bitboard to_bb = bb_from_square(to_sq);

                // Update king position
                temp_board.pieces[static_cast<size_t>(us)][static_cast<size_t>(PIECE_KING)] &= ~from_bb;
                temp_board.pieces[static_cast<size_t>(us)][static_cast<size_t>(PIECE_KING)] |= to_bb;

                // Update occupancy
                if (us == COLOR_WHITE) {
                    temp_board.white_occupancy &= ~from_bb;
                    temp_board.white_occupancy |= to_bb;
                } else {
                    temp_board.black_occupancy &= ~from_bb;
                    temp_board.black_occupancy |= to_bb;
                }
                temp_board.all_occupancy = temp_board.white_occupancy | temp_board.black_occupancy;

                // Check if new square is attacked
                if (!is_square_attacked(temp_board, to_sq, them)) {
                    moves.push(m);
                }
            } else {
                // Non-king move - check if piece is pinned
                Bitboard pin_ray = get_pin_ray(board, us, from_sq);
                if (pin_ray == BB_EMPTY) {
                    // Not pinned - move is legal (as long as we're not in check, which we're not)
                    moves.push(m);
                } else {
                    // Pinned - can only move along the pin ray
                    if (bb_test(pin_ray, to_sq)) {
                        moves.push(m);
                    }
                }
            }
        }
    } else if (num_checkers == 1) {
        // Single check - can capture checker, block (if sliding), or move king
        Square checker_sq = bb_pop_square(checkers);

        // Get the check ray (for blocking)
        Bitboard check_ray = BB_EMPTY;
        PieceType checker_pt = board.piece_type_at(checker_sq);

        // If checker is a sliding piece, compute the check ray
        if (checker_pt == PIECE_BISHOP || checker_pt == PIECE_ROOK || checker_pt == PIECE_QUEEN) {
            int king_file = file_of(king_sq);
            int king_rank = rank_of(king_sq);
            int checker_file = file_of(checker_sq);
            int checker_rank = rank_of(checker_sq);

            int file_step = (checker_file == king_file) ? 0 : (checker_file > king_file ? 1 : -1);
            int rank_step = (checker_rank == king_rank) ? 0 : (checker_rank > king_rank ? 1 : -1);

            // Build ray from king (exclusive) to checker (inclusive)
            int f = king_file + file_step;
            int r = king_rank + rank_step;
            while (f != checker_file || r != checker_rank) {
                check_ray = bb_set(check_ray, make_square(static_cast<File>(f), static_cast<Rank>(r)));
                f += file_step;
                r += rank_step;
            }
            // Include checker square for capture
            check_ray = bb_set(check_ray, checker_sq);
        } else {
            // Non-sliding checker - can only capture, not block
            check_ray = bb_from_square(checker_sq);
        }

        for (size_t i = 0; i < pseudo_legal.size(); ++i) {
            Move m = pseudo_legal[i];
            Square from_sq = from_square(m);
            Square to_sq = to_square(m);

            // King moves - verify destination is not attacked and not adjacent to enemy king
            if (from_sq == king_sq) {
                // First check: king cannot move adjacent to opponent's king
                Square enemy_king_sq = board.king_square(them);
                if (enemy_king_sq != SQUARE_NONE) {
                    int enemy_king_file = file_of(enemy_king_sq);
                    int enemy_king_rank = rank_of(enemy_king_sq);
                    int to_file = file_of(to_sq);
                    int to_rank = rank_of(to_sq);

                    // Check if destination is adjacent to enemy king
                    int file_diff = abs(to_file - enemy_king_file);
                    int rank_diff = abs(to_rank - enemy_king_rank);
                    if (file_diff <= 1 && rank_diff <= 1) {
                        continue;  // Cannot move adjacent to enemy king
                    }
                }

                // Create temp board with king moved
                BoardState temp_board = board;
                Bitboard from_bb = bb_from_square(from_sq);
                Bitboard to_bb = bb_from_square(to_sq);

                // Update king position
                temp_board.pieces[static_cast<size_t>(us)][static_cast<size_t>(PIECE_KING)] &= ~from_bb;
                temp_board.pieces[static_cast<size_t>(us)][static_cast<size_t>(PIECE_KING)] |= to_bb;

                // Update occupancy
                if (us == COLOR_WHITE) {
                    temp_board.white_occupancy &= ~from_bb;
                    temp_board.white_occupancy |= to_bb;
                } else {
                    temp_board.black_occupancy &= ~from_bb;
                    temp_board.black_occupancy |= to_bb;
                }
                temp_board.all_occupancy = temp_board.white_occupancy | temp_board.black_occupancy;

                // Check if new square is attacked
                if (!is_square_attacked(temp_board, to_sq, them)) {
                    moves.push(m);
                }
            } else {
                // Non-king move - check for pins and check evasion
                Bitboard pin_ray = get_pin_ray(board, us, from_sq);
                if (pin_ray != BB_EMPTY) {
                    // Pinned pieces cannot evade check (moving would expose king)
                    continue;
                }

                // Check if move captures the checker or blocks the check
                if (bb_test(check_ray, to_sq)) {
                    // Special case: en passant capture
                    // Check if the EP capture would remove the check or reveal a new one
                    if (is_en_passant(m)) {
                        // Create temp board to test EP legality
                        BoardState temp_board = board;
                        Bitboard from_bb = bb_from_square(from_sq);
                        Bitboard to_bb = bb_from_square(to_sq);

                        // Get the captured pawn square
                        Rank cap_rank = (us == COLOR_WHITE) ? RANK_5 : RANK_4;
                        Square captured_sq = make_square(file_of(to_sq), cap_rank);
                        Bitboard captured_bb = bb_from_square(captured_sq);

                        // Update pawn position
                        temp_board.pieces[static_cast<size_t>(us)][static_cast<size_t>(PIECE_PAWN)] &= ~from_bb;
                        temp_board.pieces[static_cast<size_t>(us)][static_cast<size_t>(PIECE_PAWN)] |= to_bb;

                        // Remove captured pawn
                        temp_board.pieces[static_cast<size_t>(them)][static_cast<size_t>(PIECE_PAWN)] &= ~captured_bb;

                        // Update occupancy
                        if (us == COLOR_WHITE) {
                            temp_board.white_occupancy &= ~from_bb;
                            temp_board.white_occupancy |= to_bb;
                            temp_board.black_occupancy &= ~captured_bb;
                        } else {
                            temp_board.black_occupancy &= ~from_bb;
                            temp_board.black_occupancy |= to_bb;
                            temp_board.white_occupancy &= ~captured_bb;
                        }
                        temp_board.all_occupancy = temp_board.white_occupancy | temp_board.black_occupancy;

                        // Check if king is still in check after EP
                        Square new_king_sq = temp_board.king_square(us);
                        if (!is_square_attacked(temp_board, new_king_sq, them)) {
                            moves.push(m);
                        }
                    } else {
                        moves.push(m);
                    }
                }
            }
        }
    } else {
        // Double check (or more) - only king moves allowed
        for (size_t i = 0; i < pseudo_legal.size(); ++i) {
            Move m = pseudo_legal[i];
            Square from_sq = from_square(m);

            if (from_sq != king_sq) {
                continue;  // Only king moves allowed in double check
            }

            Square to_sq = to_square(m);

            // First check: king cannot move adjacent to opponent's king
            Square enemy_king_sq = board.king_square(them);
            if (enemy_king_sq != SQUARE_NONE) {
                int enemy_king_file = file_of(enemy_king_sq);
                int enemy_king_rank = rank_of(enemy_king_sq);
                int to_file = file_of(to_sq);
                int to_rank = rank_of(to_sq);

                // Check if destination is adjacent to enemy king
                int file_diff = abs(to_file - enemy_king_file);
                int rank_diff = abs(to_rank - enemy_king_rank);
                if (file_diff <= 1 && rank_diff <= 1) {
                    continue;  // Cannot move adjacent to enemy king
                }
            }

            // Create temp board with king moved
            BoardState temp_board = board;
            Bitboard from_bb = bb_from_square(from_sq);
            Bitboard to_bb = bb_from_square(to_sq);

            // Update king position
            temp_board.pieces[static_cast<size_t>(us)][static_cast<size_t>(PIECE_KING)] &= ~from_bb;
            temp_board.pieces[static_cast<size_t>(us)][static_cast<size_t>(PIECE_KING)] |= to_bb;

            // Update occupancy
            if (us == COLOR_WHITE) {
                temp_board.white_occupancy &= ~from_bb;
                temp_board.white_occupancy |= to_bb;
            } else {
                temp_board.black_occupancy &= ~from_bb;
                temp_board.black_occupancy |= to_bb;
            }
            temp_board.all_occupancy = temp_board.white_occupancy | temp_board.black_occupancy;

            // Check if new square is attacked
            if (!is_square_attacked(temp_board, to_sq, them)) {
                moves.push(m);
            }
        }
    }

    return moves.size();
}

// Check if a specific move is legal
bool is_move_legal(const BoardState& board, Move move) {
    if (move == MOVE_NONE) {
        return false;
    }

    MoveList legal_moves;
    generate_legal_moves(board, legal_moves);

    for (size_t i = 0; i < legal_moves.size(); ++i) {
        if (legal_moves[i] == move) {
            return true;
        }
    }

    return false;
}

}  // namespace thinmint::movegen
