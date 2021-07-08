#include "gameapi.hpp"

CheckersApi::CheckersApi(const unsigned int depth, BitBoard board, bool black_turn) :
    depth(depth), board(board), black_turn(black_turn), draw(false), all_moves(this->board.moves(this->get_black_turn())), engine(Engine()) {
}

/**
 * @brief updated the board position, also handeles keeping track of how many moves since the last significant one happened
 *
 * @param new_board
 */
void CheckersApi::set_board(BitBoard new_board) {
    if (new_board.piece_count() == this->board.piece_count())
        this->engine.increment_since_capture();
    else
        this->engine.reset_since_capture();

    this->board = new_board;
}

/**
 * @brief switches the turn, and updates the draw variable while at it.
 *
 */
void CheckersApi::switch_turn() {
    this->black_turn = !this->black_turn;
    this->draw = this->draw || (REPETITION_DRAW == engine.increment_position_history_counter(Position(this->board, this->get_black_turn())))
        || this->engine.get_since_capture() >= NO_CAPTURE_DRAW;
}

bool CheckersApi::get_black_turn() const {
    return this->black_turn;
}

bool CheckersApi::is_black(const unsigned int x, const unsigned int y) const {
    return this->get(x, y) == Piece::BLACK || this->get(x, y) == Piece::BLACK_KING;
}

bool CheckersApi::is_white(const unsigned int x, const unsigned int y) const {
    return this->get(x, y) == Piece::WHITE || this->get(x, y) == Piece::WHITE_KING;
}

bool CheckersApi::is_king(const unsigned int x, const unsigned int y) const {
    return this->get(x, y) == Piece::WHITE_KING || this->get(x, y) == Piece::BLACK_KING;
}

bool CheckersApi::game_over() const {
    return this->all_moves.size() == 0 || this->draw;
}

/**
 * @brief returns: has this game been drawn.
 *
 * @return true
 * @return false
 */
bool CheckersApi::game_drawn() const {
    return this->draw;
}

/**
 * @brief plays the best move accoarsing to the engine.
 *
 * @return short
 */
short CheckersApi::play() {
    auto [best_position, eval] = engine.best_move(this->board, this->get_black_turn(), this->depth);
    this->set_board(best_position);
    this->switch_turn();
    this->all_moves = this->board.moves(this->get_black_turn());
    return eval;
}

/**
 * @brief returns a vector which holds the series of move representing the best move, as well as an evaluation.
 *
 * @return std::pair<std::vector<std::pair<std::pair<unsigned int, unsigned int>, std::pair<unsigned int, unsigned int>>>, short>
 */
std::pair<std::vector<std::pair<std::pair<unsigned int, unsigned int>, std::pair<unsigned int, unsigned int>>>, short> CheckersApi::best_move() {
    auto [next_best, eval] = engine.best_move(this->board, this->get_black_turn(), this->depth);
    std::vector<std::pair<unsigned int, unsigned int>> path_to_end;

    for (auto [x, y] : this->board) {
        if (this->get_black_turn() && !this->board.is_black(x, y))
            continue;
        if (!this->get_black_turn() && !this->board.is_white(x, y))
            continue;
        // if we had a capture, or a series of captures, then we need to build the path.
        //for (auto [x, y] : this->board) {
            // locations that where reached.
        std::vector<BitBoard> reached_jumps = { this->board };
        // coordinates that where reached: jump_coords[i] will be the landing coordinate to get to reaches_jumps[i]
        std::vector<std::pair<unsigned int, unsigned int>> jump_coords = { {x, y} };
        // all of the current move paths.
        std::vector<std::vector<std::pair<unsigned int, unsigned int>>> history = { jump_coords };

        while (jump_coords.size() != 0) {
            // the next iterations of the previos vectors.
            std::vector<BitBoard> new_reached_jumps;
            std::vector<std::pair<unsigned int, unsigned int>> new_coords_jumps;
            std::vector<std::vector<std::pair<unsigned int, unsigned int>>> new_history;

            for (unsigned i = 0; i < jump_coords.size(); i++) {
                // for each position that was reached:
                auto [source_x, source_y] = jump_coords[i];
                auto current_position = reached_jumps[i];
                auto& path = history[i];
                // get all the candidates for the next capture:
                auto candidates = get_candidate_locations(source_x, source_y);

                // get all the next possible positions from this position:
                bool captured = false;
                for (auto [capture_x, capture_y] : candidates) {
                    if (current_position.leagal_capture(black_turn, source_x, source_y, capture_x, capture_y)) {
                        new_reached_jumps.push_back(current_position.capture(source_x, source_y, capture_x, capture_y));
                        new_coords_jumps.push_back(get_end_capture_pos(source_x, source_y, capture_x, capture_y));
                        path.push_back(get_end_capture_pos(source_x, source_y, capture_x, capture_y));
                        new_history.push_back(path);
                        captured = true;
                    }
                }

                // if we found the right one: we have the path
                if (!captured && current_position == next_best) {
                    // move the path to path_to_end and exit this lambda.
                    path_to_end = std::move(path);
                    goto BREAK_ALL_LOOPS;
                }
            }

            // update the relevent reached positions, last coords, and the paths.
            reached_jumps = std::move(new_reached_jumps);
            jump_coords = std::move(new_coords_jumps);
            history = std::move(new_history);
        }
    }

BREAK_ALL_LOOPS:

    // if the position was reaches
    if (path_to_end.size() != 0) {
        // convert it to the format that is needed for the return value.
        std::vector<std::pair<std::pair<unsigned int, unsigned int>, std::pair<unsigned int, unsigned int>>> result_path;
        result_path.reserve(path_to_end.size() - 1);
        for (unsigned i = 0; i < path_to_end.size() - 1; i++)
            result_path.emplace_back(path_to_end[i], path_to_end[i + 1]);

        // finally return it.
        return std::pair(result_path, eval);
    }

    // else it was not reached via captures, get all the possible next moves and the move serires to get to them: 
    for (auto [x, y] : this->board) {
        auto candidates = get_candidate_locations(x, y);
        for (auto [dest_x, dest_y] : candidates) {
            if (this->board.leagal_move(this->get_black_turn(), x, y, dest_x, dest_y) && next_best == this->board.move(x, y, dest_x, dest_y))
                return std::pair{ std::vector{ std::pair{std::pair(x, y), std::pair(dest_x, dest_y)} }, eval };

        }
    }

    // unreachable default return value
    return std::pair{ std::vector{ std::pair{std::pair(0u, 0u), std::pair(0u, 0u)} }, 0 };
}

/**
 * @brief returns the coords for the first move in the best move
 *
 */
std::pair<unsigned int, unsigned int> CheckersApi::hint() {
    return this->best_move().first.front().first;
}

/**
 * @brief checks if there are captures available in the position.
 *
 * @return true
 * @return false
 */
bool CheckersApi::captures_available() const {
    if (this->game_over())
        return false;
    BitBoard option = this->all_moves.front();
    std::bitset black_magic = option ^ this->board;

    unsigned int count = 0;
    for (unsigned int i = 1; i < black_magic.size(); i++)
        if (black_magic[i])
            count++;

    return count > 2;
}

/**
 * @brief attempts to play a move, returns the end position if move was not successfull otherwise returns None
 * NOTE: This function was written entirely for python, and is not meant to be used within c++ source code.
 *
 * @param source_x
 * @param source_y
 * @param dest_x
 * @param dest_y
 * @return py::object
 */
py::object CheckersApi::move(const unsigned int source_x, const unsigned int source_y, const unsigned int dest_x, const unsigned int dest_y) {
    // get the after position if this move was a regular move or a capture
    BitBoard after_move = this->board.move(source_x, source_y, dest_x, dest_y);
    BitBoard after_capture = this->board.capture(source_x, source_y, (dest_x + source_x) / 2, (dest_y + source_y) / 2);

    // check if it was a capture and a leagal one (all_moves holds all leagal moves)
    if (std::find(this->all_moves.begin(), this->all_moves.end(), after_capture) != this->all_moves.end()) {
        // update the position
        this->set_board(after_capture);

        // check if there are possible further captures, if so turn is not passed and all_moves is updated.
        auto continuations = after_capture.captures(this->get_black_turn(), dest_x, dest_y);
        if (continuations.size() > 0) {
            this->all_moves = std::move(continuations);
            return py::make_tuple(dest_x, dest_y);
        }

        // switch the turn and update all_moves.
        this->switch_turn();
        this->all_moves = this->board.moves(this->get_black_turn());

        return py::cast<py::none>(Py_None);
    }
    // if it was a regular move and a leagal one.
    if (std::find(this->all_moves.begin(), this->all_moves.end(), after_move) != this->all_moves.end()) {
        // update the position and the leagal moves, pass the turn.
        this->set_board(after_move);
        this->switch_turn();
        this->all_moves = this->board.moves(this->get_black_turn());
        return py::cast<py::none>(Py_None);
    }

    return py::make_tuple(dest_x, dest_y);
}

/**
 * @brief returns the piece at position x, y, checks if input is leagall
 *
 * @param x
 * @param y
 * @return Piece
 */
Piece CheckersApi::get(const unsigned int x, const unsigned int y) const {
    // if the input is illegal => return empty piece.
    if ((x & 1) == (y & 1) || !in_bounds(x) || !in_bounds(y))
        return Piece::NONE;
    // return the piece.
    return this->board.get(x, y);
}

/**
 * @brief get all the possible leagall destinations moves from coordinates (x, y)
 *
 * @param x
 * @param y
 * @return std::vector<std::pair<unsigned int, unsigned int>>
 */
std::vector<std::pair<unsigned int, unsigned int>> CheckersApi::possible_moves(const unsigned int x, const unsigned int y) const {
    auto candidates = get_candidate_locations(x, y);
    std::vector<std::pair<unsigned int, unsigned int>> possibilities;

    if (this->get(x, y) != Piece::NONE) {
        // if there are captures: check for captures and return the found ones.
        if (this->captures_available()) {
            for (auto [dest_x, dest_y] : candidates) {
                if (this->board.leagal_capture(this->get_black_turn(), x, y, dest_x, dest_y))
                    possibilities.push_back(get_end_capture_pos(x, y, dest_x, dest_y));
            }
        }
        // if there are no captures: check for regular moves and return them.
        else {
            for (auto [dest_x, dest_y] : candidates) {
                if (this->board.leagal_move(this->get_black_turn(), x, y, dest_x, dest_y))
                    possibilities.emplace_back(dest_x, dest_y);
            }
        }
    }

    return possibilities;
}

std::stringstream& operator<<(std::stringstream& strm, CheckersApi& api) {
    if (api.get_black_turn())
        strm << "BLACK" << std::endl;
    else
        strm << "WHITE" << std::endl;

    return (strm << api.board);
}

PYBIND11_MODULE(checkers, handle) {
    handle.doc() =
        "Basic checkers api to handle the checkers game\n"
        "Api.Api => constructor for the api.\n"
        "Api.is_white(x: int, y: int) -> bool, checks if the piece at (x, y) coordinates is white\n"
        "Api.is_black(x: int, y: int) -> bool, checks if the piece at (x, y) coordinates is black\n"
        "Api.is_king(x: int, y: int) -> bool, checks if the piece at (x, y) coordinates is a king\n"
        "Api.leagal_moves(x: int, y: int) -> list[tuple[int, int]], returns a list of all leagal destinations from x, y coordinates\n"
        "Api.move(source_x: int, source_y: int, dest_x: int, dest_: int) -> tuple, checks if a move is leagal, if so plays it, returns None if move is not leagall otherwise returns the end position\n"
        "Api.play() -> None, plays the best move according to the engine.\n"
        "Api.best_move() -> tuple[list[tuple[tuple[int, int], tuple[int, int]]], int], returns a list representing the best move, and the evaluation of the end position from that move\n"
        "Api.__len__() -> int, returns the length or width of the board (equal)\n"
        "Api.__str__() -> str, returns the board in a string format as well as who has the move on the top\n"
        "Api.black_move -> bool, is it's blacks turn\n"
        "Api.white_move -> bool, is it's blacks turn\n"
        "Api.game_over -> bool, has the game ended\n"
        "Api.draw -> bool, has the game ended in a draw\n"
        "Api.__getitem__(index: tuple[int, int]) -> Piece, returns the piece at coordinates (x, y)\n"
        "Piece.__str__() -> str, return the string representation of the piece\n"
        ;
    py::enum_<Piece>(handle, "Piece")
        .value("NONE", Piece::NONE)
        .value("BLACK", Piece::BLACK)
        .value("WHITE", Piece::WHITE)
        .value("BLACK_KING", Piece::BLACK_KING)
        .value("WHITE_KING", Piece::WHITE_KING)
        .def("__str__", [](Piece& self) { std::stringstream s; s << self; return s.str(); })
        ;

    py::class_<CheckersApi>(handle, "Api")
        .def(py::init<unsigned int>())
        .def(py::init<>())
        .def("is_white", &CheckersApi::is_white, py::arg("x"), py::arg("y"))
        .def("is_black", &CheckersApi::is_black, py::arg("x"), py::arg("y"))
        .def("is_king", &CheckersApi::is_king, py::arg("x"), py::arg("y"))
        .def("leagal_moves", &CheckersApi::possible_moves, py::arg("x"), py::arg("y"))
        .def("move", &CheckersApi::move, py::arg("source_x"), py::arg("source_y"), py::arg("dest_x"), py::arg("dest_y"))
        .def("play", &CheckersApi::play)
        .def("hint", &CheckersApi::hint)
        .def("best_move", &CheckersApi::best_move)
        .def("__len__", [](CheckersApi& self) { return NUM_ROWS; })
        .def("__str__",
            [](CheckersApi& self) {
                std::stringstream s;
                s << self;
                return s.str();
            })
        .def_property_readonly("black_move", &CheckersApi::get_black_turn)
                .def_property_readonly("white_move", [](CheckersApi& self) { return !self.get_black_turn(); })
                .def_property_readonly("game_over", &CheckersApi::game_over)
                .def_property_readonly("draw", &CheckersApi::game_drawn)
                .def("__getitem__",
                    [](CheckersApi& self, py::tuple values) {
                        auto [x, y] = values.cast<std::tuple<int, int>>();
                        return self.get(x, y);
                    },
                py::arg("index"))
                ;
}
