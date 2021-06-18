#include "bitboard.hpp"

BitBoard::BitBoard() {}


Piece BitBoard::get(const int x, const int y) const {
    if (x % 2 == y % 2) {
        return Piece::NONE;
    }

    int index = (x >> 1) + (y << 2);

    if (this->black_is_in[index]) {
        if (this->kings[index])
            return Piece::BLACK_KING;
        return Piece::BLACK;
    }
    if (this->white_is_in[index]) {
        if (this->kings[index])
            return Piece::WHITE_KING;
        return Piece::WHITE;
    }
    return Piece::NONE;
}

void BitBoard::set(const int x, const int y, const Piece value) {
    if (x % 2 == y % 2) {
        return;
    }
    int index = (x >> 1) + (y << 2);

    switch (value) {
    case Piece::NONE:
        this->black_is_in[index] = false;
        this->white_is_in[index] = false;
        this->kings[index] = false;
        break;
    case Piece::WHITE:
        this->black_is_in[index] = false;
        this->white_is_in[index] = true;
        this->kings[index] = false;
        break;
    case Piece::WHITE_KING:
        this->black_is_in[index] = false;
        this->white_is_in[index] = true;
        this->kings[index] = true;
        break;
    case Piece::BLACK_KING:
        this->black_is_in[index] = true;
        this->white_is_in[index] = false;
        this->kings[index] = true;
        break;
    }
}

std::ostream& operator<<(std::ostream& strm, const BitBoard& board) {
    for (int y = 0; y < NUM_ROWS; y++) {
        strm << '[';

        for (int x = 0; x < NUM_COLS; x++)
            strm << std::setw(8) << std::left << board.get(x, y);

        strm << ']' << std::endl;
    }

    return strm;
}

std::ostream& operator<<(std::ostream& strm, Piece piece) {
    switch (piece) {
    case Piece::NONE:
        strm << "NONE";
        break;
    case Piece::BLACK:
        strm << "BLACK";
        break;
    case Piece::WHITE:
        strm << "WHITE";
        break;
    case Piece::BLACK_KING:
        strm << "K_BLACK";
        break;
    case Piece::WHITE_KING:
        strm << "K_WHITE";
        break;
    }

    return strm;
}
