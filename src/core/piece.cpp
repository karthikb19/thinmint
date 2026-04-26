#include "thinmint/core/piece.h"

#include <cctype>

namespace thinmint::core {

char piece_to_char(Piece p) {
    PieceType pt = type_of(p);
    Color c = color_of(p);

    char piece_char;
    switch (pt) {
        case PIECE_PAWN:   piece_char = 'p'; break;
        case PIECE_KNIGHT: piece_char = 'n'; break;
        case PIECE_BISHOP: piece_char = 'b'; break;
        case PIECE_ROOK:   piece_char = 'r'; break;
        case PIECE_QUEEN:  piece_char = 'q'; break;
        case PIECE_KING:   piece_char = 'k'; break;
        default:           return '?';
    }

    return (c == COLOR_WHITE) ? static_cast<char>(std::toupper(static_cast<unsigned char>(piece_char))) : piece_char;
}

Piece char_to_piece(char c) {
    char lower = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    Color color = std::isupper(static_cast<unsigned char>(c)) ? COLOR_WHITE : COLOR_BLACK;

    switch (lower) {
        case 'p': return make_piece(color, PIECE_PAWN);
        case 'n': return make_piece(color, PIECE_KNIGHT);
        case 'b': return make_piece(color, PIECE_BISHOP);
        case 'r': return make_piece(color, PIECE_ROOK);
        case 'q': return make_piece(color, PIECE_QUEEN);
        case 'k': return make_piece(color, PIECE_KING);
        default:  return PIECE_NO_PIECE;
    }
}

void castling_rights_to_string(CastlingRights rights, char* buffer) {
    int idx = 0;

    if (has_castling_right(rights, CASTLING_WHITE_KINGSIDE)) {
        buffer[idx++] = 'K';
    }
    if (has_castling_right(rights, CASTLING_WHITE_QUEENSIDE)) {
        buffer[idx++] = 'Q';
    }
    if (has_castling_right(rights, CASTLING_BLACK_KINGSIDE)) {
        buffer[idx++] = 'k';
    }
    if (has_castling_right(rights, CASTLING_BLACK_QUEENSIDE)) {
        buffer[idx++] = 'q';
    }

    if (idx == 0) {
        buffer[idx++] = '-';
    }

    buffer[idx] = '\0';
}

CastlingRights parse_castling_rights(std::string_view str) {
    CastlingRights rights = CASTLING_NONE;

    if (str == "-" || str.empty()) {
        return rights;
    }

    for (char c : str) {
        switch (c) {
            case 'K': rights |= CASTLING_WHITE_KINGSIDE; break;
            case 'Q': rights |= CASTLING_WHITE_QUEENSIDE; break;
            case 'k': rights |= CASTLING_BLACK_KINGSIDE; break;
            case 'q': rights |= CASTLING_BLACK_QUEENSIDE; break;
            default: break;  // Ignore invalid characters
        }
    }

    return rights;
}

}  // namespace thinmint::core
