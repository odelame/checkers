#include "helpFuncs.hpp"

std::vector<std::pair<int, int>> get_candidate_locations(const int x, const int y) {
    std::vector<std::pair<int, int>> candidates = { {x + 1, y + 1}, {x + 1, y - 1}, {x - 1, y + 1}, {x - 1, y - 1} };
    std::vector<std::pair<int, int>> locations;

    for (auto c : candidates) {
        if (NUM_COLS > c.first && c.first >= 0 && NUM_ROWS > c.second && c.second >= 0)
            locations.push_back(c);
    }

    return locations;
}

bool is_opposite_color(Piece source, Piece other) {
    if (source == Piece::BLACK || source == Piece::BLACK_KING)
        return other == Piece::WHITE || other == Piece::WHITE_KING;

    return (source == Piece::WHITE || source == Piece::WHITE_KING) && (other == Piece::BLACK || other == Piece::BLACK_KING);
}

void iter_on_board(std::function<void(int, int)> operation) {
    for (int x = 0; x < NUM_COLS; x++) {
        for (int y = (x + 1) & 1; y < NUM_ROWS; y += 2) {
            operation(x, y);
        }
    }
}

std::ostream& operator<<(std::ostream& strm, std::vector<BitBoard>& list) {
    for (auto b : list)
        strm << b << std::endl;

    return strm;
}

int get_end_capture_pos(const int source, const int capture) {
    return 2 * capture - source;
}

std::pair<int, int> get_end_capture_pos(const int source_x, const int source_y, const int capture_x, const int capture_y) {
    return std::make_pair(get_end_capture_pos(source_x, capture_x), get_end_capture_pos(source_y, capture_y));
}

int get_board_index(const int x, const int y) {
    return (x >> 1) + (y << 2);
}

std::pair<int, int> board_index_to_xy(const int index) {
    return std::make_pair<int, int>(((index & 3) << 1) | (((index >> 2) ^ 1) & 1), index >> 2);
}
