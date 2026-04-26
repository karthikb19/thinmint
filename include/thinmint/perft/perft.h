#pragma once

#include <cstdint>
#include <string>

#include "thinmint/board/board.h"
#include "thinmint/movegen/movegen.h"

namespace thinmint::perft_ns {

// Perft (Performance Test) counts all leaf nodes at a given depth.
// This is the standard way to validate move generation correctness.
//
// Usage:
//   uint64_t nodes = perft(board, 5);  // Count all positions at depth 5
//
// For divide perft (count per first move), use perft_divide.

// Standard perft: count all leaf nodes at the given depth
// Returns the total number of nodes
uint64_t perft(thinmint::board::BoardState& board, int depth);

// Perft with node counting but no output
// Used internally for bulk counting
uint64_t perft_bulk(thinmint::board::BoardState& board, int depth);

// Perft "divide": show count per first move
// This is useful for debugging move generation issues
// Returns total node count and optionally prints per-move counts
uint64_t perft_divide(thinmint::board::BoardState& board, int depth, bool print = true);

// Perft result structure for detailed reporting
struct PerftResult {
    uint64_t nodes;
    uint64_t captures;
    uint64_t en_passant;
    uint64_t castles;
    uint64_t promotions;
    uint64_t checks;
    uint64_t checkmates;

    PerftResult() : nodes(0), captures(0), en_passant(0), castles(0),
                    promotions(0), checks(0), checkmates(0) {}
};

// Detailed perft with move type counting
PerftResult perft_detailed(thinmint::board::BoardState& board, int depth);

// Helper to run perft on a FEN string
uint64_t perft_fen(const std::string& fen, int depth);

// Known perft test positions with expected node counts
struct PerftFixture {
    const char* name;
    const char* fen;
    int max_depth;
    const uint64_t* expected_nodes;  // Array indexed by depth
};

// Starting position perft values (depth 0-6)
// Depth 0: 1
// Depth 1: 20
// Depth 2: 400
// Depth 3: 8902
// Depth 4: 197281
// Depth 5: 4865609
// Depth 6: 119060324
inline constexpr uint64_t STARTPOS_PERFT[] = {
    1ULL,
    20ULL,
    400ULL,
    8902ULL,
    197281ULL,
    4865609ULL,
    119060324ULL
};

// Kiwipete position (Position 2 from CPW)
// r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -
inline constexpr uint64_t KIWIPETE_PERFT[] = {
    1ULL,
    48ULL,
    2039ULL,
    97862ULL,
    4085603ULL,
    193690690ULL
};

// Position 3 (CPW) - tests promotions and pins
// 8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -
inline constexpr uint64_t POSITION3_PERFT[] = {
    1ULL,
    14ULL,
    191ULL,
    2812ULL,
    43238ULL,
    674624ULL
};

// Position 4 (CPW) - discovered check
// r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1
inline constexpr uint64_t POSITION4_PERFT[] = {
    1ULL,
    6ULL,
    264ULL,
    9467ULL,
    422333ULL,
    15833292ULL
};

// Position 5 (CPW) - complex position
// rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8
inline constexpr uint64_t POSITION5_PERFT[] = {
    1ULL,
    44ULL,
    1486ULL,
    62379ULL,
    2103487ULL,
    89941194ULL
};

// Position 6 (CPW) - edge cases
// r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10
inline constexpr uint64_t POSITION6_PERFT[] = {
    1ULL,
    46ULL,
    2079ULL,
    89890ULL,
    3894594ULL,
    164075551ULL
};

// Standard fixture definitions
inline constexpr PerftFixture PERFT_FIXTURES[] = {
    {
        "Start Position",
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        6,
        STARTPOS_PERFT
    },
    {
        "Kiwipete",
        "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",
        5,
        KIWIPETE_PERFT
    },
    {
        "Position 3 (promotions/pins)",
        "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1",
        5,
        POSITION3_PERFT
    },
    {
        "Position 4 (discovered check)",
        "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",
        5,
        POSITION4_PERFT
    },
    {
        "Position 5 (complex)",
        "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8",
        5,
        POSITION5_PERFT
    },
    {
        "Position 6 (edge cases)",
        "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10",
        5,
        POSITION6_PERFT
    }
};

inline constexpr size_t PERFT_FIXTURE_COUNT = sizeof(PERFT_FIXTURES) / sizeof(PERFT_FIXTURES[0]);

}  // namespace thinmint::perft_ns

namespace thinmint::perft {
// Re-export for backward compatibility
using perft_ns::perft;
using perft_ns::perft_bulk;
using perft_ns::perft_divide;
using perft_ns::perft_detailed;
using perft_ns::perft_fen;
using perft_ns::PerftResult;
using perft_ns::PerftFixture;
using perft_ns::STARTPOS_PERFT;
using perft_ns::KIWIPETE_PERFT;
using perft_ns::POSITION3_PERFT;
using perft_ns::POSITION4_PERFT;
using perft_ns::POSITION5_PERFT;
using perft_ns::POSITION6_PERFT;
using perft_ns::PERFT_FIXTURES;
using perft_ns::PERFT_FIXTURE_COUNT;
}  // namespace thinmint::perft
