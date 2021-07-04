#include "engine.hpp"

TreeNode::TreeNode(const BitBoard state, const bool black_turn, TreeNode* father, short eval) :
    board{ state }, black_turn{ black_turn }, father{ father }, eval{ eval } {}

namespace eval {
    short evaluate(BitBoard board) {
        return 2 * board.num_white() + board.num_white_kings() - 2 * board.num_black() - board.num_black_kings();
    }

    std::vector<BitBoard> reachable(BitBoard board, const bool black_turn) {
        std::vector<BitBoard> result;

        iter_on_board([&result, &board, black_turn](const int x, const int y) {
            std::vector<BitBoard> reached_jumps = { board };
            std::vector<std::pair<int, int>> jump_coords = { {x, y} };
            std::mutex lock;

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
        if (depth == 0)
            return evaluate(root.board);

        root.children = expand(&root);

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

    std::pair<BitBoard, short> best_move(BitBoard board, bool black_turn, const unsigned int depth) {
        Timer t(std::cout);
        TreeNode root(board, black_turn);
        short evaluation = expand_tree_ab(root, depth);

        for (auto& child : root.children) {
            if (child.eval == evaluation)
                return std::pair<BitBoard, short>(child.board, evaluation);
        }
        // unreachable
        return std::pair(board, 0);
    }
}

