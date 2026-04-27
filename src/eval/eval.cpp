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

const int PAWN_ENDGAME_PST[64] = {
      0,   0,   0,   0,   0,   0,   0,   0,
     10,  10,  10,  10,  10,  10,  10,  10,
     10,  10,  20,  25,  25,  20,  10,  10,
     15,  15,  25,  35,  35,  25,  15,  15,
     25,  25,  35,  45,  45,  35,  25,  25,
     40,  40,  50,  60,  60,  50,  40,  40,
     70,  70,  80,  90,  90,  80,  70,  70,
      0,   0,   0,   0,   0,   0,   0,   0
};

const int KNIGHT_ENDGAME_PST[64] = {
    -40, -30, -20, -20, -20, -20, -30, -40,
    -30, -10,   0,   5,   5,   0, -10, -30,
    -20,   0,  10,  15,  15,  10,   0, -20,
    -20,   5,  15,  20,  20,  15,   5, -20,
    -20,   5,  15,  20,  20,  15,   5, -20,
    -20,   0,  10,  15,  15,  10,   0, -20,
    -30, -10,   0,   5,   5,   0, -10, -30,
    -40, -30, -20, -20, -20, -20, -30, -40
};

const int BISHOP_ENDGAME_PST[64] = {
    -10, -10, -10, -10, -10, -10, -10, -10,
    -10,   5,   0,   0,   0,   0,   5, -10,
    -10,   0,  10,  10,  10,  10,   0, -10,
    -10,   0,  10,  15,  15,  10,   0, -10,
    -10,   0,  10,  15,  15,  10,   0, -10,
    -10,   0,  10,  10,  10,  10,   0, -10,
    -10,   5,   0,   0,   0,   0,   5, -10,
    -10, -10, -10, -10, -10, -10, -10, -10
};

const int ROOK_ENDGAME_PST[64] = {
      0,   0,   5,  10,  10,   5,   0,   0,
      5,  10,  10,  15,  15,  10,  10,   5,
      0,   5,  10,  10,  10,  10,   5,   0,
      0,   5,  10,  10,  10,  10,   5,   0,
      0,   5,  10,  10,  10,  10,   5,   0,
      0,   5,  10,  10,  10,  10,   5,   0,
      5,  10,  10,  15,  15,  10,  10,   5,
      0,   0,   5,  10,  10,   5,   0,   0
};

const int QUEEN_ENDGAME_PST[64] = {
    -10, -10,  -5,  -5,  -5,  -5, -10, -10,
    -10,   0,   0,   0,   0,   0,   0, -10,
     -5,   0,   5,   5,   5,   5,   0,  -5,
     -5,   0,   5,  10,  10,   5,   0,  -5,
     -5,   0,   5,  10,  10,   5,   0,  -5,
     -5,   0,   5,   5,   5,   5,   0,  -5,
    -10,   0,   0,   0,   0,   0,   0, -10,
    -10, -10,  -5,  -5,  -5,  -5, -10, -10
};

const int KING_ENDGAME_PST[64] = {
    -50, -30, -20, -10, -10, -20, -30, -50,
    -30, -10,  10,  20,  20,  10, -10, -30,
    -20,  10,  30,  40,  40,  30,  10, -20,
    -10,  20,  40,  50,  50,  40,  20, -10,
    -10,  20,  40,  50,  50,  40,  20, -10,
    -20,  10,  30,  40,  40,  30,  10, -20,
    -30, -10,  10,  20,  20,  10, -10, -30,
    -50, -30, -20, -10, -10, -20, -30, -50
};

// Get the appropriate PST for a piece type
const int* get_pst(PieceType pt) {
    return get_pst(pt, EvalPhase::OPENING);
}

const int* get_pst(PieceType pt, EvalPhase phase) {
    switch (pt) {
        case PIECE_PAWN:   return phase == EvalPhase::OPENING ? PAWN_PST : PAWN_ENDGAME_PST;
        case PIECE_KNIGHT: return phase == EvalPhase::OPENING ? KNIGHT_PST : KNIGHT_ENDGAME_PST;
        case PIECE_BISHOP: return phase == EvalPhase::OPENING ? BISHOP_PST : BISHOP_ENDGAME_PST;
        case PIECE_ROOK:   return phase == EvalPhase::OPENING ? ROOK_PST : ROOK_ENDGAME_PST;
        case PIECE_QUEEN:  return phase == EvalPhase::OPENING ? QUEEN_PST : QUEEN_ENDGAME_PST;
        case PIECE_KING:   return phase == EvalPhase::OPENING ? KING_PST : KING_ENDGAME_PST;
        default:           return nullptr;
    }
}

// Evaluate a piece's position
// For White: use PST directly
// For Black: mirror the square (rank and file reversed)
int evaluate_piece_position(Square sq, PieceType pt, bool is_white) {
    return evaluate_piece_position(sq, pt, is_white, EvalPhase::OPENING);
}

int evaluate_piece_position(Square sq, PieceType pt, bool is_white, EvalPhase phase) {
    const int* pst = get_pst(pt, phase);
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
    return evaluate_material(board, EvalPhase::OPENING);
}

int evaluate_material(const BoardState& board, EvalPhase phase) {
    int white_material = 0;
    int black_material = 0;

    // Sum material for all piece types except king (king is priceless)
    for (PieceType pt = PIECE_PAWN; pt <= PIECE_QUEEN; pt = static_cast<PieceType>(static_cast<int>(pt) + 1)) {
        int value = piece_value(pt, phase);
        white_material += board.piece_count(COLOR_WHITE, pt) * value;
        black_material += board.piece_count(COLOR_BLACK, pt) * value;
    }

    return white_material - black_material;
}

// Calculate positional score using piece-square tables
int evaluate_position(const BoardState& board) {
    return evaluate_position(board, EvalPhase::OPENING);
}

int evaluate_position(const BoardState& board, EvalPhase phase) {
    int white_pos = 0;
    int black_pos = 0;

    // Evaluate all pieces for White
    for (PieceType pt = PIECE_PAWN; pt <= PIECE_KING; pt = static_cast<PieceType>(static_cast<int>(pt) + 1)) {
        Bitboard pieces = board.pieces_of(COLOR_WHITE, pt);
        while (pieces) {
            Square sq = bb_pop_square(pieces);
            white_pos += evaluate_piece_position(sq, pt, true, phase);
        }
    }

    // Evaluate all pieces for Black
    for (PieceType pt = PIECE_PAWN; pt <= PIECE_KING; pt = static_cast<PieceType>(static_cast<int>(pt) + 1)) {
        Bitboard pieces = board.pieces_of(COLOR_BLACK, pt);
        while (pieces) {
            Square sq = bb_pop_square(pieces);
            black_pos += evaluate_piece_position(sq, pt, false, phase);
        }
    }

    return white_pos - black_pos;
}

int phase_score(const BoardState& board) {
    int phase = 0;
    phase += board.piece_count(COLOR_WHITE, PIECE_KNIGHT);
    phase += board.piece_count(COLOR_BLACK, PIECE_KNIGHT);
    phase += board.piece_count(COLOR_WHITE, PIECE_BISHOP);
    phase += board.piece_count(COLOR_BLACK, PIECE_BISHOP);
    phase += 2 * board.piece_count(COLOR_WHITE, PIECE_ROOK);
    phase += 2 * board.piece_count(COLOR_BLACK, PIECE_ROOK);
    phase += 4 * board.piece_count(COLOR_WHITE, PIECE_QUEEN);
    phase += 4 * board.piece_count(COLOR_BLACK, PIECE_QUEEN);
    return std::clamp(phase, 0, MAX_PHASE);
}

EvalComponents evaluate_components(const BoardState& board) {
    int opening = evaluate_material(board, EvalPhase::OPENING) +
                  evaluate_position(board, EvalPhase::OPENING);
    int endgame = evaluate_material(board, EvalPhase::ENDGAME) +
                  evaluate_position(board, EvalPhase::ENDGAME);
    int phase = phase_score(board);
    int score = ((opening * phase) + (endgame * (MAX_PHASE - phase))) / MAX_PHASE;

    if (board.side_to_move == COLOR_BLACK) {
        opening = -opening;
        endgame = -endgame;
        score = -score;
    }

    return {opening, endgame, phase, score};
}

// Full static evaluation
int evaluate(const BoardState& board) {
    return evaluate_components(board).score;
}

// Estimate game phase (0 = endgame, 1 = opening/middlegame)
// Simple heuristic based on material remaining
double game_phase(const BoardState& board) {
    return static_cast<double>(phase_score(board)) / MAX_PHASE;
}

}  // namespace thinmint::eval
