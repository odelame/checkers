#ifndef _HELPFUNCS_HPP_
#define _HELPFUNCS_HPP_

#include <vector>
#include <tuple>
#include "consts.hpp"
#include <functional>
#include <iostream>
#include "bitboard.hpp"
#include <bitset>
#include <algorithm>
#include <execution>

std::vector<std::pair<int, int>> get_candidate_locations(const int x, const int y);
bool is_opposite_color(Piece source, Piece other);
void iter_on_board(std::function<void(int, int)> operation);
std::ostream& operator<<(std::ostream& strm, std::vector<BitBoard>& list);
std::pair<int, int> get_end_capture_pos(const int source_x, const int source_y, const int capture_x, const int capture_y);
int get_end_capture_pos(const int source, const int capture);
int get_board_index(const int x, const int y);
std::pair<int, int> board_index_to_xy(const int index);

template<unsigned long N>
short bit_count(std::bitset<N> bits) {
    short count = 0;

    for (unsigned int i = 0; i < N; i++) {
        if (bits[i])
            count++;
    }

    return count;
}

template<unsigned long N>
unsigned int get_bit_num(std::bitset<N> bits) {
    unsigned int position = 0;
    while (position < N && !bits[position])
        position++;
    return position;
}

#endif // _HELPFUNCS_HPP_
