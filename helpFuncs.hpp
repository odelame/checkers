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
void iter_on_board(std::function<void(int, int, bool&)> operation);

std::ostream& operator<<(std::ostream& strm, std::vector<BitBoard>& list);
std::pair<int, int> get_end_capture_pos(const int source_x, const int source_y, const int capture_x, const int capture_y);
int get_end_capture_pos(const int source, const int capture);
int get_board_index(const int x, const int y);
std::pair<int, int> board_index_to_xy(const int index);

#endif // _HELPFUNCS_HPP_
