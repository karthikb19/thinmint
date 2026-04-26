#include "thinmint/search/search.h"

#include "thinmint/board/makemove.h"
#include "thinmint/board/unmakemove.h"
#include "thinmint/eval/eval.h"
#include "thinmint/movegen/movegen.h"

namespace thinmint::search {

using namespace thinmint::eval;
using namespace thinmint::movegen;

bool is_terminal(const BoardState& board) {
    MoveList moves;
    generate_legal_moves(board, moves);
    return moves.empty();
}

int get_terminal_score(const BoardState& board, int ply) {
    // Check if it's checkmate or stalemate
    if (board.is_check(board.side_to_move)) {
        // Checkmate - return negative score (losing) adjusted by ply
        // Closer mate is better (less negative), so add ply
        return -MATE_SCORE + ply;
    } else {
        // Stalemate - draw
        return 0;
    }
}

bool is_mate_score(int score) {
    return std::abs(score) >= MATE_THRESHOLD;
}

int score_to_mate_in(int score) {
    if (!is_mate_score(score)) {
        return 0;
    }
    if (score > 0) {
        // We're mating - round up to full moves, convert to plies
        return (MATE_SCORE - score + 1) / 2;
    } else {
        // We're being mated
        return -(MATE_SCORE + score) / 2;
    }
}

int negamax(BoardState& board, int depth, int alpha, int beta, SearchStats& stats) {
    stats.nodes_searched++;

    // Terminal node check
    MoveList moves;
    generate_legal_moves(board, moves);

    if (moves.empty()) {
        stats.leaf_nodes++;
        return get_terminal_score(board, stats.current_max_depth - depth);
    }

    // Depth limit reached - evaluate
    if (depth <= 0) {
        stats.leaf_nodes++;
        // Evaluate from current side's perspective
        return evaluate(board);
    }

    int best_score = -INF_SCORE;

    // Iterate through all legal moves
    for (size_t i = 0; i < moves.size(); ++i) {
        Move move = moves[i];

        // Make the move
        UndoState undo = make_move_with_undo(board, move);
        stats.current_depth++;

        // Recursively search
        int score = -negamax(board, depth - 1, -beta, -alpha, stats);

        // Unmake the move
        unmake_move(board, undo);
        stats.current_depth--;

        // Update best score
        if (score > best_score) {
            best_score = score;
        }

        // Alpha-beta pruning
        if (score > alpha) {
            alpha = score;
        }

        if (alpha >= beta) {
            // Beta cutoff - opponent will avoid this line
            break;
        }
    }

    return best_score;
}

SearchResult search_root(BoardState& board, int depth) {
    SearchResult result;
    SearchStats stats;

    result.depth = depth;
    stats.current_max_depth = depth;

    // Generate all legal moves
    MoveList moves;
    generate_legal_moves(board, moves);

    // Check for terminal position
    if (moves.empty()) {
        result.score = get_terminal_score(board, 0);
        result.nodes = 1;
        result.best_move = MOVE_NONE;
        return result;
    }

    // If depth is 0, just evaluate
    if (depth <= 0) {
        result.score = evaluate(board);
        result.nodes = 1;
        result.best_move = MOVE_NONE;
        return result;
    }

    Move best_move = MOVE_NONE;
    int best_score = -INF_SCORE;
    int alpha = -INF_SCORE;
    int beta = INF_SCORE;

    // Search each root move
    for (size_t i = 0; i < moves.size(); ++i) {
        Move move = moves[i];

        // Make the move
        UndoState undo = make_move_with_undo(board, move);
        stats.current_depth = 1;

        // Search with negamax
        int score = -negamax(board, depth - 1, -beta, -alpha, stats);

        // Unmake the move
        unmake_move(board, undo);

        // Track best move
        if (score > best_score) {
            best_score = score;
            best_move = move;
        }

        // Update alpha
        if (score > alpha) {
            alpha = score;
        }
    }

    result.best_move = best_move;
    result.score = best_score;
    result.nodes = stats.nodes_searched;

    // Check for mate
    if (is_mate_score(best_score)) {
        result.mate_found = true;
        result.mate_in = score_to_mate_in(best_score);
    }

    return result;
}

int quiescence(BoardState& board, int alpha, int beta, SearchStats& stats) {
    // For now, just return static evaluation
    // Full quiescence search will be implemented in Sprint 04
    return evaluate(board);
}

SearchResult iterative_deepening(BoardState& board, int max_depth, SearchStats* stats) {
    SearchResult final_result;
    SearchStats local_stats;
    SearchStats* accumulated_stats = stats ? stats : &local_stats;

    // Remember starting node count to calculate nodes for this search only
    uint64_t starting_nodes = accumulated_stats->nodes_searched;

    // Generate legal moves once at the start
    MoveList moves;
    generate_legal_moves(board, moves);

    // Check for terminal position
    if (moves.empty()) {
        final_result.score = get_terminal_score(board, 0);
        final_result.nodes = 1;
        final_result.best_move = MOVE_NONE;
        final_result.depth = 0;
        return final_result;
    }

    // If max_depth is 0, just evaluate
    if (max_depth <= 0) {
        final_result.score = evaluate(board);
        final_result.nodes = 1;
        final_result.best_move = MOVE_NONE;
        final_result.depth = 0;
        return final_result;
    }

    // Iterative deepening: search at depth 1, 2, ..., max_depth
    for (int current_depth = 1; current_depth <= max_depth; ++current_depth) {
        SearchResult depth_result = search_root(board, current_depth);

        // Accumulate statistics
        accumulated_stats->nodes_searched += depth_result.nodes;
        accumulated_stats->current_depth = current_depth;
        accumulated_stats->current_max_depth = current_depth;

        // Save this iteration's result as our best so far
        // (deeper search should be better, but we keep the result)
        final_result = depth_result;

        // If we found a forced mate at this depth, we can stop early
        // The mate score is accurate, and deeper won't improve it
        if (depth_result.mate_found) {
            // But only stop if we're the side delivering mate
            // and the mate is found within the current depth
            if (depth_result.score > MATE_THRESHOLD) {
                // Found mate for us - this is the best we can do
                break;
            }
        }
    }

    // Update final result with nodes from this search only (not accumulated across calls)
    final_result.nodes = accumulated_stats->nodes_searched - starting_nodes;

    return final_result;
}

}  // namespace thinmint::search
