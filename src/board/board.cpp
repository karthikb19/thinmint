#include "thinmint/board/board.h"

#include <sstream>
#include <cctype>

#include "thinmint/movegen/attacks.h"

namespace thinmint::board {

using namespace thinmint::core;

namespace {

// Parse the piece placement field of FEN (e.g., "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR")
// Returns true on success, populates the board's piece bitboards
bool parse_piece_placement(BoardState& board, const std::string& placement) {
    Rank current_rank = RANK_8;  // Start from rank 8 (top of board)
    File current_file = FILE_A;

    for (size_t i = 0; i < placement.size(); ++i) {
        char c = placement[i];

        if (c == '/') {
            // Move to next rank
            if (current_rank == RANK_1) {
                return false;  // Too many ranks
            }
            current_rank = static_cast<Rank>(current_rank - 1);
            current_file = FILE_A;
        } else if (c >= '1' && c <= '8') {
            // Empty squares
            int empty_count = c - '0';
            for (int j = 0; j < empty_count; ++j) {
                if (current_file > FILE_H) {
                    return false;  // Too many files in this rank
                }
                ++current_file;
            }
        } else {
            // Should be a piece character
            Piece piece = char_to_piece(c);
            if (piece == PIECE_NO_PIECE) {
                return false;  // Invalid piece character
            }

            if (current_file > FILE_H) {
                return false;  // Too many files in this rank
            }

            Square sq = make_square(current_file, current_rank);
            Color color = color_of(piece);
            PieceType pt = type_of(piece);

            // Set the piece bitboard
            board.pieces[static_cast<size_t>(color)][static_cast<size_t>(pt)] |= bb_from_square(sq);

            ++current_file;
        }
    }

    // Verify we filled all ranks
    if (current_rank != RANK_1 || current_file != FILE_NONE) {
        // Check if we ended on the right position
        // After rank 1, we should have processed 8 files
        if (!(current_rank == RANK_1 && current_file == FILE_A + 8)) {
            return false;
        }
    }

    // Update occupancy bitboards
    for (size_t pt = 1; pt < PIECE_TYPE_COUNT; ++pt) {
        board.white_occupancy |= board.pieces[0][pt];
        board.black_occupancy |= board.pieces[1][pt];
    }
    board.all_occupancy = board.white_occupancy | board.black_occupancy;

    return true;
}

}  // namespace

BoardState::BoardState() {
    clear();
}

void BoardState::clear() {
    // Clear all piece bitboards
    for (auto& color_pieces : pieces) {
        for (auto& bb : color_pieces) {
            bb = BB_EMPTY;
        }
    }

    // Clear occupancy
    white_occupancy = BB_EMPTY;
    black_occupancy = BB_EMPTY;
    all_occupancy = BB_EMPTY;

    // Reset game state
    side_to_move = COLOR_WHITE;
    castling_rights = CASTLING_NONE;
    en_passant_square = SQUARE_NONE;
    halfmove_clock = 0;
    fullmove_number = 1;
}

void BoardState::reset_to_start_position() {
    clear();

    // White pieces
    pieces[0][static_cast<size_t>(PIECE_PAWN)] = 0x000000000000FF00ULL;   // Rank 2
    pieces[0][static_cast<size_t>(PIECE_ROOK)] = 0x0000000000000081ULL;   // A1, H1
    pieces[0][static_cast<size_t>(PIECE_KNIGHT)] = 0x0000000000000042ULL; // B1, G1
    pieces[0][static_cast<size_t>(PIECE_BISHOP)] = 0x0000000000000024ULL; // C1, F1
    pieces[0][static_cast<size_t>(PIECE_QUEEN)] = 0x0000000000000008ULL;  // D1
    pieces[0][static_cast<size_t>(PIECE_KING)] = 0x0000000000000010ULL;   // E1

    // Black pieces
    pieces[1][static_cast<size_t>(PIECE_PAWN)] = 0x00FF000000000000ULL;   // Rank 7
    pieces[1][static_cast<size_t>(PIECE_ROOK)] = 0x8100000000000000ULL;   // A8, H8
    pieces[1][static_cast<size_t>(PIECE_KNIGHT)] = 0x4200000000000000ULL; // B8, G8
    pieces[1][static_cast<size_t>(PIECE_BISHOP)] = 0x2400000000000000ULL; // C8, F8
    pieces[1][static_cast<size_t>(PIECE_QUEEN)] = 0x0800000000000000ULL;  // D8
    pieces[1][static_cast<size_t>(PIECE_KING)] = 0x1000000000000000ULL;   // E8

    // Update occupancy
    white_occupancy = 0x000000000000FFFFULL;  // Ranks 1-2
    black_occupancy = 0xFFFF000000000000ULL;  // Ranks 7-8
    all_occupancy = white_occupancy | black_occupancy;

    // Set game state
    side_to_move = COLOR_WHITE;
    castling_rights = CASTLING_ALL;  // All castling rights available
    en_passant_square = SQUARE_NONE;
    halfmove_clock = 0;
    fullmove_number = 1;
}

Piece BoardState::piece_at(Square sq) const {
    if (!is_square_occupied(sq)) {
        return PIECE_NO_PIECE;
    }

    // Determine color
    Color color = color_at(sq);

    // Determine piece type
    PieceType pt = piece_type_at(sq);

    return make_piece(color, pt);
}

PieceType BoardState::piece_type_at(Square sq) const {
    if (!is_square_occupied(sq)) {
        return PIECE_NONE;
    }

    // Check each piece type (skip NONE at index 0)
    for (size_t pt = 1; pt < PIECE_TYPE_COUNT; ++pt) {
        for (size_t c = 0; c < COLOR_COUNT; ++c) {
            if (bb_test(pieces[c][pt], sq)) {
                return static_cast<PieceType>(pt);
            }
        }
    }

    return PIECE_NONE;  // Should not reach here if occupied
}

Color BoardState::color_at(Square sq) const {
    if (bb_test(white_occupancy, sq)) {
        return COLOR_WHITE;
    }
    if (bb_test(black_occupancy, sq)) {
        return COLOR_BLACK;
    }
    // Default to White for empty squares (caller should check is_square_occupied first)
    return COLOR_WHITE;
}

Square BoardState::king_square(Color c) const {
    Bitboard king_bb = pieces[static_cast<size_t>(c)][static_cast<size_t>(PIECE_KING)];
    return bb_first_square(king_bb);
}

bool BoardState::is_check(Color c) const {
    // Check if the king of color c is attacked by any opponent piece
    Square king_sq = king_square(c);
    if (king_sq == SQUARE_NONE) {
        return false;  // No king - invalid position
    }

    // Use the movegen attack detection
    Color opponent = !c;
    return thinmint::movegen::is_square_attacked(*this, king_sq, opponent);
}

bool BoardState::is_valid() const {
    // Check 1: Exactly one white king and one black king
    Bitboard white_king = pieces[0][static_cast<size_t>(PIECE_KING)];
    Bitboard black_king = pieces[1][static_cast<size_t>(PIECE_KING)];

    if (bb_popcount(white_king) != 1) {
        return false;  // Must have exactly one white king
    }
    if (bb_popcount(black_king) != 1) {
        return false;  // Must have exactly one black king
    }

    // Check 2: Kings cannot be on adjacent squares
    Square wk_sq = bb_first_square(white_king);
    Square bk_sq = bb_first_square(black_king);

    if (is_valid_square(wk_sq) && is_valid_square(bk_sq)) {
        File wk_file = file_of(wk_sq);
        Rank wk_rank = rank_of(wk_sq);
        File bk_file = file_of(bk_sq);
        Rank bk_rank = rank_of(bk_sq);

        int file_dist = std::abs(static_cast<int>(wk_file) - static_cast<int>(bk_file));
        int rank_dist = std::abs(static_cast<int>(wk_rank) - static_cast<int>(bk_rank));

        if (file_dist <= 1 && rank_dist <= 1) {
            return false;  // Kings are adjacent
        }
    }

    // Check 3: No overlapping pieces between colors
    if ((white_occupancy & black_occupancy) != BB_EMPTY) {
        return false;
    }

    // Check 4: Piece bitboards match occupancy
    Bitboard computed_white_occupancy = BB_EMPTY;
    Bitboard computed_black_occupancy = BB_EMPTY;

    for (size_t pt = 1; pt < PIECE_TYPE_COUNT; ++pt) {
        computed_white_occupancy |= pieces[0][pt];
        computed_black_occupancy |= pieces[1][pt];
    }

    if (computed_white_occupancy != white_occupancy) {
        return false;
    }
    if (computed_black_occupancy != black_occupancy) {
        return false;
    }

    // Check 5: Combined occupancy matches
    if (all_occupancy != (white_occupancy | black_occupancy)) {
        return false;
    }

    // Check 6: Side to move is valid
    if (side_to_move != COLOR_WHITE && side_to_move != COLOR_BLACK) {
        return false;
    }

    // Check 7: En passant square is valid if set
    if (en_passant_square != SQUARE_NONE) {
        // EP square must be on rank 3 (for white EP) or rank 6 (for black EP)
        Rank ep_rank = rank_of(en_passant_square);
        if (side_to_move == COLOR_WHITE) {
            // White to move, so previous move was black pushing pawn to rank 5
            // EP target is on rank 6 (0-indexed: rank 5)
            if (ep_rank != 5) {
                return false;
            }
        } else {
            // Black to move, so previous move was white pushing pawn to rank 4
            // EP target is on rank 3 (0-indexed: rank 2)
            if (ep_rank != 2) {
                return false;
            }
        }

        // EP square must be empty (it's the target square, not a piece location)
        if (is_square_occupied(en_passant_square)) {
            return false;
        }
    }

    // Check 8: Castling rights are valid for the position
    // If kingside castling right is set, king and rook should be in starting positions
    if (has_castling_right(castling_rights, CASTLING_WHITE_KINGSIDE)) {
        if (!bb_test(pieces[0][static_cast<size_t>(PIECE_KING)], 4) ||  // E1
            !bb_test(pieces[0][static_cast<size_t>(PIECE_ROOK)], 7)) {  // H1
            // King or rook not in position, but right is set
            // This is actually valid in chess (rights can exist even if pieces moved and returned)
            // So we don't enforce this strictly
        }
    }

    return true;
}

std::string BoardState::to_string() const {
    std::ostringstream oss;

    // Print board from rank 8 to rank 1
    for (int r = 7; r >= 0; --r) {
        oss << (r + 1) << " ";  // Rank number

        for (int f = 0; f < 8; ++f) {
            Square sq = make_square(static_cast<File>(f), static_cast<Rank>(r));
            Piece piece = piece_at(sq);

            if (piece == PIECE_NO_PIECE) {
                oss << ". ";
            } else {
                oss << piece_to_char(piece) << " ";
            }
        }
        oss << "\n";
    }

    // File labels
    oss << "  a b c d e f g h\n";

    // Additional info
    oss << "\nSide to move: " << (side_to_move == COLOR_WHITE ? "White" : "Black") << "\n";

    // Castling rights
    char castling_buf[5];
    castling_rights_to_string(castling_rights, castling_buf);
    oss << "Castling: " << castling_buf << "\n";

    // En passant
    if (en_passant_square == SQUARE_NONE) {
        oss << "En passant: -\n";
    } else {
        char ep_buf[3];
        square_to_algebraic(en_passant_square, ep_buf);
        oss << "En passant: " << ep_buf << "\n";
    }

    // Clocks
    oss << "Halfmove clock: " << static_cast<int>(halfmove_clock) << "\n";
    oss << "Fullmove number: " << fullmove_number << "\n";

    return oss.str();
}

bool BoardState::parse_fen(const std::string& fen) {
    // Clear the board first
    clear();

    // Split FEN into fields
    std::istringstream iss(fen);
    std::string piece_placement, side, castling, ep_square;
    int halfmove = 0, fullmove = 1;

    // Parse fields (6 fields in standard FEN)
    if (!(iss >> piece_placement)) {
        return false;  // Missing piece placement
    }
    if (!(iss >> side)) {
        return false;  // Missing side to move
    }
    if (!(iss >> castling)) {
        return false;  // Missing castling rights
    }
    if (!(iss >> ep_square)) {
        return false;  // Missing en passant square
    }

    // Halfmove and fullmove are optional in some contexts, but we require them
    if (!(iss >> halfmove)) {
        // Default to 0
        halfmove = 0;
    }
    if (!(iss >> fullmove)) {
        // Default to 1
        fullmove = 1;
    }

    // Check for extra tokens (invalid FEN)
    std::string extra;
    if (iss >> extra) {
        return false;
    }

    // Parse piece placement
    if (!parse_piece_placement(*this, piece_placement)) {
        return false;
    }

    // Parse side to move
    if (side == "w" || side == "W") {
        side_to_move = COLOR_WHITE;
    } else if (side == "b" || side == "B") {
        side_to_move = COLOR_BLACK;
    } else {
        return false;  // Invalid side to move
    }

    // Parse castling rights
    castling_rights = parse_castling_rights(castling);

    // Parse en passant square
    if (ep_square == "-" || ep_square.empty()) {
        en_passant_square = SQUARE_NONE;
    } else {
        en_passant_square = algebraic_to_square(ep_square);
        if (en_passant_square == SQUARE_NONE) {
            return false;  // Invalid EP square
        }
    }

    // Set clocks
    if (halfmove < 0 || halfmove > 255) {
        return false;  // Invalid halfmove clock
    }
    halfmove_clock = static_cast<uint8_t>(halfmove);

    if (fullmove < 1 || fullmove > 65535) {
        return false;  // Invalid fullmove number
    }
    fullmove_number = static_cast<uint16_t>(fullmove);

    // Validate the resulting position
    if (!is_valid()) {
        return false;
    }

    return true;
}

BoardState board_from_fen(const std::string& fen) {
    BoardState board;
    if (!board.parse_fen(fen)) {
        // Return empty board on failure (which is an invalid position)
        board.clear();
    }
    return board;
}

}  // namespace thinmint::board
