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
    TreeNode* father;
    short eval;
    std::vector<TreeNode> children = {};
    TreeNode(const BitBoard state, TreeNode* father = nullptr, short eval = 0);
    TreeNode(const TreeNode& other);
    void expand(bool black_turn);
    short evaluate();
    //TreeNode& operator=(const TreeNode& other);
};

std::ostream& operator<<(std::ostream& strm, TreeNode& root);
namespace engine {
    std::pair<BitBoard, short> best_move(BitBoard board, bool black_turn, const unsigned int depth = 6);
    short alpha_beta_analysis(TreeNode* root, bool black_turn, const unsigned int depth = 6);
}

#endif // _ENGINE_HPP_
