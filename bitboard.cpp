#include "bitboard.hpp"

BitBoard::BitBoard() {}
BitBoard::BitBoard(const BitBoard& other) :
    black_is_in(other.black_is_in), white_is_in(other.white_is_in), kings(other.kings) {}

bool BitBoard::operator==(const BitBoard& other) const {
    return this->black_is_in == other.black_is_in && this->white_is_in == other.white_is_in && this->kings == other.kings;
}

int get_index(const int x, const int y) {
    return (x >> 1) + (y << 2);
}

std::pair<int, int> get_xy(const int index) {
    return std::make_pair<int, int>(index & ((NUM_COLS - 1) >> 1), index >> 2);
}

bool BitBoard::is_black(const int x, const int y) const {
    return this->black_is_in[get_index(x, y)];
}

bool BitBoard::is_white(const int x, const int y) const {
    return this->white_is_in[get_index(x, y)];
}

bool BitBoard::is_king(const int x, const int y) const {
    return this->kings[get_index(x, y)];
}

void BitBoard::set_king(const int x, const int y) {
    this->kings[get_index(x, y)] = true;
}

bool in_bounds(const int index) {
    return 0 <= index && index < NUM_COLS;
}

bool BitBoard::leagal_capture(bool black_turn, const int source_x, const int source_y, const int capture_x, const int capture_y) const {
    if (!in_bounds(2 * capture_x - source_x) || !in_bounds(2 * capture_y - source_y) || Piece::NONE != this->get(2 * capture_x - source_x, 2 * capture_y - source_y))
        return false;

    const Piece captured = this->get(capture_x, capture_y);
    const Piece source = this->get(source_x, source_y);

    if (!is_opposite_color(source, captured))
        return false;

    if (black_turn) {
        if (source == Piece::BLACK && capture_y - source_y == UP)
            return true;

        return source == Piece::BLACK_KING && std::abs(capture_y - source_y) == 1;
    }
    else {
        if (source == Piece::WHITE && capture_y - source_y == DOWN)
            return true;

        return source == Piece::WHITE_KING && std::abs(capture_y - source_y) == 1;
    }
}

bool BitBoard::leagal_move(const bool black_turn, const int source_x, const int source_y, const int dest_x, const int dest_y) const {
    if (!in_bounds(dest_x) || !in_bounds(dest_y) || this->get(dest_x, dest_y) != Piece::NONE || std::abs(dest_x - source_x) != 1)
        return false;

    const Piece source = this->get(source_x, source_y);
    if (black_turn)
        return (source == Piece::BLACK && dest_y - source_y == UP) ||
        (source == Piece::BLACK_KING && std::abs(dest_y - source_y) == 1);
    else
        return (source == Piece::WHITE && dest_y - source_y == DOWN) ||
        (source == Piece::WHITE_KING && std::abs(dest_y - source_y) == 1);
}

BitBoard BitBoard::move(const int source_x, const int source_y, const int dest_x, const int dest_y) const {
    BitBoard end_position(*this);
    end_position.set(dest_x, dest_y, end_position.get(source_x, source_y));
    if (0 == dest_y || NUM_ROWS - 1 == dest_y)
        end_position.set_king(dest_x, dest_y);
    end_position.set(source_x, source_y, Piece::NONE);
    return end_position;
}

BitBoard BitBoard::capture(const int source_x, const int source_y, const int capture_x, const int capture_y) const {
    BitBoard end_position(*this);
    const int dest_x = 2 * capture_x - source_x;
    const int dest_y = 2 * capture_y - source_y;
    end_position.set(dest_x, dest_y, end_position.get(source_x, source_y));
    if (0 == dest_y || NUM_ROWS - 1 == dest_y)
        end_position.set_king(dest_x, dest_y);
    end_position.set(capture_x, capture_y, Piece::NONE);
    end_position.set(source_x, source_y, Piece::NONE);
    return end_position;
}


std::vector<BitBoard> BitBoard::captures(const bool black_turn, const int x, const int y) const {
    auto candidates = get_candidate_locations(x, y);
    std::vector<BitBoard> capture_positions;

    for (auto dest : candidates) {
        if (this->leagal_capture(black_turn, x, y, dest.first, dest.second)) {
            auto new_position = this->capture(x, y, dest.first, dest.second);
            capture_positions.push_back(new_position);
        }
    }

    return capture_positions;
}

std::vector<BitBoard> BitBoard::moves(const bool black_turn, const int x, const int y) const {
    auto candidates = get_candidate_locations(x, y);
    std::vector<BitBoard> moves;

    for (auto dest : candidates) {
        if (this->leagal_move(black_turn, x, y, dest.first, dest.second)) {
            auto new_position = this->move(x, y, dest.first, dest.second);
            moves.push_back(new_position);
        }
    }

    return moves;
}

Piece BitBoard::get(const int x, const int y) const {
    if (this->is_black(x, y)) {
        if (this->is_king(x, y))
            return Piece::BLACK_KING;
        return Piece::BLACK;
    }

    if (this->is_white(x, y)) {
        if (this->is_king(x, y))
            return Piece::WHITE_KING;
        return Piece::WHITE;
    }

    return Piece::NONE;
}

void BitBoard::set(const int x, const int y, const Piece value) {
    int index = get_index(x, y);

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
    case Piece::BLACK:
        this->black_is_in[index] = true;
        this->white_is_in[index] = false;
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
        std::cout << '[';

        for (int x = (y + 1) & 1; x < NUM_COLS; x += 2) {
            if (y & 1) {
                strm << std::setw(8) << std::left << board.get(x, y);
                strm << std::setw(8) << std::left << "NONE";
            }
            else {
                strm << std::setw(8) << std::left << "NONE";
                strm << std::setw(8) << std::left << board.get(x, y);
            }
        }
        std::cout << ']' << std::endl;
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
