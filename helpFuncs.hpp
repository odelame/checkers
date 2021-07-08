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

std::vector<std::pair<unsigned int, unsigned int>> get_candidate_locations(const unsigned int x, const unsigned int y);
bool is_opposite_color(Piece source, Piece other);
std::ostream& operator<<(std::ostream& strm, std::vector<BitBoard>& list);
std::pair<unsigned int, unsigned int> get_end_capture_pos(const unsigned int source_x, const unsigned int source_y, const unsigned int capture_x, const unsigned int capture_y);
unsigned int get_end_capture_pos(const unsigned int source, const unsigned int capture);

#endif // _HELPFUNCS_HPP_
