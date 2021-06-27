#include "gameapi.hpp"

CheckersApi::CheckersApi(BitBoard board, bool black_turn) :
    board(board), black_turn(black_turn), captures_available(false), all_moves(this->get_moves()) {
}


void CheckersApi::switch_turn() {
    this->black_turn = !this->black_turn;
}

bool CheckersApi::get_black_turn() const {
    return this->black_turn;
}

bool CheckersApi::is_black(const int x, const int y) const {
    return this->get(x, y) == Piece::BLACK || this->get(x, y) == Piece::BLACK_KING;
}

bool CheckersApi::is_white(const int x, const int y) const {
    return this->get(x, y) == Piece::WHITE || this->get(x, y) == Piece::WHITE_KING;
}

std::ostream& operator<<(std::ostream& strm, std::vector<BitBoard>& list) {
    for (auto b : list)
        strm << b << std::endl;

    return strm;
}

py::object CheckersApi::move(const int source_x, const int source_y, const int dest_x, const int dest_y) {
    BitBoard after_move = this->board.move(source_x, source_y, dest_x, dest_y);
    BitBoard after_capture = this->board.capture(source_x, source_y, (dest_x + source_x) / 2, (dest_y + source_y) / 2);

    if (std::find(this->all_moves.begin(), this->all_moves.end(), after_capture) != this->all_moves.end()) {
        this->board = after_capture;

        auto continuations = after_capture.captures(this->get_black_turn(), dest_x, dest_y);
        if (continuations.size() > 0) {
            this->all_moves = continuations;
            return py::make_tuple(dest_x, dest_y);
        }

        this->switch_turn();
        this->all_moves = this->get_moves();

        return py::make_tuple(dest_x, dest_y);
    }

    if (std::find(this->all_moves.begin(), this->all_moves.end(), after_move) != this->all_moves.end()) {
        this->board = after_move;
        this->switch_turn();
        this->all_moves = this->get_moves();

        return py::make_tuple(dest_x, dest_y);
    }

    return py::cast<py::none>(Py_None);
}

Piece CheckersApi::get(const int x, const int y) const {
    if ((x & 1) == (y & 1))
        return Piece::NONE;
    return this->board.get(x, y);
}

std::vector<std::pair<int, int>> CheckersApi::possible_moves(const int x, const int y) const {
    auto candidates = get_candidate_locations(x, y);
    std::vector<std::pair<int, int>> possibilities;

    if (this->get(x, y) != Piece::NONE) {
        if (this->captures_available) {
            for (auto [dest_x, dest_y] : candidates) {
                if (this->board.leagal_capture(this->get_black_turn(), x, y, dest_x, dest_y))
                    possibilities.emplace_back(2 * dest_x - x, 2 * dest_y - y);
            }
        }
        else {
            for (auto [dest_x, dest_y] : candidates) {
                if (this->board.leagal_move(this->get_black_turn(), x, y, dest_x, dest_y))
                    possibilities.emplace_back(dest_x, dest_y);
            }
        }
    }

    return possibilities;
}

std::vector<BitBoard> CheckersApi::get_moves() {
    std::vector<BitBoard> possible_positions_captures;
    std::vector<BitBoard> possible_positions_moves;
    this->captures_available = false;

    for (int x = 0; x < NUM_COLS; x++) {
        for (int y = (x + 1) & 1; y < NUM_ROWS; y += 2) {
            auto moves = this->board.moves(this->get_black_turn(), x, y);
            possible_positions_moves.reserve(possible_positions_moves.size() + moves.size());
            possible_positions_moves.insert(possible_positions_moves.end(), moves.begin(), moves.end());

            auto captures = this->board.captures(this->get_black_turn(), x, y);
            possible_positions_captures.reserve(possible_positions_captures.size() + captures.size());
            possible_positions_captures.insert(possible_positions_captures.end(), captures.begin(), captures.end());
        }
    }

    if (possible_positions_captures.size() > 0) {
        this->captures_available = true;
        return possible_positions_captures;
    }
    return possible_positions_moves;
}
