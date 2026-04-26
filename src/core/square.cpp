#include "thinmint/core/square.h"

#include <cctype>

namespace thinmint::core {

void square_to_algebraic(Square sq, char* buffer) {
    if (!is_valid_square(sq)) {
        buffer[0] = '-';
        buffer[1] = '\0';
        return;
    }
    File f = file_of(sq);
    Rank r = rank_of(sq);
    buffer[0] = static_cast<char>('a' + f);
    buffer[1] = static_cast<char>('1' + r);
    buffer[2] = '\0';
}

Square algebraic_to_square(std::string_view algebraic) {
    if (algebraic.size() < 2) {
        return SQUARE_NONE;
    }

    char file_char = static_cast<char>(std::tolower(static_cast<unsigned char>(algebraic[0])));
    char rank_char = algebraic[1];

    if (file_char < 'a' || file_char > 'h') {
        return SQUARE_NONE;
    }
    if (rank_char < '1' || rank_char > '8') {
        return SQUARE_NONE;
    }

    File f = static_cast<File>(file_char - 'a');
    Rank r = static_cast<Rank>(rank_char - '1');

    return make_square(f, r);
}

}  // namespace thinmint::core
