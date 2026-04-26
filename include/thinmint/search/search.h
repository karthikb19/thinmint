#pragma once

#include <cstdint>

#include "thinmint/board/board.h"
#include "thinmint/core/move.h"

namespace thinmint::search {

using namespace thinmint::core;
using namespace thinmint::board;

// Score constants
inline constexpr int INF_SCORE = 30000;
inline constexpr int MATE_SCORE = 29000;
inline constexpr int MATE_THRESHOLD = 28000;

// Maximum search depth
inline constexpr int MAX_DEPTH = 64;

// Search statistics for debugging and monitoring
struct SearchStats {
    uint64_t nodes_searched;
    uint64_t leaf_nodes;
    int current_depth;
    int current_max_depth;

    SearchStats()
        : nodes_searched(0),
          leaf_nodes(0),
          current_depth(0),
          current_max_depth(0) {}

    void reset() {
        nodes_searched = 0;
        leaf_nodes = 0;
        current_depth = 0;
        current_max_depth = 0;
    }
};

// Search result returned after a completed search
struct SearchResult {
    Move best_move;           // Best move found
    int score;                // Score from side-to-move perspective
    int depth;                // Depth searched
    uint64_t nodes;           // Nodes searched
    bool mate_found;          // True if a mate was found
    int mate_in;              // Plows until mate (if mate_found)

    SearchResult()
        : best_move(0),
          score(0),
          depth(0),
          nodes(0),
          mate_found(false),
          mate_in(0) {}
};

// Negamax search with alpha-beta pruning
// Returns the best score for the current side to move
//
// Parameters:
// - board: current position
// - depth: remaining depth to search
// - alpha: lower bound
// - beta: upper bound
// - stats: search statistics accumulator
//
// Returns: score from current side's perspective
int negamax(BoardState& board, int depth, int alpha, int beta, SearchStats& stats);

// Root-level search function
// Performs a complete search to the specified depth
// Handles move selection and result formatting
//
// Parameters:
// - board: current position
// - depth: depth to search
//
// Returns: SearchResult containing best move and score
SearchResult search_root(BoardState& board, int depth);

// Iterative deepening search
// Searches at increasing depths from 1 to max_depth
// Returns the best result from the deepest completed iteration
//
// Parameters:
// - board: current position
// - max_depth: maximum depth to search
// - stats: optional pointer to receive search statistics
//
// Returns: SearchResult containing best move and score from deepest iteration
SearchResult iterative_deepening(BoardState& board, int max_depth, SearchStats* stats = nullptr);

// Quiescence search stub (for later sprint)
// Currently just returns static evaluation
int quiescence(BoardState& board, int alpha, int beta, SearchStats& stats);

// Check if the current position is a terminal node (checkmate or stalemate)
// Returns: true if the game has ended
bool is_terminal(const BoardState& board);

// Get game result score for terminal positions
// Returns: score from side-to-move perspective
//   - MATE_SCORE - ply if checkmate (closer mate is better)
//   - 0 if stalemate (draw)
int get_terminal_score(const BoardState& board, int ply);

// Check if the score represents a mate
bool is_mate_score(int score);

// Convert mate score to "mate in N" plies
// Positive = side to move mates in N
// Negative = side to move is mated in N
int score_to_mate_in(int score);

}  // namespace thinmint::search
