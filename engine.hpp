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
#include <mutex>
#include <execution>
#include <algorithm>
#include "timer.hpp"
#include <unordered_map>
#include <unordered_set>
#include <boost/functional/hash.hpp>

struct TreeNode {
    const BitBoard board;
    TreeNode* father;
    short eval;
    std::vector<TreeNode> children;
    TreeNode(const BitBoard state, TreeNode* father = nullptr, short eval = 0);
    TreeNode(const TreeNode& other);
    void expand(bool black_turn);
    short evaluate();
    //TreeNode& operator=(const TreeNode& other);
};

typedef std::pair<BitBoard, bool> Position;

struct hash_position {
    std::size_t operator()(const Position& position) const;
};

class Engine {
public:
    Engine();
    short alpha_beta_analysis(TreeNode* root, bool black_turn, const unsigned int depth = 6);
    std::pair<BitBoard, short> best_move(BitBoard board, bool black_turn, const unsigned int depth = 6);
    unsigned int increment_get_position_counter(const Position& position);
private:
    std::unordered_map<const Position, unsigned int, hash_position> position_history;
};

std::ostream& operator<<(std::ostream& strm, TreeNode& root);


#endif // _ENGINE_HPP_
