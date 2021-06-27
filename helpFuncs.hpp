#ifndef _HELPFUNCS_HPP_
#define _HELPFUNCS_HPP_

#include <vector>
#include <tuple>
#include "consts.hpp"

std::vector<std::pair<int, int>> get_candidate_locations(const int x, const int y);
bool is_opposite_color(Piece source, Piece other);

#endif // _HELPFUNCS_HPP_
