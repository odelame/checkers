#ifndef _BITBOARD_HPP_
#define _BITBOARD_HPP_

#include <iostream>
#include <bitset>
#include <iomanip>
#include "consts.hpp"
#include "helpFuncs.hpp"
#include <tuple>
#include <vector>

class BitBoard {
private:
    // 4293918720ULL => the number that in the bitboard represents all the starting black squares.
    std::bitset<NUMBER_OF_REACHABLE_SQUARES> black_is_in = { 4293918720ULL };
    // 4095ULL => the number that in the bitboard represents all the starting white squares.
    std::bitset<NUMBER_OF_REACHABLE_SQUARES> white_is_in = { 4095ULL };
    std::bitset<NUMBER_OF_REACHABLE_SQUARES> kings = { 0ULL };

    void set_king(const int x, const int y);
    void set(const int x, const int y, const Piece value);

public:
    BitBoard();
    BitBoard(const BitBoard& other);

    Piece get(const int x, const int y) const;

    bool is_king(const int x, const int y) const;
    bool is_black(const int x, const int y) const;
    bool is_white(const int x, const int y) const;

    BitBoard move(const int source_x, const int source_y, const int dest_x, const int dest_y) const;
    BitBoard capture(const int source_x, const int source_y, const int dest_x, const int dest_y) const;

    bool leagal_capture(bool black_turn, const int source_x, const int source_y, const int capture_x, const int capture_y) const;
    bool leagal_move(const bool black_turn, const int source_x, const int source_y, const int dest_x, const int dest_y) const;

    std::vector<BitBoard> moves(const bool black_turn, const int x, const int y) const;
    std::vector<BitBoard> captures(const bool black_turn, const int x, const int y) const;

    bool operator==(const BitBoard& other) const;
    friend std::ostream& operator<<(std::ostream& strm, const BitBoard& board);
};

int get_index(const int x, const int y);
std::pair<int, int> get_xy(const int index);

std::ostream& operator<<(std::ostream& strm, const BitBoard& board);
std::ostream& operator<<(std::ostream& strm, Piece piece);

#endif // _BITBOARD_HPP_
