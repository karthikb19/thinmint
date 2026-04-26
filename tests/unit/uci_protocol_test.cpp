#include <iostream>
#include <sstream>
#include <string>

#include "thinmint/uci/protocol.h"

// Simple assertion helper
#define ASSERT_TRUE(cond) do { if (!(cond)) { std::cerr << "FAIL: " #cond " at line " << __LINE__ << "\n"; return 1; } } while(0)
#define ASSERT_FALSE(cond) do { if (cond) { std::cerr << "FAIL: NOT " #cond " at line " << __LINE__ << "\n"; return 1; } } while(0)
#define ASSERT_EQ(a, b) do { if ((a) != (b)) { std::cerr << "FAIL: " #a " == " #b " at line " << __LINE__ << " (got: " << (a) << ", expected: " << (b) << ")\n"; return 1; } } while(0)
#define ASSERT_CONTAINS(str, substr) do { if ((str).find(substr) == std::string::npos) { std::cerr << "FAIL: '" << str << "' contains '" << substr << "' at line " << __LINE__ << "\n"; return 1; } } while(0)

int main() {
  std::cerr << "Running UCI protocol tests...\n";

  // Test 1: UCI command returns id and uciok
  {
    std::cerr << "Test 1: uci command responds with id and uciok\n";
    thinmint::uci::Protocol protocol;
    std::istringstream in("");
    std::ostringstream out;

    bool continue_loop = protocol.ProcessCommand("uci", out);

    ASSERT_TRUE(continue_loop);
    std::string response = out.str();
    ASSERT_CONTAINS(response, "id name thinmint");
    ASSERT_CONTAINS(response, "id author");
    ASSERT_CONTAINS(response, "uciok\n");
  }

  // Test 2: isready command returns readyok
  {
    std::cerr << "Test 2: isready command responds with readyok\n";
    thinmint::uci::Protocol protocol;
    std::ostringstream out;

    bool continue_loop = protocol.ProcessCommand("isready", out);

    ASSERT_TRUE(continue_loop);
    ASSERT_EQ(out.str(), "readyok\n");
  }

  // Test 3: ucinewgame command is accepted
  {
    std::cerr << "Test 3: ucinewgame command is accepted\n";
    thinmint::uci::Protocol protocol;
    std::ostringstream out;

    bool continue_loop = protocol.ProcessCommand("ucinewgame", out);

    ASSERT_TRUE(continue_loop);
    ASSERT_TRUE(protocol.IsReady());
    // ucinewgame produces no output
    ASSERT_EQ(out.str(), "");
  }

  // Test 4: quit command terminates the loop
  {
    std::cerr << "Test 4: quit command terminates the loop\n";
    thinmint::uci::Protocol protocol;
    std::ostringstream out;

    bool continue_loop = protocol.ProcessCommand("quit", out);

    ASSERT_FALSE(continue_loop);
    ASSERT_EQ(out.str(), "");
  }

  // Test 5: empty command is handled gracefully
  {
    std::cerr << "Test 5: empty command is handled gracefully\n";
    thinmint::uci::Protocol protocol;
    std::ostringstream out;

    bool continue_loop = protocol.ProcessCommand("", out);

    ASSERT_TRUE(continue_loop);
    ASSERT_EQ(out.str(), "");
  }

  // Test 6: unknown commands are ignored
  {
    std::cerr << "Test 6: unknown commands are ignored\n";
    thinmint::uci::Protocol protocol;
    std::ostringstream out;

    bool continue_loop = protocol.ProcessCommand("unknowncommand xyz", out);

    ASSERT_TRUE(continue_loop);
    ASSERT_EQ(out.str(), "");
  }

  // Test 7: multiple commands in sequence
  {
    std::cerr << "Test 7: multiple commands in sequence\n";
    thinmint::uci::Protocol protocol;
    std::ostringstream out;

    protocol.ProcessCommand("uci", out);
    std::string uci_response = out.str();
    ASSERT_CONTAINS(uci_response, "uciok");

    out.str("");
    protocol.ProcessCommand("isready", out);
    ASSERT_EQ(out.str(), "readyok\n");

    out.str("");
    protocol.ProcessCommand("ucinewgame", out);
    ASSERT_TRUE(protocol.IsReady());
    ASSERT_EQ(out.str(), "");
  }

  // Test 8: debug command is handled (no-op for now)
  {
    std::cerr << "Test 8: debug command is handled\n";
    thinmint::uci::Protocol protocol;
    std::ostringstream out;

    bool continue_loop = protocol.ProcessCommand("debug on", out);

    ASSERT_TRUE(continue_loop);
    ASSERT_EQ(out.str(), "");
  }

  // Test 9: setoption command is handled (no-op for now)
  {
    std::cerr << "Test 9: setoption command is handled\n";
    thinmint::uci::Protocol protocol;
    std::ostringstream out;

    bool continue_loop = protocol.ProcessCommand("setoption name Hash value 32", out);

    ASSERT_TRUE(continue_loop);
    ASSERT_EQ(out.str(), "");
  }

  // Test 10: whitespace handling in commands
  {
    std::cerr << "Test 10: whitespace handling in commands\n";
    thinmint::uci::Protocol protocol;
    std::ostringstream out;

    bool continue_loop = protocol.ProcessCommand("  uci  ", out);

    ASSERT_TRUE(continue_loop);
    std::string response = out.str();
    ASSERT_CONTAINS(response, "uciok");
  }

  std::cerr << "All UCI protocol tests passed!\n";
  return 0;
}
