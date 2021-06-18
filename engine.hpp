#ifndef _ENGINE_HPP_
#define _ENGINE_HPP_

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <iostream>
#include <bitset>

constexpr int NUM_ROWS = 8;
constexpr int NUM_COLS = 8;
constexpr int NUMBER_OF_REACHABLE_SQUARES = (NUM_ROWS * NUM_COLS) / 2;
constexpr int STARTING_ROWS = 3;

enum class Piece {
    NONE = 0,
    BLACK = 1,
    WHITE = 2,
    BLACK_KING = 3,
    WHITE_KING = 4
};

class BitBoard {
private:
    std::bitset<NUMBER_OF_REACHABLE_SQUARES> black_is_in = { false };
    std::bitset<NUMBER_OF_REACHABLE_SQUARES> white_is_in = { true };
    std::bitset<NUMBER_OF_REACHABLE_SQUARES> kings = { false };
    class ProxyIndexing {
    private:
        const int x;
        const BitBoard* board;
    public:
        ProxyIndexing(const BitBoard* board, const int x);
        Piece operator[](const int y) const;
    };

public:
    BitBoard();
    ProxyIndexing operator[](const int x) const;


    friend std::ostream& operator<<(std::ostream& strm, const BitBoard& board);
};
std::ostream& operator<<(std::ostream& strm, const BitBoard& board);

#endif // _ENGINE_HPP_
