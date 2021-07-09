#ifndef GAMEAPI_HPP
#define GAMEAPI_HPP

#include "bitboard.hpp"
#include <tuple>
#include <vector>
#include <list>
#include "consts.hpp"
#include <algorithm>
#include <pybind11/stl.h>
#include <sstream>
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
    std::vector<BitBoard> all_moves;
    Engine engine;
    void switch_turn();
    void set_board(BitBoard new_board);
public:
    CheckersApi(const unsigned int depth = 6, BitBoard board = BitBoard(), bool black_turn = true);
    py::object move(const unsigned int source_x, const unsigned int source_y, const unsigned int dest_x, const unsigned int dest_y);
    Piece get(const unsigned int x, const unsigned int y) const;
    bool get_black_turn() const;
    bool is_black(const unsigned int x, const unsigned int y) const;
    bool is_white(const unsigned int x, const unsigned int y) const;
    bool is_king(const unsigned int x, const unsigned int y) const;
    bool game_over() const;
    bool game_drawn() const;
    bool captures_available() const;
    std::vector<std::pair<unsigned int, unsigned int>> possible_moves(const unsigned int x, const unsigned int y) const;
    short play();
    std::pair<std::vector<std::pair<std::pair<unsigned int, unsigned int>, std::pair<unsigned int, unsigned int>>>, short> best_move();
    std::pair<unsigned int, unsigned int> hint();
    friend std::stringstream& operator<<(std::stringstream& strm, CheckersApi& api);
};

std::stringstream& operator<<(std::stringstream& strm, CheckersApi& api);

#endif // GAMEAPI_HPP
