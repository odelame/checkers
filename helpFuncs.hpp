#ifndef _HELPFUNCS_HPP_
#define _HELPFUNCS_HPP_

#include <vector>
#include <tuple>
#include "consts.hpp"
#include <functional>
#include <iostream>
#include "bitboard.hpp"
#include <bitset>

std::vector<std::pair<int, int>> get_candidate_locations(const int x, const int y);
bool is_opposite_color(Piece source, Piece other);
void iter_on_board(std::function<void(int, int)> operation);
std::ostream& operator<<(std::ostream& strm, std::vector<BitBoard>& list);
std::pair<int, int> get_end_capture_pos(const int source_x, const int source_y, const int capture_x, const int capture_y);
int get_end_capture_pos(const int source, const int capture);

template<unsigned long N>
short bit_count(std::bitset<N> bits) {
    short count = 0;

    for (unsigned int i = 1; i <= N; i++) {
        if (bits[i])
            count++;
    }

    return count;
}

#endif // _HELPFUNCS_HPP_
