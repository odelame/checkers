#include "bitboard.hpp"

BitBoard::BitBoard() : white_is_in(false), black_is_in(false), kings(false) {
    for (int i = 0; i < STARTING_ROWS * NUM_COLS >> 1; i++) {
        white_is_in[i] = true;
        black_is_in[NUMBER_OF_REACHABLE_SQUARES - i - 1] = true;
    }
}

BitBoard::ProxyIndexing::ProxyIndexing(const BitBoard* board, const int x) : board(board), x(x) {}

Piece BitBoard::ProxyIndexing::operator[](const int y) const {
    if (x % 2 == y % 2) {
        return Piece::NONE;
    }

    int index = (this->x >> 1) + (y << 2);

    if (board->black_is_in[index]) {
        if (board->kings[index])
            return Piece::BLACK_KING;
        return Piece::BLACK;
    }
    if (board->white_is_in[index]) {
        if (board->kings[index])
            return Piece::WHITE_KING;
        return Piece::WHITE;
    }
    return Piece::NONE;
}

BitBoard::ProxyIndexing BitBoard::operator[](const int x) const {
    return ProxyIndexing(this, x);
}

std::ostream& operator<<(std::ostream& strm, const BitBoard& board) {
    for (int y = 0; y < NUM_ROWS; y++) {
        strm << '[';

        for (int x = 0; x < NUM_COLS; x++) {
            switch (board[x][y]) {
            case Piece::NONE:
                strm << ' ';
                break;
            case Piece::BLACK:
                strm << 'b';
                break;
            case Piece::WHITE:
                strm << 'w';
                break;
            }
        }

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
        strm << "BLACK_KING";
        break;
    case Piece::WHITE_KING:
        strm << "WHITE_KING";
        break;
    }

    return strm;
}


int main() {
    BitBoard b;
    std::cout << b[2][1] << std::endl;
    std::cout << b[0][1] << std::endl;
    std::cout << b[1][5] << std::endl;

    std::cout << b;
}
