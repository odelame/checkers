#ifndef _ENGINE_HPP_
#define _ENGINE_HPP_

#include "bitboard.hpp"
#include "helpFuncs.hpp"
#include "consts.hpp"
#include <tuple>
#include <list>
#include <climits>
#include <boost/range/combine.hpp>
#include <sstream>


struct TreeNode {
    const BitBoard board;
    const bool black_turn;
    TreeNode* father;
    short eval;
    std::vector<TreeNode> children = {};
    TreeNode(const BitBoard state, const bool black_turn = true, TreeNode* father = nullptr, short eval = 0);
    void expand();
    short evaluate();
};

namespace engine {
    short evaluate(BitBoard board);
    std::pair<BitBoard, short> best_move(BitBoard board, bool black_turn, const unsigned int depth = 4);
    std::vector<TreeNode> expand(TreeNode* root);
    std::vector<BitBoard> reachable(BitBoard board, const bool black_turn);
    short expand_tree_ab(TreeNode& root, const unsigned int depth, short alpha = SHRT_MIN, short beta = SHRT_MAX);
}

#endif // _ENGINE_HPP_
