#include "engine.hpp"

#define std ::std

TreeNode::TreeNode(const BitBoard state, const bool black_turn, TreeNode* father, short eval) :
    board{ state }, black_turn{ black_turn }, father{ father }, eval{ eval } {}

void TreeNode::expand() {
    this->children = engine::expand(this);
}

short TreeNode::evaluate() {
    return (this->eval = engine::evaluate(this->board));
}

namespace engine {
    /*short evaluate(BitBoard board) {
        short eval = 2 * board.num_white() + board.num_white_kings() - 2 * board.num_black() - board.num_black_kings();
        eval <<= 5;

        // center is worth more because it can impact more
        for (int x = NUM_COLS / 4; x <= NUM_COLS - NUM_COLS / 4; x++) {
            for (int y = (x & 1) + NUM_ROWS / 4 + 1; y < NUM_ROWS - NUM_ROWS / 4; y += 2) {
                eval += (board.get(x, y) == Piece::WHITE) + (board.get(x, y) == Piece::WHITE_KING)
                    - (board.get(x, y) == Piece::BLACK) - (board.get(x, y) == Piece::BLACK_KING);
            }
        }

        eval <<= 1;

        // Sides of the board are worth more because cannot be taken
        for (int y = 1; y < NUM_ROWS; y += 2) {
            eval += (board.get(0, y) == Piece::WHITE) + (board.get(0, y) == Piece::WHITE_KING)
                - (board.get(0, y) == Piece::BLACK) - (board.get(0, y) == Piece::BLACK_KING);
            eval += (board.get(NUM_COLS - 1, y) == Piece::WHITE) + (board.get(NUM_COLS - 1, y) == Piece::WHITE_KING)
                - (board.get(NUM_COLS - 1, y) == Piece::BLACK) - (board.get(NUM_COLS - 1, y) == Piece::BLACK_KING);
        }

        return eval;
    }*/
    short evaluate(BitBoard board) {
        short eval = 2 * board.num_white() + board.num_white_kings() - 2 * board.num_black() - board.num_black_kings();
        return eval;
    }

    std::vector<BitBoard> reachable(BitBoard board, const bool black_turn) {
        std::vector<BitBoard> result;

        iter_on_board([&result, &board, black_turn](const int x, const int y) {
            std::vector<BitBoard> reached_jumps = { board };
            std::vector<std::pair<int, int>> jump_coords = { {x, y} };
            while (jump_coords.size()) {
                std::vector<BitBoard> new_reached_jumps;
                std::vector<std::pair<int, int>> new_coords_jumps;

                for (auto [jump_coords, current_position] : boost::combine(jump_coords, reached_jumps)) {
                    auto [source_x, source_y] = jump_coords;
                    auto candidates = get_candidate_locations(source_x, source_y);

                    bool captured = false;
                    for (auto candidate : candidates) {
                        auto [capture_x, capture_y] = candidate;

                        if (current_position.leagal_capture(black_turn, source_x, source_y, capture_x, capture_y)) {
                            new_reached_jumps.push_back(current_position.capture(source_x, source_y, capture_x, capture_y));
                            new_coords_jumps.push_back(get_end_capture_pos(source_x, source_y, capture_x, capture_y));
                            captured = true;
                        }
                    }

                    if (!captured && current_position != board)
                        result.push_back(current_position);

                }

                reached_jumps = std::move(new_reached_jumps);
                jump_coords = std::move(new_coords_jumps);

            }
            });

        if (result.size() != 0)
            return result;

        iter_on_board([&result, &board, black_turn](const int x, const int y) {
            auto new_moves = board.moves(black_turn, x, y);
            result.reserve(result.size() + new_moves.size());
            result.insert(result.end(), new_moves.begin(), new_moves.end());
            });

        return result;
    }

    std::vector<TreeNode> expand(TreeNode* root) {
        std::vector<BitBoard> possibilities = reachable(root->board, root->black_turn);
        std::vector<TreeNode> result;

        for (auto& position : possibilities)
            result.emplace_back(position, !root->black_turn, root);

        return result;
    }

    short expand_tree_ab(TreeNode& root, const unsigned int depth, short alpha, short beta) {
        if (depth == 0) {
            root.evaluate();
            return root.eval;
        }

        root.expand();

        bool minimizing = root.black_turn;
        short eval;

        if (minimizing) {
            eval = SHRT_MAX;
            if (root.children.size() == 0) {
                root.eval = eval;
                return eval;
            }

            for (auto& child : root.children) {
                eval = std::min(eval, expand_tree_ab(child, depth - 1, alpha, beta));
                if (eval <= alpha)
                    break;

                beta = std::min(beta, eval);
            }
        }
        else {
            eval = SHRT_MIN;
            if (root.children.size() == 0) {
                root.eval = eval;
                return eval;
            }

            for (auto& child : root.children) {
                eval = std::max(eval, expand_tree_ab(child, depth - 1, alpha, beta));

                if (eval >= beta)
                    break;

                alpha = std::max(alpha, eval);
            }
        }

        root.eval = eval;
        return eval;
    }

    short alpha_beta_analysis(TreeNode* root, const unsigned int depth) {
        std::vector<unsigned int> indexes(depth + 1, 0);
        std::vector<std::pair<short, short>> level_ab(depth + 1, std::pair(SHRT_MIN, SHRT_MAX));
        bool minimize = root->black_turn;
        TreeNode* node = root;
        unsigned int level = 0;

        do {
            if (level == depth) {
                node->evaluate();

                // father is maximize
                if (minimize) {
                    node->father->eval = std::max(node->father->eval, node->eval);
                    short& alpha = std::get<0>(level_ab[level - 1]);
                    alpha = std::max(alpha, node->eval);
                }
                // father is minimize
                else {
                    node->father->eval = std::min(node->father->eval, node->eval);
                    short& beta = std::get<1>(level_ab[level - 1]);
                    beta = std::min(beta, node->eval);
                }

                //  indexes[level] = 0;
                level--;
                indexes[level]++;
                minimize = !minimize;
                node = node->father;
            }
            // if first time in this node:
            else if (indexes[level] == 0) {
                node->expand();
                if (minimize)
                    node->eval = SHRT_MAX;
                else
                    node->eval = SHRT_MIN;

                if (node->children.size() == 0) {
                    // father is maximize, this node is winning for father
                    if (minimize)
                        node->father->eval = SHRT_MAX;
                    // father is minimize, this node is winning for father
                    else
                        node->father->eval = SHRT_MIN;

                    level--;
                    node = node->father;
                    // cut of all other father children, this child is winning so there is no need to analyse others.
                    indexes[level] = node->children.size();
                    minimize = !minimize;
                }
                else {
                    node = &node->children.front();
                    minimize = !minimize;
                    level++;
                    level_ab[level] = level_ab[level - 1];
                    indexes[level] = 0;
                }
            }
            // if we checked all this nodes children:
            else if (indexes[level] >= node->children.size()) {
                // father is maximize
                if (minimize) {
                    node->father->eval = std::max(node->father->eval, node->eval);
                    short& alpha = std::get<0>(level_ab[level - 1]);
                    alpha = std::max(alpha, node->eval);
                }
                else {
                    node->father->eval = std::min(node->father->eval, node->eval);
                    short& beta = std::get<1>(level_ab[level - 1]);
                    beta = std::min(beta, node->eval);
                }

                level--;
                node = node->father;
                indexes[level]++;
                minimize = !minimize;
            }
            else {
                if (minimize && node->eval <= level_ab[level].first) {
                    indexes[level] = node->children.size();
                    continue;
                }
                if (!minimize && node->eval >= level_ab[level].second) {
                    indexes[level] = node->children.size();
                    continue;
                }

                node = &node->children[indexes[level]];
                minimize = !minimize;
                level++;
                level_ab[level] = level_ab[level - 1];
                indexes[level] = 0;
            }
        } while (indexes[0] < root->children.size());

        return root->eval;
    }

    std::stringstream& operator<<(std::stringstream& stream, std::vector<TreeNode>& nodes) {
        auto it = nodes.begin();
        if (it == nodes.end())
            return stream;

        stream << '[';
        for (; it != nodes.end().operator--(); it++)
            stream << it->eval << ", ";
        stream << it->eval;

        stream << ']';
        return stream;
    }
    std::ostream& operator<<(std::ostream& stream, std::vector<TreeNode>& nodes) {
        std::stringstream s;
        s << nodes;
        return stream << s.str();
    }

    std::string string_tree(TreeNode* root, const unsigned int depth) {
        std::stringstream s;
        s << '[' << root->eval << ']' << std::endl;
        std::vector<std::vector<TreeNode>*> edges = { &root->children };
        for (unsigned int i = 0; i < depth; i++) {
            std::vector<std::vector<TreeNode>*> new_edges;

            for (auto& level : edges) {
                s << *level << " ";

                for (auto& node : *level)
                    new_edges.push_back(&node.children);
            }
            s << std::endl;
            edges = std::move(new_edges);
        }

        return s.str();
    }

    std::pair<BitBoard, short> best_move(BitBoard board, bool black_turn, const unsigned int depth) {
        TreeNode root(board, black_turn);
        TreeNode root2(board, black_turn);

        expand_tree_ab(root2, depth);
        short evaluation = alpha_beta_analysis(&root, depth);

        std::string s1 = string_tree(&root, depth);
        std::string s2 = string_tree(&root2, depth);

        if (s1 != s2) {
            std::cout << s1 << std::endl;
            std::cout << s2 << std::endl;
        }
        std::cout << evaluation << std::endl;

        for (auto& child : root.children) {
            if (child.eval == evaluation)
                return std::pair<BitBoard, short>(child.board, evaluation);
        }

        // unreachable
        return std::pair(board, 0);
    }
}
