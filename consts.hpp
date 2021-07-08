#ifndef _CONSTS_HPP_
#define _CONSTS_HPP_

#include <vector>
#include <tuple>

constexpr unsigned int NUM_ROWS = 8;
constexpr unsigned int NUM_COLS = NUM_ROWS;
constexpr unsigned int NUMBER_OF_REACHABLE_SQUARES = (NUM_ROWS * NUM_COLS) / 2;
constexpr unsigned int STARTING_ROWS = 3;
// Number of move repetitions for a draw
constexpr unsigned int REPETITION_DRAW = 3;
// Number of non capture move to force a draw
constexpr unsigned int NO_CAPTURE_DRAW = 100;

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
