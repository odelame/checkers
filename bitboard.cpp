#include "bitboard.hpp"

unsigned int get_board_index(const unsigned int x, const unsigned int y);
std::pair<unsigned int, unsigned int> board_index_to_xy(const unsigned int index);

/**
 * @brief given an index to the bitboard return the corresponding x,y coordinates.
 *
 * @param index
 * @return std::pair<unsigned int, unsigned int>
 */
std::pair<unsigned int, unsigned int> board_index_to_xy(const unsigned int index) {
    return std::pair(((index & 3) << 1) | (1 & ((index >> 2) ^ 1)), index >> 2);
}
/**
 * @brief calculate the bit index for (x, y) coordinate.
 *
 * @param x
 * @param y
 * @return unsigned int
 */
unsigned int get_board_index(const unsigned int x, const unsigned int y) {
    return (x >> 1) + (y << 2);
}

BitBoard::BitBoard() {}
BitBoard::BitBoard(const BitBoard& other) :
    black_is_in(other.black_is_in), white_is_in(other.white_is_in), kings(other.kings) {}

bool BitBoard::operator==(const BitBoard& other) const {
    return this->black_is_in == other.black_is_in && this->white_is_in == other.white_is_in && this->kings == other.kings;
}

bool BitBoard::operator!=(const BitBoard& other) const {
    return this->black_is_in != other.black_is_in || this->white_is_in != other.white_is_in || this->kings != other.kings;
}

BitBoard& BitBoard::operator=(const BitBoard& other) {
    this->black_is_in = other.black_is_in;
    this->white_is_in = other.white_is_in;
    this->kings = other.kings;
    return *this;
}

/**
 * @brief hashes a bitboard
 *
 * @param board
 * @return std::size_t
 */
std::size_t BitBoard::hasher::operator()(const BitBoard& board) const {
    std::size_t seed = (std::size_t)board.black_is_in.to_ulong();
    seed |= (std::size_t)board.white_is_in.to_ulong() << (sizeof(std::size_t) * 8 - sizeof(board.white_is_in) * 8);
    boost::hash_combine(seed, board.kings.to_ulong() << (sizeof(std::size_t) * 4 - sizeof(board.kings)));
    return seed;
}

std::bitset<NUMBER_OF_REACHABLE_SQUARES> BitBoard::operator^(const BitBoard& other) const {
    return (this->black_is_in | this->white_is_in | this->kings) ^ (other.black_is_in | other.white_is_in | other.kings);
}

std::bitset<NUMBER_OF_REACHABLE_SQUARES> BitBoard::operator&(const BitBoard& other) const {
    return (this->black_is_in | this->white_is_in | this->kings) & (other.black_is_in | other.white_is_in | other.kings);
}

std::bitset<NUMBER_OF_REACHABLE_SQUARES> BitBoard::operator^(std::bitset<NUMBER_OF_REACHABLE_SQUARES> other) const {
    return (this->black_is_in | this->white_is_in | this->kings) ^ other;
}

std::bitset<NUMBER_OF_REACHABLE_SQUARES> BitBoard::operator&(std::bitset<NUMBER_OF_REACHABLE_SQUARES> other) const {
    return (this->black_is_in | this->white_is_in | this->kings) & other;
}

std::bitset<NUMBER_OF_REACHABLE_SQUARES> operator^(std::bitset<NUMBER_OF_REACHABLE_SQUARES> bits, const BitBoard& board) {
    return board ^ bits;
}
std::bitset<NUMBER_OF_REACHABLE_SQUARES> operator&(std::bitset<NUMBER_OF_REACHABLE_SQUARES> bits, const BitBoard& board) {
    return board & bits;
}

bool BitBoard::is_black(const unsigned int x, const unsigned int y) const {
    return this->black_is_in[get_board_index(x, y)];
}

bool BitBoard::is_white(const unsigned int x, const unsigned int y) const {
    return this->white_is_in[get_board_index(x, y)];
}

bool BitBoard::is_king(const unsigned int x, const unsigned int y) const {
    return this->kings[get_board_index(x, y)];
}

void BitBoard::set_king(const unsigned int x, const unsigned int y) {
    this->kings[get_board_index(x, y)] = true;
}

bool BitBoard::is_black(const std::pair<const unsigned int, const unsigned int> coords) const {
    return this->is_black(coords.first, coords.second);
}

bool BitBoard::is_white(const std::pair<const unsigned int, const unsigned int> coords) const {
    return this->is_white(coords.first, coords.second);
}

bool BitBoard::is_king(const std::pair<const unsigned int, const unsigned int> coords) const {
    return this->is_king(coords.first, coords.second);
}

void BitBoard::set_king(const std::pair<const unsigned int, const unsigned int> coords) {
    this->set_king(coords.first, coords.second);
}
/**
 * @brief given an index, checks if it is in the bounds of the board.
 *
 * @param index
 * @return true
 * @return false
 */
bool in_bounds(const unsigned int index) {
    return 0 <= index && index < NUM_COLS;
}

/**
 * @brief returns the number of white pieces on the board
 *
 * @return short
 */
short BitBoard::num_white() const {
    return this->white_is_in.count();
}

/**
 * @brief returns the number of black pieces on the board
 *
 * @return short
 */
short BitBoard::num_black() const {
    return this->black_is_in.count();
}

/**
 * @brief returns the number of white kings on the board
 *
 * @return short
 */
short BitBoard::num_white_kings() const {
    return (this->white_is_in & this->kings).count();
}

/**
 * @brief returns the number of black kings on the board
 *
 * @return short
 */
short BitBoard::num_black_kings() const {
    return (this->black_is_in & this->kings).count();
}

/**
 * @brief returns the number of pieces on the board
 *
 * @return short
 */
short BitBoard::piece_count() const {
    return this->num_black() + this->num_white();
}

/**
 * @brief returns the number of kings on the board
 *
 * @return short
 */
short BitBoard::king_count() const {
    return this->num_black_kings() + this->num_white_kings();
}

/**
 * @brief checks if it's legal to capture (dest_x, dest_y) from (source_x, source_y) assuming its blacks turn if black_turn
 *
 * @param black_turn
 * @param source_x
 * @param source_y
 * @param capture_x
 * @param capture_y
 * @return true
 * @return false
 */
bool BitBoard::legal_capture(bool black_turn, const unsigned int source_x, const unsigned int source_y, const unsigned int capture_x, const unsigned int capture_y) const {
    if (!in_bounds(get_end_capture_pos(source_x, capture_x)) || !in_bounds(get_end_capture_pos(source_y, capture_y)) || Piece::NONE != this->get(get_end_capture_pos(source_x, capture_x), get_end_capture_pos(source_y, capture_y)))
        return false;

    const Piece captured = this->get(capture_x, capture_y);
    const Piece source = this->get(source_x, source_y);

    if (!is_opposite_color(source, captured))
        return false;

    if (black_turn) {
        if (source == Piece::BLACK && (int)capture_y - (int)source_y == UP)
            return true;

        return source == Piece::BLACK_KING && std::abs((int)capture_y - (int)source_y) == 1;
    }
    else {
        if (source == Piece::WHITE && (int)capture_y - (int)source_y == DOWN)
            return true;

        return source == Piece::WHITE_KING && std::abs((int)capture_y - (int)source_y) == 1;
    }
}

/**
 * @brief checks if a move from (source_x, source_y) to (dest_x, dest_y) is legal, assumes no captures are possible
 *
 * @param black_turn
 * @param source_x
 * @param source_y
 * @param dest_x
 * @param dest_y
 * @return true
 * @return false
 */
bool BitBoard::legal_move(const bool black_turn, const unsigned int source_x, const unsigned int source_y, const unsigned int dest_x, const unsigned int dest_y) const {
    if (!in_bounds(dest_x) || !in_bounds(dest_y) || this->get(dest_x, dest_y) != Piece::NONE || std::abs((int)dest_x - (int)source_x) != 1)
        return false;

    const Piece source = this->get(source_x, source_y);
    if (black_turn)
        return (source == Piece::BLACK && (int)dest_y - (int)source_y == UP) ||
        (source == Piece::BLACK_KING && std::abs((int)dest_y - (int)source_y) == 1);
    else
        return (source == Piece::WHITE && (int)dest_y - (int)source_y == DOWN) ||
        (source == Piece::WHITE_KING && std::abs((int)dest_y - (int)source_y) == 1);
}

/**
 * @brief moves from (source_x, source_y) to (dest_x, dest_y) and gives new board result, assumes legal input
 *
 * @param source_x
 * @param source_y
 * @param dest_x
 * @param dest_y
 * @return BitBoard
 */
BitBoard BitBoard::move(const unsigned int source_x, const unsigned int source_y, const unsigned int dest_x, const unsigned int dest_y) const {
    BitBoard end_position(*this);
    end_position.set(dest_x, dest_y, end_position.get(source_x, source_y));
    if (0 == dest_y || NUM_ROWS - 1 == dest_y)
        end_position.set_king(dest_x, dest_y);
    end_position.set(source_x, source_y, Piece::NONE);
    return end_position;
}

/**
 * @brief captures (capture_x, capture_y) with (source_x, source_y) returns position result, assumes legal input
 *
 * @param source_x
 * @param source_y
 * @param capture_x
 * @param capture_y
 * @return BitBoard
 */
BitBoard BitBoard::capture(const unsigned int source_x, const unsigned int source_y, const unsigned int capture_x, const unsigned int capture_y) const {
    BitBoard end_position(*this);
    const auto [dest_x, dest_y] = get_end_capture_pos(source_x, source_y, capture_x, capture_y);
    end_position.set(dest_x, dest_y, end_position.get(source_x, source_y));
    if (0 == dest_y || NUM_ROWS - 1 == dest_y)
        end_position.set_king(dest_x, dest_y);
    end_position.set(capture_x, capture_y, Piece::NONE);
    end_position.set(source_x, source_y, Piece::NONE);
    return end_position;
}

/**
 * @brief returns all the legal captures from coordinates (x, y) given that is is blacks turn if black_turn.
 *
 * @param black_turn
 * @param x
 * @param y
 * @return std::vector<BitBoard>
 */
std::vector<BitBoard> BitBoard::captures(const bool black_turn, const unsigned int x, const unsigned int y) const {
    auto candidates = get_candidate_locations(x, y);
    std::vector<BitBoard> capture_positions;

    for (auto dest : candidates) {
        if (this->legal_capture(black_turn, x, y, dest.first, dest.second)) {
            auto new_position = this->capture(x, y, dest.first, dest.second);
            capture_positions.push_back(new_position);
        }
    }

    return capture_positions;
}

/**
 * @brief returns all possible non-capture moves from (x, y), assumes that there are no captures in the position
 *
 * @param black_turn
 * @param x
 * @param y
 * @return std::vector<BitBoard>
 */
std::vector<BitBoard> BitBoard::moves(const bool black_turn, const unsigned int x, const unsigned int y) const {
    auto candidates = get_candidate_locations(x, y);
    std::vector<BitBoard> moves;

    for (auto dest : candidates) {
        if (this->legal_move(black_turn, x, y, dest.first, dest.second)) {
            moves.push_back(this->move(x, y, dest.first, dest.second));
        }
    }

    return moves;
}

/**
 * @brief returns all the captures available on the board.
 *
 * @param black_turn
 * @return std::vector<BitBoard>
 */
std::vector<BitBoard> BitBoard::captures(const bool black_turn) const {
    std::vector<BitBoard> result;

    // generate all captures.
    for (auto [x, y] : *this) {
        auto new_moves = this->captures(black_turn, x, y);
        result.reserve(result.size() + new_moves.size());
        result.insert(result.end(), new_moves.begin(), new_moves.end());
    }

    return result;
}

/**
 * @brief returns all the legal moves on the board, if there are captures => returns them.
 *
 * @param black_turn
 * @return std::vector<BitBoard>
 */
std::vector<BitBoard> BitBoard::moves(const bool black_turn) const {
    std::vector<BitBoard> result = this->captures(black_turn);

    // if there are captures
    if (result.size() > 0)
        return result;

    // generate all moves.
    for (auto [x, y] : *this) {
        auto new_moves = this->moves(black_turn, x, y);
        result.reserve(result.size() + new_moves.size());
        result.insert(result.end(), new_moves.begin(), new_moves.end());
    }

    return result;
}

/**
 * @brief returns the piece on (x, y) coordinates, assumes legal coordinates.
 *
 * @param x
 * @param y
 * @return Piece
 */
Piece BitBoard::get(const unsigned int x, const unsigned int y) const {
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

/**
 * @brief sets the board at (x, y) to be Piece value, assumes legal coordinates.
 *
 * @param x
 * @param y
 * @param value
 */
void BitBoard::set(const unsigned int x, const unsigned int y, const Piece value) {
    int index = get_board_index(x, y);

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

BitBoard::Iterator::Iterator(const unsigned int index, const BitBoard& board) : index(index), board(board) {
    if (!board.white_is_in[this->index] && !board.black_is_in[this->index])
        this->skip_to_next();
}

BitBoard::Iterator BitBoard::begin() const {
    return BitBoard::Iterator(0, *this);
}

BitBoard::Iterator BitBoard::end() const {
    return BitBoard::Iterator(NUMBER_OF_REACHABLE_SQUARES, *this);
}

BitBoard::Iterator& BitBoard::Iterator::operator++() {
    this->skip_to_next();
    return *this;
}

BitBoard::Iterator& BitBoard::Iterator::operator--() {
    this->skip_to_pre();
    return *this;
}

BitBoard::Iterator BitBoard::Iterator::operator++(int) {
    auto copy = *this;
    ++(*this);
    return copy;
}

BitBoard::Iterator BitBoard::Iterator::operator--(int) {
    auto copy = *this;
    --(*this);
    return copy;
}

std::pair<unsigned int, unsigned int> BitBoard::Iterator::operator*() {
    return board_index_to_xy(this->index);
}

bool BitBoard::Iterator::operator==(const Iterator& other) const {
    return this->index == other.index;
}

bool BitBoard::Iterator::operator!=(const Iterator& other) const {
    return this->index != other.index;
}

void BitBoard::Iterator::skip_to_next() {
    this->index++;
    for (; this->index < NUMBER_OF_REACHABLE_SQUARES && !this->board.black_is_in[this->index] && !this->board.white_is_in[this->index]; this->index++);
}

void BitBoard::Iterator::skip_to_pre() {
    if (0 == this->index)
        return;
    this->index--;
    for (; 0 < this->index && !this->board.black_is_in[this->index] && !this->board.white_is_in[this->index]; this->index--);
}

std::stringstream& operator<<(std::stringstream& strm, Piece piece) {
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

std::stringstream& operator<<(std::stringstream& strm, const BitBoard& board) {
    for (unsigned int y = 0; y < NUM_ROWS; y++) {
        strm << '[';

        for (unsigned int x = (y + 1) & 1; x < NUM_COLS; x += 2) {
            if (y & 1) {
                strm << std::setw(8) << std::left << board.get(x, y);
                strm << std::setw(8) << std::left << "NONE";
            }
            else {
                strm << std::setw(8) << std::left << "NONE";
                strm << std::setw(8) << std::left << board.get(x, y);
            }
        }
        strm << ']' << std::endl;
    }

    return strm;
}

std::ostream& operator<<(std::ostream& strm, const BitBoard& board) {
    std::stringstream s;
    s << board;
    return (strm << s.str());
}

std::ostream& operator<<(std::ostream& strm, Piece piece) {
    std::stringstream s;
    s << piece;
    return (strm << s.str());
}
