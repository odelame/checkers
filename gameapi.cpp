#include "gameapi.hpp"

CheckersApi::CheckersApi() : board(BitBoard()), black_turn(true), jumps_available(false) {}

bool is_opposite_color(const Piece source, const Piece other) {
    if (source == Piece::BLACK || source == Piece::BLACK_KING)
        return other == Piece::WHITE || other == Piece::WHITE_KING;

    return (source == Piece::WHITE || source == Piece::WHITE_KING) && (other == Piece::BLACK || other == Piece::BLACK_KING);
}

void CheckersApi::switch_turn() {
    this->black_turn = !this->black_turn;
}

bool CheckersApi::leagal_capture(const int source_x, const int source_y, const int capture_x, const int capture_y) {
    if (Piece::NONE != this->board.get(2 * capture_x - source_x, 2 * capture_y - source_y))
        return false;

    const Piece captured = this->board.get(capture_x, capture_y);
    const Piece source = this->board.get(source_x, source_y);

    if (!is_opposite_color(source, captured))
        return false;

    if (this->black_turn) {
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

bool CheckersApi::leagal_move(const int source_x, const int source_y, const int dest_x, const int dest_y) {
    if (this->jumps_available)
        return false;

    if (this->board.get(dest_x, dest_y) != Piece::NONE || std::abs(dest_x - source_x) != 1)
        return false;

    const Piece source = this->board.get(source_x, source_y);
    if (this->black_turn)
        return (source == Piece::BLACK && dest_y - source_y == UP) || (source == Piece::BLACK_KING && std::abs(dest_y - source_y) == 1);
    else
        return (source == Piece::WHITE && dest_y - source_y == DOWN) || (source == Piece::WHITE_KING && std::abs(dest_y - source_y) == 1);
}

bool CheckersApi::move(const int source_x, const int source_y, const int dest_x, const int dest_y) {
    if (this->leagal_capture(source_x, source_y, dest_x, dest_y)) {
        this->board = this->board.capture(source_x, source_y, dest_x, dest_y);
        this->switch_turn();
        return true;
    }

    if (this->leagal_move(source_x, source_y, dest_x, dest_y)) {
        this->board = this->board.move(source_x, source_y, dest_x, dest_y);
        this->switch_turn();
        return true;
    }

    return false;
}

std::vector<std::pair<int, int>> get_candidate_locations(const int x, const int y) {
    std::vector<std::pair<int, int>> candidates = { {x + 1, y + 1}, {x - 1, y + 1}, {x - 1, y - 1}, {x + 1, y - 1} };
    std::vector<std::pair<int, int>> locations;

    for (auto c : candidates) {
        if (NUM_COLS > c.first && c.first >= 0 && NUM_ROWS > c.second && c.second >= 0)
            locations.push_back(c);
    }

    return locations;
}

std::vector<BitBoard> CheckersApi::jumps(const int x, const int y) {
    auto candidates = get_candidate_locations(x, y);
    std::vector<BitBoard> jumps_positions;

    for (auto dest : candidates) {
        if (this->leagal_capture(x, y, dest.first, dest.second)) {
            auto new_position = this->board.capture(x, y, dest.first, dest.second);
            jumps_positions.push_back(new_position);
        }
    }

    return jumps_positions;
}

std::vector<BitBoard> CheckersApi::moves(const int x, const int y) {
    auto candidates = get_candidate_locations(x, y);
    std::vector<BitBoard> moves;

    for (auto dest : candidates) {
        if (this->leagal_move(x, y, dest.first, dest.second)) {
            auto new_position = this->board.move(x, y, dest.first, dest.second);
            moves.push_back(new_position);
        }
    }

    return moves;
}

std::vector<BitBoard> CheckersApi::moves() {
    std::vector<BitBoard> possible_positions_jumps;
    std::vector<BitBoard> possible_positions_moves;


    if (this->black_turn) {
        for (int x = 0; x < NUM_COLS; x++) {
            for (int y = (x + 1) & 1; y < NUM_ROWS; y += 2) {
                if (this->board.is_black(x, y)) {
                    auto moves = this->moves(x, y);
                    possible_positions_moves.reserve(possible_positions_moves.size() + moves.size());
                    possible_positions_moves.insert(possible_positions_moves.end(), moves.begin(), moves.end());
                    auto jumps = this->jumps(x, y);
                    possible_positions_jumps.reserve(possible_positions_jumps.size() + jumps.size());
                    possible_positions_jumps.insert(possible_positions_jumps.end(), jumps.begin(), jumps.end());
                }
            }
        }
    }
    else {
        for (int x = 0; x < NUM_COLS; x++) {
            for (int y = (x + 1) & 1; y < NUM_ROWS; y += 2) {
                if (this->board.is_white(x, y)) {
                    auto moves = this->moves(x, y);
                    possible_positions_moves.reserve(possible_positions_moves.size() + moves.size());
                    possible_positions_moves.insert(possible_positions_moves.end(), moves.begin(), moves.end());
                    auto jumps = this->jumps(x, y);
                    possible_positions_jumps.reserve(possible_positions_jumps.size() + jumps.size());
                    possible_positions_jumps.insert(possible_positions_jumps.end(), jumps.begin(), jumps.end());
                }
            }
        }
    }

    if (possible_positions_jumps.size() > 0)
        return possible_positions_jumps;
    return possible_positions_moves;
}

std::ostream& operator<<(std::ostream& strm, std::vector<BitBoard>& list) {
    for (auto b : list)
        strm << b << std::endl;

    return strm;
}

int main(int argc, char** argv) {
    CheckersApi api;

    // std::cout << api.board << std::endl;
    auto x = api.moves();
    std::cout << x << std::endl;
}

