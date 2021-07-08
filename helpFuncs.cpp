#include "helpFuncs.hpp"
/**
 * @brief return all the possible neighboring locations of x, y
 *
 * @param x
 * @param y
 * @return std::vector<std::pair<unsigned int, unsigned int>>
 */
std::vector<std::pair<unsigned int, unsigned int>> get_candidate_locations(const unsigned int x, const unsigned int y) {
    std::vector<std::pair<unsigned int, unsigned int>> candidates = { {x + 1, y + 1}, {x + 1, y - 1}, {x - 1, y + 1}, {x - 1, y - 1} };
    std::vector<std::pair<unsigned int, unsigned int>> locations;

    for (auto c : candidates) {
        if (in_bounds(c.first) && in_bounds(c.second))
            locations.push_back(c);
    }

    return locations;
}

/**
 * @brief get two pieces and check if they are of opposite colors.
 *
 * @param source
 * @param other
 * @return true
 * @return false
 */
bool is_opposite_color(Piece source, Piece other) {
    if (source == Piece::BLACK || source == Piece::BLACK_KING)
        return other == Piece::WHITE || other == Piece::WHITE_KING;

    return (source == Piece::WHITE || source == Piece::WHITE_KING) && (other == Piece::BLACK || other == Piece::BLACK_KING);
}

/**
 * @brief iter on all the leagall indexes in a board, activate operation on each one.
 *
 * @param operation
 */
void iter_on_board(std::function<void(unsigned int x, unsigned int y)> operation) {
    for (unsigned int x = 0; x < NUM_COLS; x++) {
        for (unsigned int y = (x + 1) & 1; y < NUM_ROWS; y += 2) {
            operation(x, y);
        }
    }
}

/**
 * @brief iter on all the leagall indexes in a board, activate operation on each one, stop if run is set to false.
 *
 * @param operation
 */
void iter_on_board(std::function<void(unsigned int x, unsigned int y, bool& run)> operation) {
    bool done = false;
    for (unsigned int x = 0; x < NUM_COLS; x++) {
        for (unsigned int y = (x + 1) & 1; y < NUM_ROWS; y += 2) {
            operation(x, y, done);
            if (done)
                return;
        }
    }
}

std::ostream& operator<<(std::ostream& strm, std::vector<BitBoard>& list) {
    for (auto b : list)
        strm << b << std::endl;

    return strm;
}

/**
 * @brief if source is part of a coordinate which is capturing capture => return the result partial coordinate, assumes both are x coordinate ot both are y coordinate
 *
 * @param source
 * @param capture
 * @return unsigned int
 */
unsigned int get_end_capture_pos(const unsigned int source, const unsigned int capture) {
    return 2 * capture - source;
}

/**
 * @brief if (source_x, source_y) is capturing (capture_x, capture_y) return the destination coordinate.
 *
 * @param source_x
 * @param source_y
 * @param capture_x
 * @param capture_y
 * @return std::pair<unsigned int, unsigned int>
 */
std::pair<unsigned int, unsigned int> get_end_capture_pos(const unsigned int source_x, const unsigned int source_y, const unsigned int capture_x, const unsigned int capture_y) {
    return std::make_pair(get_end_capture_pos(source_x, capture_x), get_end_capture_pos(source_y, capture_y));
}
