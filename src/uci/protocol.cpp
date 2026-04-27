#include "thinmint/uci/protocol.h"

#include <sstream>

#include "thinmint/core/version.h"
#include "thinmint/core/move.h"
#include "thinmint/board/makemove.h"
#include "thinmint/movegen/movegen.h"
#include "thinmint/search/search.h"

namespace thinmint::uci {

Protocol::Protocol()
    : quit_received_(false),
      ready_(true),
      best_move_callback_(nullptr),
      info_callback_(nullptr) {
  // Initialize board to start position
  board_.reset_to_start_position();
}

bool Protocol::ProcessCommand(const std::string& line, std::ostream& out) {
  std::istringstream iss(line);
  std::string command;
  iss >> command;

  if (command.empty()) {
    return !quit_received_;
  }

  if (command == "uci") {
    HandleUci(out);
  } else if (command == "isready") {
    HandleIsReady(out);
  } else if (command == "ucinewgame") {
    HandleUciNewGame();
  } else if (command == "quit") {
    HandleQuit();
  } else if (command == "position") {
    HandlePosition(iss);
  } else if (command == "go") {
    HandleGo(iss, out);
  } else if (command == "debug") {
    // Debug mode - acknowledged but no-op for now
  } else if (command == "setoption") {
    // Set option - acknowledged but no-op for now
  } else {
    // Unknown command - silently ignored per UCI spec
  }

  return !quit_received_;
}

void Protocol::RunLoop(std::istream& in, std::ostream& out) {
  std::string line;
  while (std::getline(in, line)) {
    if (!ProcessCommand(line, out)) {
      break;
    }
  }
}

void Protocol::SetBestMoveCallback(SendBestMoveCallback callback) {
  best_move_callback_ = callback;
}

void Protocol::SetInfoCallback(SendInfoCallback callback) {
  info_callback_ = callback;
}

void Protocol::HandleUci(std::ostream& out) {
  // Send engine identification
  out << "id name thinmint " << thinmint::core::version() << "\n";
  out << "id author thinmint team\n";

  // Send available options (none for now)
  // out << "option name ...\n";

  // Signal UCI mode is active
  out << "uciok\n";
  out.flush();
}

void Protocol::HandleIsReady(std::ostream& out) {
  // Engine is ready to receive commands
  out << "readyok\n";
  out.flush();
}

void Protocol::HandleUciNewGame() {
  // Reset any game-specific state
  // For now, just mark that we're ready for a new game
  ready_ = true;
}

void Protocol::HandleQuit() {
  quit_received_ = true;
}

void Protocol::HandlePosition(std::istringstream& iss) {
  std::string token;
  iss >> token;

  if (token == "startpos") {
    // Reset to start position
    board_.reset_to_start_position();
  } else if (token == "fen") {
    // Read the FEN string (6 fields)
    std::string fen_parts[6];
    bool fen_complete = true;

    for (int i = 0; i < 6; ++i) {
      if (!(iss >> fen_parts[i])) {
        fen_complete = false;
        break;
      }
    }

    if (!fen_complete) {
      // Invalid FEN - keep current position
      return;
    }

    // Reconstruct FEN string
    std::string fen = fen_parts[0] + " " + fen_parts[1] + " " +
                      fen_parts[2] + " " + fen_parts[3] + " " +
                      fen_parts[4] + " " + fen_parts[5];

    // Parse the FEN
    if (!board_.parse_fen(fen)) {
      // Invalid FEN - keep current position
      return;
    }
  } else {
    // Invalid position command - ignore
    return;
  }

  // Check for "moves" keyword followed by move list
  iss >> token;
  if (token == "moves") {
    // Apply each move in sequence
    std::string uci_move;
    while (iss >> uci_move) {
      if (!ApplyUciMove(uci_move)) {
        // Failed to apply move - stop processing
        break;
      }
    }
  }
}

bool Protocol::ApplyUciMove(const std::string& uci_move) {
  // Find the legal move matching the UCI string
  thinmint::core::Move move = FindLegalMove(uci_move);

  if (move == thinmint::core::MOVE_NONE) {
    return false;
  }

  // Apply the move to the board
  make_move(board_, move);
  return true;
}

thinmint::core::Move Protocol::FindLegalMove(const std::string& uci_move) const {
  // Parse UCI string
  if (uci_move.length() < 4 || uci_move.length() > 5) {
    return thinmint::core::MOVE_NONE;
  }

  // Parse from and to squares
  std::string from_str = uci_move.substr(0, 2);
  std::string to_str = uci_move.substr(2, 2);

  thinmint::core::Square from = thinmint::core::algebraic_to_square(from_str);
  thinmint::core::Square to = thinmint::core::algebraic_to_square(to_str);

  if (from == thinmint::core::SQUARE_NONE || to == thinmint::core::SQUARE_NONE) {
    return thinmint::core::MOVE_NONE;
  }

  // Parse promotion piece if present
  thinmint::core::PieceType promotion = thinmint::core::PIECE_NONE;
  if (uci_move.length() == 5) {
    char promo_char = uci_move[4];
    switch (promo_char) {
      case 'q': case 'Q': promotion = thinmint::core::PIECE_QUEEN; break;
      case 'r': case 'R': promotion = thinmint::core::PIECE_ROOK; break;
      case 'b': case 'B': promotion = thinmint::core::PIECE_BISHOP; break;
      case 'n': case 'N': promotion = thinmint::core::PIECE_KNIGHT; break;
      default: return thinmint::core::MOVE_NONE;
    }
  }

  // Generate legal moves and find the matching one
  thinmint::movegen::MoveList legal_moves;
  thinmint::movegen::generate_legal_moves(board_, legal_moves);

  for (size_t i = 0; i < legal_moves.size(); ++i) {
    thinmint::core::Move move = legal_moves[i];

    if (thinmint::core::from_square(move) != from) continue;
    if (thinmint::core::to_square(move) != to) continue;

    if (promotion != thinmint::core::PIECE_NONE) {
      // Must be a promotion with matching piece type
      if (!thinmint::core::is_promotion(move)) continue;
      if (thinmint::core::promotion_type(move) != promotion) continue;
    } else {
      // Must not be a promotion
      if (thinmint::core::is_promotion(move)) continue;
    }

    return move;
  }

  return thinmint::core::MOVE_NONE;
}

void Protocol::HandleGo(std::istringstream& iss, std::ostream& out) {
  std::string token;
  int depth = -1;  // -1 means not specified

  // Parse go command parameters
  while (iss >> token) {
    if (token == "depth") {
      if (!(iss >> depth)) {
        depth = -1;  // Invalid depth parameter
      }
    }
    // Other parameters (wtime, btime, movetime, etc.) are parsed but ignored
    // for now since we only support depth-limited search in this sprint
    else if (token == "wtime" || token == "btime" || token == "winc" ||
             token == "binc" || token == "movestogo" || token == "movetime") {
      // Skip the value for these parameters
      int value;
      iss >> value;
    } else if (token == "infinite") {
      // Infinite search - we'll use a reasonable max depth
      // Depth 3 keeps the current synchronous implementation responsive.
      depth = 3;
    }
  }

  // Default to depth 1 if no depth specified
  if (depth < 0) {
    depth = 1;
  }

  // Clamp to reasonable bounds
  if (depth > thinmint::search::MAX_DEPTH) {
    depth = thinmint::search::MAX_DEPTH;
  }

  // Perform iterative deepening search
  thinmint::search::SearchResult result =
      thinmint::search::iterative_deepening(board_, depth);

  // Send info about the search
  // Format: info depth X score cp Y nodes Z pv <bestmove>
  out << "info depth " << result.depth;
  if (result.mate_found) {
    out << " score mate " << result.mate_in;
  } else {
    out << " score cp " << result.score;
  }
  out << " nodes " << result.nodes;
  out << "\n";

  // Send the best move
  std::string best_move_str = "0000";
  if (result.best_move != thinmint::core::MOVE_NONE) {
    best_move_str = thinmint::core::move_to_uci(result.best_move);
  }

  out << "bestmove " << best_move_str << "\n";
  out.flush();

  // Also call callbacks if set
  if (best_move_callback_) {
    best_move_callback_(best_move_str, "");
  }
}

}  // namespace thinmint::uci
