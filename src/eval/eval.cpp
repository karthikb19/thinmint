#include "thinmint/eval/eval.h"

#include <algorithm>
#include <cmath>

namespace thinmint::eval {

// Piece-square tables
// Values tuned for typical chess engine behavior
// Higher values = more desirable squares

// Pawn PST: encourage center control, 7th rank pushes, penalize back rank
const int PAWN_PST[64] = {
      0,   0,   0,   0,   0,   0,   0,   0,  // Rank 1 (should be empty for pawns)
     50,  50,  50,  50,  50,  50,  50,  50,  // Rank 2 (starting position - keep compact)
     10,  10,  20,  30,  30,  20,  10,  10,  // Rank 3
      5,   5,  10,  25,  25,  10,   5,   5,  // Rank 4
      0,   0,   0,  20,  20,   0,   0,   0,  // Rank 5
      5,  -5, -10,   0,   0, -10,  -5,   5,  // Rank 6
      5,  10,  10, -20, -20,  10,  10,   5,  // Rank 7
      0,   0,   0,   0,   0,   0,   0,   0   // Rank 8 (promotion rank)
};

// Knight PST: strongly favor center, penalize edges and corners
const int KNIGHT_PST[64] = {
    -50, -40, -30, -30, -30, -30, -40, -50,  // Rank 1
    -40, -20,   0,   0,   0,   0, -20, -40,  // Rank 2
    -30,   0,  10,  15,  15,  10,   0, -30,  // Rank 3
    -30,   5,  15,  20,  20,  15,   5, -30,  // Rank 4
    -30,   0,  15,  20,  20,  15,   0, -30,  // Rank 5
    -30,   5,  10,  15,  15,  10,   5, -30,  // Rank 6
    -40, -20,   0,   5,   5,   0, -20, -40,  // Rank 7
    -50, -40, -30, -30, -30, -30, -40, -50   // Rank 8
};

// Bishop PST: favor long diagonals and active squares
const int BISHOP_PST[64] = {
    -20, -10, -10, -10, -10, -10, -10, -20,  // Rank 1
    -10,   0,   0,   0,   0,   0,   0, -10,  // Rank 2
    -10,   0,   5,  10,  10,   5,   0, -10,  // Rank 3
    -10,   5,   5,  10,  10,   5,   5, -10,  // Rank 4
    -10,   0,  10,  10,  10,  10,   0, -10,  // Rank 5
    -10,  10,  10,  10,  10,  10,  10, -10,  // Rank 6
    -10,   5,   0,   0,   0,   0,   5, -10,  // Rank 7
    -20, -10, -10, -10, -10, -10, -10, -20   // Rank 8
};

// Rook PST: favor open files and 7th rank
const int ROOK_PST[64] = {
      0,   0,   0,   0,   0,   0,   0,   0,  // Rank 1
      5,  10,  10,  10,  10,  10,  10,   5,  // Rank 2
     -5,   0,   0,   0,   0,   0,   0,  -5,  // Rank 3
     -5,   0,   0,   0,   0,   0,   0,  -5,  // Rank 4
     -5,   0,   0,   0,   0,   0,   0,  -5,  // Rank 5
     -5,   0,   0,   0,   0,   0,   0,  -5,  // Rank 6
     -5,   0,   0,   0,   0,   0,   0,  -5,  // Rank 7
      0,   0,   0,   5,   5,   0,   0,   0   // Rank 8 (7th rank bonus)
};

// Queen PST: slightly favor center, avoid early development
const int QUEEN_PST[64] = {
    -20, -10, -10,  -5,  -5, -10, -10, -20,  // Rank 1
    -10,   0,   0,   0,   0,   0,   0, -10,  // Rank 2
    -10,   0,   5,   5,   5,   5,   0, -10,  // Rank 3
     -5,   0,   5,   5,   5,   5,   0,  -5,  // Rank 4
      0,   0,   5,   5,   5,   5,   0,  -5,  // Rank 5
    -10,   5,   5,   5,   5,   5,   0, -10,  // Rank 6
    -10,   0,   5,   0,   0,   0,   0, -10,  // Rank 7
    -20, -10, -10,  -5,  -5, -10, -10, -20   // Rank 8
};

// King PST (middlegame): encourage castling, stay safe
const int KING_PST[64] = {
    -30, -40, -40, -50, -50, -40, -40, -30,  // Rank 1 (castled position good)
    -30, -40, -40, -50, -50, -40, -40, -30,  // Rank 2
    -30, -40, -40, -50, -50, -40, -40, -30,  // Rank 3
    -30, -40, -40, -50, -50, -40, -40, -30,  // Rank 4
    -20, -30, -30, -40, -40, -30, -30, -20,  // Rank 5
    -10, -20, -20, -20, -20, -20, -20, -10,  // Rank 6
     20,  20,   0,   0,   0,   0,  20,  20,  // Rank 7
     20,  30,  10,   0,   0,  10,  30,  20   // Rank 8 (stay back, encourage castling)
};

// Get the appropriate PST for a piece type
const int* get_pst(PieceType pt) {
    switch (pt) {
        case PIECE_PAWN:   return PAWN_PST;
        case PIECE_KNIGHT: return KNIGHT_PST;
        case PIECE_BISHOP: return BISHOP_PST;
        case PIECE_ROOK:   return ROOK_PST;
        case PIECE_QUEEN:  return QUEEN_PST;
        case PIECE_KING:   return KING_PST;
        default:           return nullptr;
    }
}

// Evaluate a piece's position
// For White: use PST directly
// For Black: mirror the square (rank and file reversed)
int evaluate_piece_position(Square sq, PieceType pt, bool is_white) {
    const int* pst = get_pst(pt);
    if (!pst) return 0;

    // Convert square to PST index
    // Square 0 = a1, 7 = h1, 56 = a8, 63 = h8
    int file = sq % 8;
    int rank = sq / 8;

    int pst_index;
    if (is_white) {
        // White: use square as-is (a1 = 0, h8 = 63)
        pst_index = rank * 8 + file;
    } else {
        // Black: mirror vertically (flip rank)
        // Black's a8 (56) should use index 0 (a1)
        // Black's a1 (0) should use index 56 (a8)
        pst_index = (7 - rank) * 8 + file;
    }

    return pst[pst_index];
}

// Calculate material balance
int evaluate_material(const BoardState& board) {
    int white_material = 0;
    int black_material = 0;

    // Sum material for all piece types except king (king is priceless)
    for (PieceType pt = PIECE_PAWN; pt <= PIECE_QUEEN; pt = static_cast<PieceType>(static_cast<int>(pt) + 1)) {
        int value = piece_value(pt);
        white_material += board.piece_count(COLOR_WHITE, pt) * value;
        black_material += board.piece_count(COLOR_BLACK, pt) * value;
    }

    return white_material - black_material;
}

// Calculate positional score using piece-square tables
int evaluate_position(const BoardState& board) {
    int white_pos = 0;
    int black_pos = 0;

    // Evaluate all pieces for White
    for (PieceType pt = PIECE_PAWN; pt <= PIECE_KING; pt = static_cast<PieceType>(static_cast<int>(pt) + 1)) {
        Bitboard pieces = board.pieces_of(COLOR_WHITE, pt);
        while (pieces) {
            Square sq = bb_pop_square(pieces);
            white_pos += evaluate_piece_position(sq, pt, true);
        }
    }

    // Evaluate all pieces for Black
    for (PieceType pt = PIECE_PAWN; pt <= PIECE_KING; pt = static_cast<PieceType>(static_cast<int>(pt) + 1)) {
        Bitboard pieces = board.pieces_of(COLOR_BLACK, pt);
        while (pieces) {
            Square sq = bb_pop_square(pieces);
            black_pos += evaluate_piece_position(sq, pt, false);
        }
    }

    return white_pos - black_pos;
}

// Full static evaluation
int evaluate(const BoardState& board) {
    // Calculate material and positional scores from White's perspective
    int material = evaluate_material(board);
    int positional = evaluate_position(board);

    // Total score from White's perspective
    int score = material + positional;

    // Return score from side_to_move perspective
    // If Black to move, negate the score
    if (board.side_to_move == COLOR_BLACK) {
        return -score;
    }
    return score;
}

// Estimate game phase (0 = endgame, 1 = opening/middlegame)
// Simple heuristic based on material remaining
double game_phase(const BoardState& board) {
    // Count non-pawn material (queen, rook, bishop, knight)
    int white_material = 0;
    int black_material = 0;

    for (PieceType pt = PIECE_KNIGHT; pt <= PIECE_QUEEN; pt = static_cast<PieceType>(static_cast<int>(pt) + 1)) {
        int value = piece_value(pt);
        white_material += board.piece_count(COLOR_WHITE, pt) * value;
        black_material += board.piece_count(COLOR_BLACK, pt) * value;
    }

    // Maximum material (both sides have all pieces)
    // 2*(R+B+N) + Q = 2*(500+330+320) + 900 = 2*1150 + 900 = 3200 per side
    // Total max = 6400
    const int MAX_MATERIAL = 6400;

    int total_material = white_material + black_material;

    // Clamp between 0 and 1
    return std::max(0.0, std::min(1.0, static_cast<double>(total_material) / MAX_MATERIAL));
}

}  // namespace thinmint::eval
