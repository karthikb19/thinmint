#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>

#include "thinmint/uci/protocol.h"
#include "thinmint/movegen/attacks.h"

using namespace thinmint;
using namespace thinmint::uci;
using namespace thinmint::core;
using namespace thinmint::movegen;

// Test counter
int tests_passed = 0;
int tests_failed = 0;

void check(bool condition, const char* test_name) {
    if (condition) {
        std::cout << "  [PASS] " << test_name << std::endl;
        tests_passed++;
    } else {
        std::cout << "  [FAIL] " << test_name << std::endl;
        tests_failed++;
    }
}

// Test go depth command
void test_go_depth() {
    std::cout << "\nTest: go depth" << std::endl;

    Protocol protocol;
    std::ostringstream output;

    // Set up position
    protocol.ProcessCommand("position startpos", output);

    // Clear output
    output.str("");

    // Search depth 1
    protocol.ProcessCommand("go depth 1", output);

    std::string response = output.str();

    // Should contain bestmove
    check(response.find("bestmove") != std::string::npos,
          "Response contains 'bestmove'");

    // Should contain info
    check(response.find("info") != std::string::npos,
          "Response contains 'info'");

    // Should contain depth
    check(response.find("depth") != std::string::npos,
          "Response contains 'depth'");
}

// Test go command without depth (defaults to depth 1)
void test_go_default() {
    std::cout << "\nTest: go command default depth" << std::endl;

    Protocol protocol;
    std::ostringstream output;

    protocol.ProcessCommand("position startpos", output);
    output.str("");

    protocol.ProcessCommand("go", output);

    std::string response = output.str();

    check(response.find("bestmove") != std::string::npos,
          "Default go returns bestmove");
}

// Test go depth with various depths
void test_go_various_depths() {
    std::cout << "\nTest: go with various depths" << std::endl;

    Protocol protocol;

    for (int depth = 1; depth <= 3; ++depth) {
        std::ostringstream output;
        protocol.ProcessCommand("position startpos", output);
        output.str("");

        std::string cmd = "go depth " + std::to_string(depth);
        protocol.ProcessCommand(cmd, output);

        std::string response = output.str();

        std::string test_name = "Depth " + std::to_string(depth) + " returns bestmove";
        check(response.find("bestmove") != std::string::npos, test_name.c_str());
    }
}

// Test go with mate position
void test_go_mate_position() {
    std::cout << "\nTest: go with mate position" << std::endl;

    Protocol protocol;
    std::ostringstream output;

    // White to move and mate in 1
    protocol.ProcessCommand("position fen 7k/8/6K1/8/8/8/8/Q7 w - - 0 1", output);
    output.str("");

    protocol.ProcessCommand("go depth 2", output);

    std::string response = output.str();

    check(response.find("bestmove") != std::string::npos,
          "Returns bestmove in mate position");

    // Should indicate mate in the info
    // (might have mate score or just cp score)
    check(response.find("score") != std::string::npos,
          "Info contains score");
}

// Test go infinite
void test_go_infinite() {
    std::cout << "\nTest: go infinite" << std::endl;

    Protocol protocol;
    std::ostringstream output;

    protocol.ProcessCommand("position startpos", output);
    output.str("");

    // Infinite should use a reasonable max depth
    protocol.ProcessCommand("go infinite", output);

    std::string response = output.str();

    check(response.find("bestmove") != std::string::npos,
          "go infinite returns bestmove");
}

// Test go with other parameters (should be parsed but ignored for now)
void test_go_with_time_params() {
    std::cout << "\nTest: go with time parameters" << std::endl;

    Protocol protocol;
    std::ostringstream output;

    protocol.ProcessCommand("position startpos", output);
    output.str("");

    // These parameters should be parsed but not cause errors
    protocol.ProcessCommand("go wtime 60000 btime 60000 winc 1000 binc 1000", output);

    std::string response = output.str();

    check(response.find("bestmove") != std::string::npos,
          "go with time params returns bestmove");
}

// Test bestmove is legal
void test_bestmove_is_legal() {
    std::cout << "\nTest: bestmove is legal" << std::endl;

    Protocol protocol;
    std::ostringstream output;

    protocol.ProcessCommand("position startpos", output);
    output.str("");

    protocol.ProcessCommand("go depth 2", output);

    std::string response = output.str();

    // Extract the bestmove from response
    size_t bestmove_pos = response.find("bestmove ");
    if (bestmove_pos != std::string::npos) {
        size_t move_start = bestmove_pos + 9;
        size_t move_end = response.find(' ', move_start);
        if (move_end == std::string::npos) {
            move_end = response.find('\n', move_start);
        }
        if (move_end == std::string::npos) {
            move_end = response.length();
        }

        std::string move_str = response.substr(move_start, move_end - move_start);

        // Should be a valid UCI move (e2e4 format)
        bool valid_format = (move_str.length() == 4 || move_str.length() == 5);
        if (valid_format) {
            bool valid_files = (move_str[0] >= 'a' && move_str[0] <= 'h') &&
                              (move_str[2] >= 'a' && move_str[2] <= 'h');
            bool valid_ranks = (move_str[1] >= '1' && move_str[1] <= '8') &&
                              (move_str[3] >= '1' && move_str[3] <= '8');
            check(valid_files && valid_ranks, "Bestmove has valid UCI format");
        } else {
            check(false, "Bestmove has valid UCI format");
        }
    } else {
        check(false, "Found bestmove in response");
    }
}

// Test go after moves
void test_go_after_moves() {
    std::cout << "\nTest: go after moves" << std::endl;

    Protocol protocol;
    std::ostringstream output;

    // Set position with moves
    protocol.ProcessCommand("position startpos moves e2e4 e7e5", output);
    output.str("");

    protocol.ProcessCommand("go depth 2", output);

    std::string response = output.str();

    check(response.find("bestmove") != std::string::npos,
          "Returns bestmove after moves");
}

// Test info output format
void test_info_format() {
    std::cout << "\nTest: info format" << std::endl;

    Protocol protocol;
    std::ostringstream output;

    protocol.ProcessCommand("position startpos", output);
    output.str("");

    protocol.ProcessCommand("go depth 2", output);

    std::string response = output.str();

    // Check info line format: info depth X score cp Y nodes Z
    bool has_depth = response.find("info depth") != std::string::npos;
    bool has_score = response.find("score") != std::string::npos;
    bool has_nodes = response.find("nodes") != std::string::npos;

    check(has_depth, "Info has depth");
    check(has_score, "Info has score");
    check(has_nodes, "Info has nodes");
}

int main() {
    std::cout << "=== UCI Go Command Tests ===" << std::endl;

    // Initialize attack tables
    init_all_attacks();

    test_go_depth();
    test_go_default();
    test_go_various_depths();
    test_go_mate_position();
    test_go_infinite();
    test_go_with_time_params();
    test_bestmove_is_legal();
    test_go_after_moves();
    test_info_format();

    std::cout << "\n=== Test Summary ===" << std::endl;
    std::cout << "Passed: " << tests_passed << std::endl;
    std::cout << "Failed: " << tests_failed << std::endl;

    return tests_failed > 0 ? 1 : 0;
}
