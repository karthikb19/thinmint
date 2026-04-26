#pragma once

#include <cstdint>
#include <string_view>

#include "thinmint/core/square.h"

namespace thinmint::core {

// Color: 0 = White, 1 = Black
enum class Color : uint8_t {
    WHITE = 0,
    BLACK = 1
};

inline constexpr Color COLOR_WHITE = Color::WHITE;
inline constexpr Color COLOR_BLACK = Color::BLACK;
inline constexpr int COLOR_COUNT = 2;

// Toggle color (WHITE -> BLACK, BLACK -> WHITE)
constexpr Color operator!(Color c) {
    return static_cast<Color>(1 - static_cast<uint8_t>(c));
}

// Piece types (no color)
// 0 = None, 1-6 = actual pieces
// This ordering matches typical chess piece values: P < N < B < R < Q < K
enum class PieceType : uint8_t {
    NONE = 0,
    PAWN = 1,
    KNIGHT = 2,
    BISHOP = 3,
    ROOK = 4,
    QUEEN = 5,
    KING = 6
};

inline constexpr PieceType PIECE_NONE = PieceType::NONE;
inline constexpr PieceType PIECE_PAWN = PieceType::PAWN;
inline constexpr PieceType PIECE_KNIGHT = PieceType::KNIGHT;
inline constexpr PieceType PIECE_BISHOP = PieceType::BISHOP;
inline constexpr PieceType PIECE_ROOK = PieceType::ROOK;
inline constexpr PieceType PIECE_QUEEN = PieceType::QUEEN;
inline constexpr PieceType PIECE_KING = PieceType::KING;
inline constexpr int PIECE_TYPE_COUNT = 7;  // Including NONE
inline constexpr int NUM_PIECE_TYPES = 6;   // Excluding NONE

// Combined piece: encodes both color and type
// Stored as: (color << 3) | type  -> 0-15 range
// White pieces: 0-7, Black pieces: 8-15
enum class Piece : uint8_t {
    NO_PIECE = 0,
    W_PAWN = 1,
    W_KNIGHT = 2,
    W_BISHOP = 3,
    W_ROOK = 4,
    W_QUEEN = 5,
    W_KING = 6,
    B_PAWN = 9,      // 8 + 1
    B_KNIGHT = 10,   // 8 + 2
    B_BISHOP = 11,   // 8 + 3
    B_ROOK = 12,     // 8 + 4
    B_QUEEN = 13,    // 8 + 5
    B_KING = 14      // 8 + 6
};

inline constexpr Piece PIECE_NO_PIECE = Piece::NO_PIECE;
inline constexpr Piece PIECE_W_PAWN = Piece::W_PAWN;
inline constexpr Piece PIECE_W_KNIGHT = Piece::W_KNIGHT;
inline constexpr Piece PIECE_W_BISHOP = Piece::W_BISHOP;
inline constexpr Piece PIECE_W_ROOK = Piece::W_ROOK;
inline constexpr Piece PIECE_W_QUEEN = Piece::W_QUEEN;
inline constexpr Piece PIECE_W_KING = Piece::W_KING;
inline constexpr Piece PIECE_B_PAWN = Piece::B_PAWN;
inline constexpr Piece PIECE_B_KNIGHT = Piece::B_KNIGHT;
inline constexpr Piece PIECE_B_BISHOP = Piece::B_BISHOP;
inline constexpr Piece PIECE_B_ROOK = Piece::B_ROOK;
inline constexpr Piece PIECE_B_QUEEN = Piece::B_QUEEN;
inline constexpr Piece PIECE_B_KING = Piece::B_KING;

// Extract color from piece
constexpr Color color_of(Piece p) {
    return static_cast<Color>(static_cast<uint8_t>(p) >> 3);
}

// Extract piece type from piece
constexpr PieceType type_of(Piece p) {
    return static_cast<PieceType>(static_cast<uint8_t>(p) & 7);
}

// Create piece from color and type
constexpr Piece make_piece(Color c, PieceType pt) {
    return static_cast<Piece>((static_cast<uint8_t>(c) << 3) | static_cast<uint8_t>(pt));
}

// Check if piece is valid (not NO_PIECE and type is valid)
constexpr bool is_valid_piece(Piece p) {
    PieceType pt = type_of(p);
    return pt != PIECE_NONE && pt <= PIECE_KING;
}

// Check if piece is white
constexpr bool is_white(Piece p) {
    return color_of(p) == COLOR_WHITE && is_valid_piece(p);
}

// Check if piece is black
constexpr bool is_black(Piece p) {
    return color_of(p) == COLOR_BLACK && is_valid_piece(p);
}

// Castling rights - use bit flags so multiple rights can be combined
// Bit 0: White kingside (O-O)
// Bit 1: White queenside (O-O-O)
// Bit 2: Black kingside
// Bit 3: Black queenside
enum class CastlingRights : uint8_t {
    NONE = 0,
    WHITE_KINGSIDE = 1,   // 0b0001
    WHITE_QUEENSIDE = 2,  // 0b0010
    BLACK_KINGSIDE = 4,   // 0b0100
    BLACK_QUEENSIDE = 8,  // 0b1000
    WHITE_BOTH = 3,       // 0b0011
    BLACK_BOTH = 12,      // 0b1100
    ALL = 15              // 0b1111
};

inline constexpr CastlingRights CASTLING_NONE = CastlingRights::NONE;
inline constexpr CastlingRights CASTLING_WHITE_KINGSIDE = CastlingRights::WHITE_KINGSIDE;
inline constexpr CastlingRights CASTLING_WHITE_QUEENSIDE = CastlingRights::WHITE_QUEENSIDE;
inline constexpr CastlingRights CASTLING_BLACK_KINGSIDE = CastlingRights::BLACK_KINGSIDE;
inline constexpr CastlingRights CASTLING_BLACK_QUEENSIDE = CastlingRights::BLACK_QUEENSIDE;
inline constexpr CastlingRights CASTLING_WHITE_BOTH = CastlingRights::WHITE_BOTH;
inline constexpr CastlingRights CASTLING_BLACK_BOTH = CastlingRights::BLACK_BOTH;
inline constexpr CastlingRights CASTLING_ALL = CastlingRights::ALL;

// Bitwise operators for CastlingRights
constexpr CastlingRights operator|(CastlingRights lhs, CastlingRights rhs) {
    return static_cast<CastlingRights>(
        static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
}

constexpr CastlingRights operator&(CastlingRights lhs, CastlingRights rhs) {
    return static_cast<CastlingRights>(
        static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs));
}

constexpr CastlingRights operator~(CastlingRights rhs) {
    return static_cast<CastlingRights>(~static_cast<uint8_t>(rhs) & 15);
}

constexpr CastlingRights& operator|=(CastlingRights& lhs, CastlingRights rhs) {
    lhs = lhs | rhs;
    return lhs;
}

constexpr CastlingRights& operator&=(CastlingRights& lhs, CastlingRights rhs) {
    lhs = lhs & rhs;
    return lhs;
}

// Check if specific right is set
constexpr bool has_castling_right(CastlingRights rights, CastlingRights specific) {
    return (static_cast<uint8_t>(rights) & static_cast<uint8_t>(specific)) != 0;
}

// Remove a castling right
constexpr CastlingRights remove_castling_right(CastlingRights rights, CastlingRights to_remove) {
    return static_cast<CastlingRights>(
        static_cast<uint8_t>(rights) & ~static_cast<uint8_t>(to_remove));
}

// Get the rook starting square for a given castling right
// Returns SQUARE_NONE if invalid
constexpr Square castling_rook_square(CastlingRights right) {
    switch (static_cast<uint8_t>(right)) {
        case 1: return 7;   // H1 - White kingside
        case 2: return 0;   // A1 - White queenside
        case 4: return 63;  // H8 - Black kingside
        case 8: return 56;  // A8 - Black queenside
        default: return 64; // SQUARE_NONE
    }
}

// Get the king starting square for a given color
constexpr Square king_start_square(Color c) {
    return (c == COLOR_WHITE) ? 4 : 60;  // E1 or E8
}

// Convert piece type to character (lowercase for black, uppercase for white)
// Returns '?' for invalid
char piece_to_char(Piece p);

// Convert character to piece
// Supports both cases: 'P'/'p' for pawn, etc.
// Returns PIECE_NO_PIECE for invalid
Piece char_to_piece(char c);

// Convert castling rights to FEN string component (e.g., "KQkq" or "-")
// Buffer must be at least 5 bytes
void castling_rights_to_string(CastlingRights rights, char* buffer);

// Parse FEN castling string to rights
// Supports standard FEN notation: K, Q, k, q, -
CastlingRights parse_castling_rights(std::string_view str);

}  // namespace thinmint::core
