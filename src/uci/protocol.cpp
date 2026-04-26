#include "thinmint/uci/protocol.h"

#include <sstream>

#include "thinmint/core/version.h"

namespace thinmint::uci {

Protocol::Protocol()
    : quit_received_(false),
      ready_(true),
      best_move_callback_(nullptr),
      info_callback_(nullptr) {
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

}  // namespace thinmint::uci
