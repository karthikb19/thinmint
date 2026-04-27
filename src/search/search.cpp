#include "thinmint/search/search.h"

#include <algorithm>
#include <array>
#include <cstddef>

#include "thinmint/board/makemove.h"
#include "thinmint/board/unmakemove.h"
#include "thinmint/eval/eval.h"
#include "thinmint/movegen/movegen.h"

namespace thinmint::search {

using namespace thinmint::eval;
using namespace thinmint::movegen;

namespace {

inline constexpr int MAX_QUIESCENCE_PLY = 8;
inline constexpr int KILLER_SLOTS = 2;
inline constexpr int HISTORY_COLOR_COUNT = 2;
inline constexpr int SQUARE_COUNT = 64;

struct ScoredMove {
    Move move;
    int score;
};

struct OrderedMoves {
    std::array<ScoredMove, MAX_MOVES> moves;
    size_t count = 0;
};

struct SearchHeuristics {
    std::array<std::array<Move, KILLER_SLOTS>, MAX_DEPTH> killers{};
    std::array<std::array<std::array<int, SQUARE_COUNT>, SQUARE_COUNT>, HISTORY_COLOR_COUNT> history{};
};

int search_ply(const SearchStats& stats) {
    return stats.current_depth;
}

int color_index(Color color) {
    return color == COLOR_WHITE ? 0 : 1;
}

int heuristic_ply(const SearchStats& stats) {
    return std::clamp(stats.current_depth, 0, MAX_DEPTH - 1);
}

bool is_tactical_move(Move move) {
    return is_capture(move) || is_en_passant(move) || is_promotion(move);
}

bool is_killer_move(const SearchHeuristics& heuristics, int ply, Move move) {
    const auto& killers = heuristics.killers[ply];
    return killers[0] == move || killers[1] == move;
}

void store_killer_move(SearchHeuristics& heuristics, int ply, Move move) {
    if (is_tactical_move(move) || heuristics.killers[ply][0] == move) {
        return;
    }

    heuristics.killers[ply][1] = heuristics.killers[ply][0];
    heuristics.killers[ply][0] = move;
}

void add_history_score(SearchHeuristics& heuristics, Color side, Move move, int depth) {
    if (is_tactical_move(move)) {
        return;
    }

    int bonus = depth * depth;
    int& entry = heuristics.history[color_index(side)][from_square(move)][to_square(move)];
    entry = std::min(entry + bonus, 1'000'000);
}

int move_order_score(const BoardState& board,
                     const SearchHeuristics* heuristics,
                     int ply,
                     Move move) {
    int score = 0;
    PieceType attacker = board.piece_type_at(from_square(move));

    if (is_capture(move) || is_en_passant(move)) {
        PieceType victim = is_en_passant(move) ? PIECE_PAWN : board.piece_type_at(to_square(move));
        score += 10000 + (10 * piece_value(victim)) - piece_value(attacker);
    }

    if (is_promotion(move)) {
        score += 8000 + piece_value(promotion_type(move));
    }

    if (is_castling(move)) {
        score += 50;
    }

    if (heuristics != nullptr && !is_tactical_move(move)) {
        if (is_killer_move(*heuristics, ply, move)) {
            score += 9000;
        }

        score += heuristics->history[color_index(board.side_to_move)]
                                   [from_square(move)]
                                   [to_square(move)];
    }

    return score;
}

OrderedMoves order_moves(const BoardState& board,
                         const MoveList& moves,
                         const SearchHeuristics* heuristics = nullptr,
                         int ply = 0) {
    OrderedMoves ordered;
    ordered.count = moves.size();

    for (size_t i = 0; i < moves.size(); ++i) {
        Move move = moves[i];
        ordered.moves[i] = {move, move_order_score(board, heuristics, ply, move)};
    }

    std::stable_sort(ordered.moves.begin(),
                     ordered.moves.begin() + static_cast<std::ptrdiff_t>(ordered.count),
                     [](const ScoredMove& lhs, const ScoredMove& rhs) {
                         return lhs.score > rhs.score;
                     });

    return ordered;
}

}  // namespace

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

int negamax_impl(BoardState& board,
                 int depth,
                 int alpha,
                 int beta,
                 SearchStats& stats,
                 SearchHeuristics& heuristics) {
    stats.nodes_searched++;
    const int original_alpha = alpha;

    // Terminal node check
    MoveList moves;
    generate_legal_moves(board, moves);

    if (moves.empty()) {
        stats.leaf_nodes++;
        return get_terminal_score(board, stats.current_max_depth - depth);
    }

    // Depth limit reached - quiescence search
    if (depth <= 0) {
        return quiescence(board, alpha, beta, stats);
    }

    int best_score = -INF_SCORE;
    Move best_move = MOVE_NONE;
    int ply = heuristic_ply(stats);

    bool searched_first_move = false;
    OrderedMoves ordered = order_moves(board, moves, &heuristics, ply);
    for (size_t i = 0; i < ordered.count; ++i) {
        Move move = ordered.moves[i].move;
        Color moving_side = board.side_to_move;

        // Make the move
        UndoState undo = make_move_with_undo(board, move);
        stats.current_depth++;

        int score;
        if (!searched_first_move) {
            score = -negamax_impl(board, depth - 1, -beta, -alpha, stats, heuristics);
            searched_first_move = true;
        } else {
            score = -negamax_impl(board, depth - 1, -alpha - 1, -alpha, stats, heuristics);
            if (score > alpha && score < beta) {
                score = -negamax_impl(board, depth - 1, -beta, -alpha, stats, heuristics);
            }
        }

        // Unmake the move
        unmake_move(board, undo);
        stats.current_depth--;

        // Update best score
        if (score > best_score) {
            best_score = score;
            best_move = move;
        }

        // Alpha-beta pruning
        if (score > alpha) {
            alpha = score;
        }

        if (alpha >= beta) {
            // Beta cutoff - opponent will avoid this line
            store_killer_move(heuristics, ply, move);
            add_history_score(heuristics, moving_side, move, depth);
            break;
        }
    }

    if (best_score > original_alpha) {
        add_history_score(heuristics, board.side_to_move, best_move, depth);
    }

    return best_score;
}

int negamax(BoardState& board, int depth, int alpha, int beta, SearchStats& stats) {
    SearchHeuristics heuristics;
    return negamax_impl(board, depth, alpha, beta, stats, heuristics);
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
    SearchHeuristics heuristics;

    bool searched_first_move = false;
    OrderedMoves ordered = order_moves(board, moves, &heuristics, 0);
    for (size_t i = 0; i < ordered.count; ++i) {
        Move move = ordered.moves[i].move;
        Color moving_side = board.side_to_move;

        // Make the move
        UndoState undo = make_move_with_undo(board, move);
        stats.current_depth = 1;

        int score;
        if (!searched_first_move) {
            score = -negamax_impl(board, depth - 1, -beta, -alpha, stats, heuristics);
            searched_first_move = true;
        } else {
            score = -negamax_impl(board, depth - 1, -alpha - 1, -alpha, stats, heuristics);
            if (score > alpha && score < beta) {
                score = -negamax_impl(board, depth - 1, -beta, -alpha, stats, heuristics);
            }
        }

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
            add_history_score(heuristics, moving_side, move, depth);
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
    stats.nodes_searched++;

    const bool in_check = board.is_check(board.side_to_move);
    const int q_ply = search_ply(stats) - stats.current_max_depth;

    if (q_ply >= MAX_QUIESCENCE_PLY) {
        if (in_check) {
            MoveList legal_moves;
            generate_legal_moves(board, legal_moves);
            if (legal_moves.empty()) {
                stats.leaf_nodes++;
                return get_terminal_score(board, stats.current_depth);
            }
        }

        stats.leaf_nodes++;
        return evaluate(board);
    }

    if (!in_check) {
        int stand_pat = evaluate(board);

        if (stand_pat >= beta) {
            return beta;
        }

        if (stand_pat > alpha) {
            alpha = stand_pat;
        }
    }

    MoveList moves;
    if (in_check) {
        generate_legal_moves(board, moves);
        if (moves.empty()) {
            stats.leaf_nodes++;
            return get_terminal_score(board, stats.current_depth);
        }
    } else {
        generate_legal_captures(board, moves);
    }

    if (moves.empty()) {
        stats.leaf_nodes++;
        return alpha;
    }

    OrderedMoves ordered = order_moves(board, moves);
    for (size_t i = 0; i < ordered.count; ++i) {
        Move move = ordered.moves[i].move;

        UndoState undo = make_move_with_undo(board, move);
        stats.current_depth++;

        int score = -quiescence(board, -beta, -alpha, stats);

        unmake_move(board, undo);
        stats.current_depth--;

        if (score >= beta) {
            return beta;
        }

        if (score > alpha) {
            alpha = score;
        }
    }

    return alpha;
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
