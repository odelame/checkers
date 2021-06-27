#ifndef _GAMEAPI_HPP_
#define _GAMEAPI_HPP_

#include "bitboard.hpp"
#include <tuple>
#include <vector>
#include <list>
#include "consts.hpp"
#include <algorithm>
#include <pybind11/stl.h>
#include "helpFuncs.hpp"

namespace py = pybind11;

class CheckersApi {
private:
    BitBoard board;
    bool black_turn;
    bool captures_available;
    void switch_turn();
    std::vector<BitBoard> get_moves();
    std::vector<BitBoard> all_moves;
public:
    CheckersApi(BitBoard board = BitBoard(), bool black_turn = true);
    py::object move(const int source_x, const int source_y, const int dest_x, const int dest_y);
    Piece get(const int x, const int y) const;
    bool get_black_turn() const;
    bool is_black(const int x, const int y) const;
    bool is_white(const int x, const int y) const;
    std::vector<std::pair<int, int>> possible_moves(const int x, const int y) const;
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
        .def("__getitem__", [](CheckersApi& self, py::tuple values) {
                std::tuple<int, int> indexes = values.cast<std::tuple<int, int>>();
                return self.get(std::get<0>(indexes), std::get<1>(indexes));
            }, py::arg("tuple_index"))
        .def_property_readonly("black_move", [](CheckersApi& self) {
                return self.get_black_turn();
            })
        .def("is_white", [](CheckersApi& self, py::tuple values) {
                std::tuple<int, int> indexes = values.cast<std::tuple<int, int>>();
                return self.is_white(std::get<0>(indexes), std::get<1>(indexes));
            }, py::arg("index_tuple"))
        .def("is_black",  [](CheckersApi& self, py::tuple values) {
                std::tuple<int, int> indexes = values.cast<std::tuple<int, int>>();
                return self.is_black(std::get<0>(indexes), std::get<1>(indexes));
            }, py::arg("index_tuple"))
        .def("moves", [](CheckersApi& self, py::tuple values) {
                std::tuple<int, int> indexes = values.cast<std::tuple<int, int>>();
                return self.possible_moves(std::get<0>(indexes), std::get<1>(indexes));
            }, py::arg("index_tuple"))
        .def("move", &CheckersApi::move, py::arg("source_x"), py::arg("source_y"), py::arg("dest_x"), py::arg("dest_y"))
        ;

}



#endif // _GAMEAPI_HPP_
