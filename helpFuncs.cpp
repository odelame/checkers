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

