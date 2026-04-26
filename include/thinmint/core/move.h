#pragma once

#include <cstdint>
#include <string>

#include "thinmint/core/piece.h"
#include "thinmint/core/square.h"

namespace thinmint::core {

// Compact move encoding using 16 bits
// Bits 0-5: from square (0-63)
// Bits 6-11: to square (0-63)
// Bits 12-15: flags (4 bits)
//
// Flags encoding:
//   Bit 12: promotion flag
//   Bit 13: capture flag
//   Bit 14: special move type 0 (EP or castling)
//   Bit 15: special move type 1
//
// Special move types (bits 14-15):
//   00 = normal move
//   01 = en passant capture
//   10 = castling
//   11 = promotion
//
// When promotion flag is set, the type is encoded in bits 14-15
//   00 = knight, 01 = bishop, 10 = rook, 11 = queen

using Move = uint16_t;

// Move flags
enum class MoveFlags : uint16_t {
    // Basic flags
    QUIET = 0,

    // Capture flag
    CAPTURE = 1 << 13,  // 0x2000

    // Special move types (bits 14-15 shifted)
    EP_CAPTURE = (1 << 14),           // 0x4000 - en passant
    CASTLING = (1 << 15),             // 0x8000 - castling

    // Promotion flag and piece type encoding
    PROMOTION = (1 << 12),            // 0x1000 - indicates promotion
    PROMOTE_KNIGHT = PROMOTION,       // 0x1000
    PROMOTE_BISHOP = PROMOTION | (1 << 14),  // 0x5000
    PROMOTE_ROOK = PROMOTION | (1 << 15),    // 0x9000
    PROMOTE_QUEEN = PROMOTION | (3 << 14),   // 0xD000

    // Capture variants with promotion
    PROMOTE_KNIGHT_CAPTURE = PROMOTE_KNIGHT | CAPTURE,  // 0x3000
    PROMOTE_BISHOP_CAPTURE = PROMOTE_BISHOP | CAPTURE,  // 0x7000
    PROMOTE_ROOK_CAPTURE = PROMOTE_ROOK | CAPTURE,      // 0xB000
    PROMOTE_QUEEN_CAPTURE = PROMOTE_QUEEN | CAPTURE     // 0xF000
};

// Constants
inline constexpr Move MOVE_NONE = 0;
inline constexpr int MOVE_FROM_SHIFT = 0;
inline constexpr int MOVE_TO_SHIFT = 6;
inline constexpr int MOVE_FLAGS_SHIFT = 12;

// Mask for squares (6 bits each)
inline constexpr uint16_t MOVE_SQUARE_MASK = 0x3F;  // 63

// Move creation and extraction
constexpr Move make_move(Square from, Square to) {
    return static_cast<Move>(((to & MOVE_SQUARE_MASK) << MOVE_TO_SHIFT) |
                            ((from & MOVE_SQUARE_MASK) << MOVE_FROM_SHIFT));
}

constexpr Move make_move(Square from, Square to, MoveFlags flags) {
    return static_cast<Move>(make_move(from, to) | static_cast<uint16_t>(flags));
}

constexpr Square from_square(Move m) {
    return static_cast<Square>((m >> MOVE_FROM_SHIFT) & MOVE_SQUARE_MASK);
}

constexpr Square to_square(Move m) {
    return static_cast<Square>((m >> MOVE_TO_SHIFT) & MOVE_SQUARE_MASK);
}

constexpr MoveFlags move_flags(Move m) {
    return static_cast<MoveFlags>(m & ~((MOVE_SQUARE_MASK << MOVE_TO_SHIFT) |
                                        (MOVE_SQUARE_MASK << MOVE_FROM_SHIFT)));
}

// Flag queries
constexpr bool is_capture(Move m) {
    return (m & static_cast<uint16_t>(MoveFlags::CAPTURE)) != 0;
}

constexpr bool is_promotion(Move m) {
    return (m & static_cast<uint16_t>(MoveFlags::PROMOTION)) != 0;
}

constexpr bool is_castling(Move m) {
    // Castling has special type 10 (bits 14-15)
    return (m & (3 << 14)) == static_cast<uint16_t>(MoveFlags::CASTLING);
}

constexpr bool is_en_passant(Move m) {
    // EP has special type 01 (bits 14-15) and is not a promotion
    return (m & (3 << 14)) == static_cast<uint16_t>(MoveFlags::EP_CAPTURE) &&
           !is_promotion(m);
}

// Get promotion piece type from move
// Returns PIECE_NONE if not a promotion
constexpr PieceType promotion_type(Move m) {
    if (!is_promotion(m)) {
        return PIECE_NONE;
    }
    // Promotion type is in bits 14-15
    switch ((m >> 14) & 3) {
        case 0: return PIECE_KNIGHT;
        case 1: return PIECE_BISHOP;
        case 2: return PIECE_ROOK;
        case 3: return PIECE_QUEEN;
        default: return PIECE_NONE;  // unreachable
    }
}

// Create promotion move with specified piece type
constexpr Move make_promotion(Square from, Square to, PieceType promote_to, bool capture = false) {
    MoveFlags flags;
    switch (promote_to) {
        case PIECE_KNIGHT:
            flags = capture ? MoveFlags::PROMOTE_KNIGHT_CAPTURE : MoveFlags::PROMOTE_KNIGHT;
            break;
        case PIECE_BISHOP:
            flags = capture ? MoveFlags::PROMOTE_BISHOP_CAPTURE : MoveFlags::PROMOTE_BISHOP;
            break;
        case PIECE_ROOK:
            flags = capture ? MoveFlags::PROMOTE_ROOK_CAPTURE : MoveFlags::PROMOTE_ROOK;
            break;
        case PIECE_QUEEN:
            flags = capture ? MoveFlags::PROMOTE_QUEEN_CAPTURE : MoveFlags::PROMOTE_QUEEN;
            break;
        default:
            return make_move(from, to);  // Invalid, return quiet move
    }
    return make_move(from, to, flags);
}

// Create en passant capture move
constexpr Move make_en_passant(Square from, Square to) {
    return make_move(from, to, MoveFlags::EP_CAPTURE);
}

// Create castling move
constexpr Move make_castling(Square from, Square to) {
    return make_move(from, to, MoveFlags::CASTLING);
}

// Create capture move (non-promotion)
constexpr Move make_capture(Square from, Square to) {
    return make_move(from, to, MoveFlags::CAPTURE);
}

// Convert move to UCI string (e.g., "e2e4", "e7e8q")
// Returns empty string for MOVE_NONE
std::string move_to_uci(Move m);

// Convert UCI string to move
// Returns MOVE_NONE on invalid input
// Note: This only validates format, not legality
Move uci_to_move(std::string_view uci);

// Convert move to SAN-like string for debugging (e.g., "Pe2-e4", "exd5")
// Returns "(none)" for MOVE_NONE
std::string move_to_string(Move m);

}  // namespace thinmint::core
