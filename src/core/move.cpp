#include "thinmint/core/move.h"

#include <charconv>
#include <sstream>

namespace thinmint::core {

std::string move_to_uci(Move m) {
    if (m == MOVE_NONE) {
        return "";
    }

    Square from = from_square(m);
    Square to = to_square(m);

    char buffer[6] = {};  // e2e4q + null terminator
    buffer[0] = 'a' + file_of(from);
    buffer[1] = '1' + rank_of(from);
    buffer[2] = 'a' + file_of(to);
    buffer[3] = '1' + rank_of(to);

    if (is_promotion(m)) {
        PieceType pt = promotion_type(m);
        switch (pt) {
            case PIECE_KNIGHT: buffer[4] = 'n'; break;
            case PIECE_BISHOP: buffer[4] = 'b'; break;
            case PIECE_ROOK:   buffer[4] = 'r'; break;
            case PIECE_QUEEN:  buffer[4] = 'q'; break;
            default: break;
        }
    }

    return std::string(buffer);
}

Move uci_to_move(std::string_view uci) {
    // UCI format: from square (2 chars) + to square (2 chars) + optional promotion piece
    // Examples: "e2e4", "e7e8q", "a7a8n"

    if (uci.length() < 4 || uci.length() > 5) {
        return MOVE_NONE;
    }

    // Parse from square
    char from_file = uci[0];
    char from_rank = uci[1];
    if (from_file < 'a' || from_file > 'h' || from_rank < '1' || from_rank > '8') {
        return MOVE_NONE;
    }
    Square from = make_square(from_file - 'a', from_rank - '1');

    // Parse to square
    char to_file = uci[2];
    char to_rank = uci[3];
    if (to_file < 'a' || to_file > 'h' || to_rank < '1' || to_rank > '8') {
        return MOVE_NONE;
    }
    Square to = make_square(to_file - 'a', to_rank - '1');

    // Check for promotion
    if (uci.length() == 5) {
        char promo = uci[4];
        PieceType promo_type;
        switch (promo) {
            case 'n': promo_type = PIECE_KNIGHT; break;
            case 'b': promo_type = PIECE_BISHOP; break;
            case 'r': promo_type = PIECE_ROOK; break;
            case 'q': promo_type = PIECE_QUEEN; break;
            default: return MOVE_NONE;  // Invalid promotion piece
        }
        return make_promotion(from, to, promo_type);
    }

    return make_move(from, to);
}

std::string move_to_string(Move m) {
    if (m == MOVE_NONE) {
        return "(none)";
    }

    Square from = from_square(m);
    Square to = to_square(m);

    std::ostringstream oss;

    // Basic format: from-to
    oss << static_cast<char>('a' + file_of(from))
        << static_cast<char>('1' + rank_of(from))
        << (is_capture(m) ? 'x' : '-')
        << static_cast<char>('a' + file_of(to))
        << static_cast<char>('1' + rank_of(to));

    if (is_promotion(m)) {
        PieceType pt = promotion_type(m);
        oss << "=";
        switch (pt) {
            case PIECE_KNIGHT: oss << 'N'; break;
            case PIECE_BISHOP: oss << 'B'; break;
            case PIECE_ROOK:   oss << 'R'; break;
            case PIECE_QUEEN:  oss << 'Q'; break;
            default: break;
        }
    }

    if (is_castling(m)) {
        return (to > from) ? "O-O" : "O-O-O";
    }

    if (is_en_passant(m)) {
        oss << " ep";
    }

    return oss.str();
}

}  // namespace thinmint::core
