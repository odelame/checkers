#ifndef _GAMEAPI_HPP_
#define _GAMEAPI_HPP_

#include "bitboard.hpp"
#include <tuple>
#include <vector>
#include <list>
#include "consts.hpp"
#include <algorithm>
#include <pybind11/stl.h>
#include <pybind11/pybind11.h>
#include "helpFuncs.hpp"
#include "engine.hpp"

namespace py = pybind11;

class CheckersApi {
private:
    BitBoard board;
    bool black_turn;
    void switch_turn();
    std::vector<BitBoard> all_moves;
public:
    CheckersApi(BitBoard board = BitBoard(), bool black_turn = true);
    py::object move(const int source_x, const int source_y, const int dest_x, const int dest_y);
    Piece get(const int x, const int y) const;
    bool get_black_turn() const;
    bool is_black(const int x, const int y) const;
    bool is_white(const int x, const int y) const;
    bool is_king(const int x, const int y) const;
    bool game_over() const;
    bool captures_available() const;
    std::vector<std::pair<int, int>> possible_moves(const int x, const int y) const;
    void play();
};


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
        .def(py::init<>())
        .def("is_white", &CheckersApi::is_white, py::arg("x"), py::arg("y"))
        .def("is_black", &CheckersApi::is_black, py::arg("x"), py::arg("y"))
        .def("is_king", &CheckersApi::is_king, py::arg("x"), py::arg("y"))
        .def("moves", &CheckersApi::possible_moves, py::arg("x"), py::arg("y"))
        .def("move", &CheckersApi::move, py::arg("source_x"), py::arg("source_y"), py::arg("dest_x"), py::arg("dest_y"))
        .def("play", &CheckersApi::play)
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



#endif // _GAMEAPI_HPP_
