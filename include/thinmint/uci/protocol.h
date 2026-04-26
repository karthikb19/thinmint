#pragma once

#include <functional>
#include <istream>
#include <ostream>
#include <string>

namespace thinmint::uci {

// UCI protocol handler
class Protocol {
 public:
  // Callbacks for engine integration
  using SendBestMoveCallback = std::function<void(const std::string& move, const std::string& ponder)>;
  using SendInfoCallback = std::function<void(const std::string& info)>;

  Protocol();

  // Process a single UCI command from input, writing responses to output
  // Returns false if the quit command was received
  bool ProcessCommand(const std::string& line, std::ostream& out);

  // Run the UCI loop reading from input and writing to output
  // Returns when quit command is received
  void RunLoop(std::istream& in, std::ostream& out);

  // Set callbacks for search results
  void SetBestMoveCallback(SendBestMoveCallback callback);
  void SetInfoCallback(SendInfoCallback callback);

  // Check if engine is ready for commands
  bool IsReady() const { return ready_; }

 private:
  // Command handlers
  void HandleUci(std::ostream& out);
  void HandleIsReady(std::ostream& out);
  void HandleUciNewGame();
  void HandleQuit();

  bool quit_received_;
  bool ready_;
  SendBestMoveCallback best_move_callback_;
  SendInfoCallback info_callback_;
};

}  // namespace thinmint::uci
