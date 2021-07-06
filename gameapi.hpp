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
    const unsigned int depth;
    BitBoard board;
    bool black_turn;
    bool draw;
    void switch_turn();
    std::vector<BitBoard> all_moves;
    Engine engine;
public:
    CheckersApi(const unsigned int depth = 6, BitBoard board = BitBoard(), bool black_turn = true);
    py::object move(const int source_x, const int source_y, const int dest_x, const int dest_y);
    Piece get(const int x, const int y) const;
    bool get_black_turn() const;
    bool is_black(const int x, const int y) const;
    bool is_white(const int x, const int y) const;
    bool is_king(const int x, const int y) const;
    bool game_over() const;
    bool game_drawn() const;
    bool captures_available() const;
    std::vector<std::pair<int, int>> possible_moves(const int x, const int y) const;
    void play();
    std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> best_move();
    std::pair<int, int> hint();
};


#endif // _GAMEAPI_HPP_
