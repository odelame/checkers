#ifndef _CONSTS_HPP_
#define _CONSTS_HPP_

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

enum Direction {
    UP = -1,
    DOWN = 1
};

#endif // _CONSTS_HPP_
