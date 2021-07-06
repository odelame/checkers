#include "gameapi.hpp"

CheckersApi::CheckersApi(const unsigned int depth, BitBoard board, bool black_turn) :
    depth(depth), board(board), black_turn(black_turn), all_moves(this->board.moves(this->get_black_turn())) {
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

bool CheckersApi::is_king(const int x, const int y) const {
    return this->get(x, y) == Piece::WHITE_KING || this->get(x, y) == Piece::BLACK_KING;
}

bool CheckersApi::game_over() const {
    return this->all_moves.size() == 0;
}

void CheckersApi::play() {
    this->board = std::get<0>(engine::best_move(this->board, this->get_black_turn(), this->depth));
    this->switch_turn();
    this->all_moves = this->board.moves(this->get_black_turn());
}

std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> CheckersApi::best_move() const {
    BitBoard next_best = std::get<0>(engine::best_move(this->board, this->get_black_turn(), this->depth));
    std::vector<std::pair<int, int>> path_to_end;

    iter_on_board([this, next_best, &path_to_end](const int x, const int y) {
        std::vector<BitBoard> reached_jumps = { this->board };
        std::vector<std::pair<int, int>> jump_coords = { {x, y} };
        std::vector<std::vector<std::pair<int, int>>> history = { jump_coords };

        while (jump_coords.size() != 0) {
            std::vector<BitBoard> new_reached_jumps;
            std::vector<std::pair<int, int>> new_coords_jumps;

            for (unsigned i = 0; i < jump_coords.size(); i++) {
                auto [source_x, source_y] = jump_coords[i];
                auto current_position = reached_jumps[i];
                auto& path = history[i];
                auto candidates = get_candidate_locations(source_x, source_y);

                bool captured = false;
                for (auto [capture_x, capture_y] : candidates) {
                    if (current_position.leagal_capture(black_turn, source_x, source_y, capture_x, capture_y)) {
                        new_reached_jumps.push_back(current_position.capture(source_x, source_y, capture_x, capture_y));
                        new_coords_jumps.push_back(get_end_capture_pos(source_x, source_y, capture_x, capture_y));
                        path.push_back(get_end_capture_pos(source_x, source_y, capture_x, capture_y));
                        captured = true;
                    }
                }

                if (!captured && current_position == next_best) {
                    path_to_end = std::move(path);
                }
            }

            reached_jumps = std::move(new_reached_jumps);
            jump_coords = std::move(new_coords_jumps);
        }

        });

    if (path_to_end.size()) {
        std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> result;
        result.reserve(path_to_end.size() - 1);
        for (unsigned i = 0; i < path_to_end.size() - 1; i++)
            result.emplace_back(path_to_end[i], path_to_end[i + 1]);

        return result;
    }

    std::bitset<NUMBER_OF_REACHABLE_SQUARES> source = (this->board ^ next_best) & this->board;
    std::bitset<NUMBER_OF_REACHABLE_SQUARES> end = next_best & (this->board ^ next_best);

    std::pair<int, int> source_xy;
    std::pair<int, int> end_xy;

    if (this->get_black_turn()) {
        for (unsigned i = 0; i < NUMBER_OF_REACHABLE_SQUARES; i++) {
            if (source[i] && this->board.is_black(board_index_to_xy(i))) {
                source_xy = board_index_to_xy(i);
            }
            if (end[i] && next_best.is_black(board_index_to_xy(i))) {
                end_xy = board_index_to_xy(i);
            }
        }
    }
    else {
        for (unsigned i = 0; i < NUMBER_OF_REACHABLE_SQUARES; i++) {
            if (source[i] && this->board.is_white(board_index_to_xy(i))) {
                source_xy = board_index_to_xy(i);
            }
            if (end[i] && next_best.is_white(board_index_to_xy(i))) {
                end_xy = board_index_to_xy(i);
            }
        }
    }

    return std::vector{ std::pair{source_xy, end_xy} };
}

std::pair<int, int> CheckersApi::hint() const {
    return this->best_move().front().first;
}

bool CheckersApi::captures_available() const {
    if (this->game_over())
        return false;
    BitBoard option = this->all_moves.front();
    std::bitset black_magic = option ^ this->board;

    int count = 0;
    for (unsigned int i = 1; i < black_magic.size(); i++)
        if (black_magic[i])
            count++;

    return count > 2;
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
        this->all_moves = this->board.moves(this->get_black_turn());

        return py::cast<py::none>(Py_None);
    }
    if (std::find(this->all_moves.begin(), this->all_moves.end(), after_move) != this->all_moves.end()) {
        this->board = after_move;
        this->switch_turn();
        this->all_moves = this->board.moves(this->get_black_turn());
        return py::cast<py::none>(Py_None);
    }

    return py::make_tuple(dest_x, dest_y);
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
        if (this->captures_available()) {
            for (auto [dest_x, dest_y] : candidates) {
                if (this->board.leagal_capture(this->get_black_turn(), x, y, dest_x, dest_y))
                    possibilities.push_back(get_end_capture_pos(x, y, dest_x, dest_y));
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




PYBIND11_MODULE(checkers, handle) {
    handle.doc() =
        "Basic checkers api to handle the checkers game\n"
        "CheckersApi.CheckersApi => constructor for the api.\n"
        "api.move() -> bool, checks if a move is leagal, if so plays it.\n";

    py::enum_<Piece>(handle, "Piece")
        .value("NONE", Piece::NONE)
        .value("BLACK", Piece::BLACK)
        .value("WHITE", Piece::WHITE)
        .value("BLACK_KING", Piece::BLACK_KING)
        .value("WHITE_KING", Piece::WHITE_KING)
        ;

    py::class_<CheckersApi>(handle, "Api")
        .def(py::init<unsigned int>())
        .def(py::init<>())
        .def("is_white", &CheckersApi::is_white, py::arg("x"), py::arg("y"))
        .def("is_black", &CheckersApi::is_black, py::arg("x"), py::arg("y"))
        .def("is_king", &CheckersApi::is_king, py::arg("x"), py::arg("y"))
        .def("moves", &CheckersApi::possible_moves, py::arg("x"), py::arg("y"))
        .def("move", &CheckersApi::move, py::arg("source_x"), py::arg("source_y"), py::arg("dest_x"), py::arg("dest_y"))
        .def("play", &CheckersApi::play)
        .def("hint", &CheckersApi::hint)
        .def("best_move", &CheckersApi::best_move)
        .def_property_readonly("black_move", &CheckersApi::get_black_turn)
        .def_property_readonly("game_over", &CheckersApi::game_over)
        .def("__getitem__",
            [](CheckersApi& self, py::tuple values) {
                std::tuple<int, int> indexes = values.cast<std::tuple<int, int>>();
                return self.get(std::get<0>(indexes), std::get<1>(indexes));
            },
            py::arg("index"))
        ;

}
