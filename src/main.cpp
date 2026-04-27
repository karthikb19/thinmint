#include <iostream>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <vector>

#include "thinmint/board/board.h"
#include "thinmint/core/version.h"
#include "thinmint/core/move.h"
#include "thinmint/movegen/attacks.h"
#include "thinmint/search/search.h"

namespace {

struct BenchPosition {
  const char* name;
  const char* fen;
};

int parse_depth(int argc, char* argv[]) {
  if (argc < 3) {
    return 3;
  }

  int depth = std::atoi(argv[2]);
  if (depth < 1) {
    return 1;
  }
  if (depth > 6) {
    return 6;
  }
  return depth;
}

int run_bench(int argc, char* argv[]) {
  using namespace thinmint::board;
  using namespace thinmint::core;
  using namespace thinmint::movegen;
  using namespace thinmint::search;

  init_all_attacks();

  const int depth = parse_depth(argc, argv);
  const std::vector<BenchPosition> positions = {
      {"startpos", START_POSITION_FEN},
      {"kiwipete", "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"},
      {"open", "r1bqkb1r/pppp1ppp/2n2n2/4p3/2B1P3/5N2/PPPP1PPP/RNBQK2R w KQkq - 4 4"},
      {"endgame", "8/8/4k3/3pP3/3K4/8/8/8 w - d6 0 1"},
      {"tactical", "4k3/8/8/3q4/4p3/2N5/8/4K3 w - - 0 1"},
  };

  uint64_t total_nodes = 0;
  auto start = std::chrono::steady_clock::now();

  std::cout << "thinmint bench depth " << depth << '\n';
  for (const BenchPosition& position : positions) {
    BoardState board = board_from_fen(position.fen);
    SearchResult result = iterative_deepening(board, depth);
    total_nodes += result.nodes;

    std::cout << position.name
              << " depth " << result.depth
              << " score " << result.score
              << " nodes " << result.nodes
              << " bestmove " << move_to_uci(result.best_move)
              << '\n';
  }

  auto end = std::chrono::steady_clock::now();
  auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
  uint64_t nps = elapsed_ms > 0 ? (total_nodes * 1000ULL) / static_cast<uint64_t>(elapsed_ms) : total_nodes;

  std::cout << "total nodes " << total_nodes
            << " time_ms " << elapsed_ms
            << " nps " << nps
            << '\n';

  return EXIT_SUCCESS;
}

}  // namespace

int main(int argc, char* argv[]) {
  if (argc >= 2 && std::string(argv[1]) == "bench") {
    return run_bench(argc, argv);
  }

  std::cout << "thinmint " << thinmint::core::version() << '\n';
  return EXIT_SUCCESS;
}
