#ifndef _BITBOARD_HPP_
#define _BITBOARD_HPP_

#include <iostream>
#include <bitset>
#include <iomanip>

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
    // 4293918720ULL => the number that in the bitboard represents all the starting black squares.
    std::bitset<NUMBER_OF_REACHABLE_SQUARES> black_is_in = { 4293918720ULL };
    // 4095ULL => the number that in the bitboard represents all the starting white squares.
    std::bitset<NUMBER_OF_REACHABLE_SQUARES> white_is_in = { 4095ULL };
    std::bitset<NUMBER_OF_REACHABLE_SQUARES> kings = { 0ULL };

public:
    BitBoard();
    BitBoard(const BitBoard& other);

    void set(const int x, const int y, const Piece value);
    Piece get(const int x, const int y) const;

    bool is_king(const int x, const int y) const;
    bool is_black(const int x, const int y) const;
    bool is_white(const int x, const int y) const;

    BitBoard move(const int source_x, const int source_y, const int dest_x, const int dest_y);
    BitBoard capture(const int source_x, const int source_y, const int dest_x, const int dest_y);

    friend std::ostream& operator<<(std::ostream& strm, const BitBoard& board);
};

std::ostream& operator<<(std::ostream& strm, const BitBoard& board);
std::ostream& operator<<(std::ostream& strm, Piece piece);

#endif // _BITBOARD_HPP_
