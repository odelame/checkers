#ifndef _ENGINE_HPP_
#define _ENGINE_HPP_

#include "bitboard.hpp"
#include "helpFuncs.hpp"
#include "consts.hpp"
#include <tuple>
#include <list>
#include <climits>
#include <boost/range/combine.hpp>
#include <execution>
#include <mutex>

struct TreeNode {
    const BitBoard board;
    const bool black_turn;
    TreeNode* father;
    short eval;
    std::vector<TreeNode> children;
    TreeNode(const BitBoard state, const bool black_turn = true, TreeNode* father = nullptr, short eval = 0);
    short get_eval();
    BitBoard get_board();
};

namespace eval {
    short evaluate(BitBoard board);
    std::pair<BitBoard, short> best_move(BitBoard board, bool black_turn, const unsigned int depth = 6);
    std::vector<BitBoard> reachable(BitBoard board, const bool black_turn);
    std::pair<BitBoard, short> evaluate_tree(TreeNode* head, const unsigned int tree_depth);
}

#endif // _ENGINE_HPP_
