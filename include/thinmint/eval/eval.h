#pragma once

#include <cstdint>

#include "thinmint/board/board.h"

namespace thinmint::eval {

using namespace thinmint::core;
using namespace thinmint::board;

// Evaluation is returned from the perspective of the side to move
// Positive = advantage for side to move
// Negative = disadvantage for side to move
// Mate scores are not handled at evaluation level (search handles that)

// Value of a pawn (base unit for evaluation)
inline constexpr int PAWN_VALUE = 100;

// Material values for piece types (in centipawns)
inline constexpr int KNIGHT_VALUE = 320;
inline constexpr int BISHOP_VALUE = 330;
inline constexpr int ROOK_VALUE = 500;
inline constexpr int QUEEN_VALUE = 900;
inline constexpr int KING_VALUE = 20000;  // Arbitrary high value, king is priceless
inline constexpr int MAX_PHASE = 24;

enum class EvalPhase {
    OPENING,
    ENDGAME
};

struct EvalComponents {
    int opening;
    int endgame;
    int phase;
    int score;
};

// Get material value for a piece type
inline constexpr int piece_value(PieceType pt) {
    switch (pt) {
        case PIECE_PAWN:   return PAWN_VALUE;
        case PIECE_KNIGHT: return KNIGHT_VALUE;
        case PIECE_BISHOP: return BISHOP_VALUE;
        case PIECE_ROOK:   return ROOK_VALUE;
        case PIECE_QUEEN:  return QUEEN_VALUE;
        case PIECE_KING:   return KING_VALUE;
        default:           return 0;
    }
}

inline constexpr int piece_value(PieceType pt, EvalPhase phase) {
    switch (pt) {
        case PIECE_PAWN:   return PAWN_VALUE;
        case PIECE_KNIGHT: return phase == EvalPhase::OPENING ? KNIGHT_VALUE : 300;
        case PIECE_BISHOP: return phase == EvalPhase::OPENING ? BISHOP_VALUE : 340;
        case PIECE_ROOK:   return phase == EvalPhase::OPENING ? ROOK_VALUE : 520;
        case PIECE_QUEEN:  return QUEEN_VALUE;
        case PIECE_KING:   return KING_VALUE;
        default:           return 0;
    }
}

// Piece-square tables for positional evaluation
// Values are from White's perspective (a1 = index 0)
// Tables are symmetric and will be mirrored for Black

// Pawn piece-square table
// Encourages: center control, advancement, protection of king
extern const int PAWN_PST[64];

// Knight piece-square table
// Encourages: central outposts, avoids corners
extern const int KNIGHT_PST[64];

// Bishop piece-square table
// Encourages: long diagonals, active squares
extern const int BISHOP_PST[64];

// Rook piece-square table
// Encourages: open files, 7th rank
extern const int ROOK_PST[64];

// Queen piece-square table
// Encourages: central activity, avoids early development
extern const int QUEEN_PST[64];

// King piece-square table (middlegame)
// Encourages: castling, king safety
extern const int KING_PST[64];

extern const int PAWN_ENDGAME_PST[64];
extern const int KNIGHT_ENDGAME_PST[64];
extern const int BISHOP_ENDGAME_PST[64];
extern const int ROOK_ENDGAME_PST[64];
extern const int QUEEN_ENDGAME_PST[64];
extern const int KING_ENDGAME_PST[64];

// Get piece-square table for a piece type
const int* get_pst(PieceType pt);
const int* get_pst(PieceType pt, EvalPhase phase);

// Evaluate a single piece's position using PST
// sq: square where piece is located (from White's perspective)
// pt: piece type
// is_white: true if White piece, false if Black (Black's PST is mirrored)
int evaluate_piece_position(Square sq, PieceType pt, bool is_white);
int evaluate_piece_position(Square sq, PieceType pt, bool is_white, EvalPhase phase);

// Calculate material balance for a position
// Returns (White material - Black material) in centipawns
int evaluate_material(const BoardState& board);
int evaluate_material(const BoardState& board, EvalPhase phase);

// Calculate positional score using piece-square tables
// Returns (White positional - Black positional) in centipawns
int evaluate_position(const BoardState& board);
int evaluate_position(const BoardState& board, EvalPhase phase);

// Calculate opening/endgame components and tapered final score.
// All component scores are returned from side-to-move perspective.
EvalComponents evaluate_components(const BoardState& board);

// Additional positional terms, returned from White's perspective.
int evaluate_pawn_structure(const BoardState& board);
int evaluate_mobility(const BoardState& board);
int evaluate_center_control(const BoardState& board);
int evaluate_king_safety(const BoardState& board);
int evaluate_space(const BoardState& board);
int evaluate_tempo(const BoardState& board);
int evaluate_positional_terms(const BoardState& board);

// Full static evaluation of a position
// Returns score from the perspective of side_to_move
// Positive = good for side to move, Negative = bad for side to move
int evaluate(const BoardState& board);

// Convenience: get game phase estimate (0 = endgame, 1 = middlegame/opening)
// Used for future tapered eval, currently simple threshold
double game_phase(const BoardState& board);

}  // namespace thinmint::eval
