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
    const unsigned int depth;
public:
    CheckersApi(const unsigned int depth = 6, BitBoard board = BitBoard(), bool black_turn = true);
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


#endif // _GAMEAPI_HPP_
