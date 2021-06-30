#include "engine.hpp"

TreeNode::TreeNode(const BitBoard state, const bool black_turn, TreeNode* father, short eval) :
    board{ state }, black_turn{ black_turn }, father{ father }, eval{ eval } {}

namespace eval {
    short evaluate(BitBoard board) {
        return 2 * board.num_white() + board.num_white_kings() - 2 * board.num_black() - board.num_black_kings();
    }

    std::vector<BitBoard> reachable(BitBoard board, const bool black_turn) {
        std::vector<BitBoard> result;

        if (board.captures(black_turn).size() == 0)
            return board.moves(black_turn);

        iter_on_board([&result, &board, black_turn](const int x, const int y) {
            std::vector<BitBoard> reached = { board };
            std::vector<std::pair<int, int>> coords = { {x, y} };

            do {
                std::vector<BitBoard> new_reached;
                std::vector<std::pair<int, int>> new_coords;

                for (auto [coords, current_position] : boost::combine(coords, reached)) {
                    auto [source_x, source_y] = coords;
                    auto candidates = get_candidate_locations(source_x, source_y);

                    bool captured = false;
                    for (auto candidate : candidates) {
                        auto [capture_x, capture_y] = candidate;

                        if (current_position.leagal_capture(black_turn, source_x, source_y, capture_x, capture_y)) {
                            new_reached.push_back(current_position.capture(source_x, source_y, capture_x, capture_y));
                            new_coords.push_back(get_end_capture_pos(source_x, source_y, capture_x, capture_y));
                            captured = true;
                        }
                    }
                    if (!captured && current_position != board)
                        result.push_back(current_position);
                }

                reached = std::move(new_reached);
                coords = std::move(new_coords);

            } while (coords.size() != 0);
            });

        return result;
    }

    std::vector<TreeNode> expand(TreeNode* head) {
        std::vector<BitBoard> possibilities = reachable(head->board, head->black_turn);
        std::vector<TreeNode> result;

        for (auto& position : possibilities)
            result.emplace_back(position, !head->black_turn, head);

        return result;
    }

    std::pair<BitBoard, short> evaluate_tree(TreeNode* head, const unsigned int tree_depth) {
        if (tree_depth == 0)
            return std::make_pair(head->board, evaluate(head->board));

        if (head->children.size() == 0) {
            if (head->black_turn)
                return std::make_pair(head->board, SHRT_MAX);
            return std::make_pair(head->board, SHRT_MIN);
        }


        BitBoard min_board, max_board;
        short min_eval = SHRT_MAX, max_eval = SHRT_MIN;

        for (auto& child : head->children) {
            auto [temp_board, eval] = evaluate_tree(&child, tree_depth - 1);
            if (eval < min_eval) {
                min_eval = eval;
                min_board = child.board;
            }
            if (eval > max_eval) {
                max_eval = eval;
                max_board = child.board;
            }
        }

        if (head->black_turn)
            return std::make_pair(min_board, min_eval);
        return std::make_pair(max_board, max_eval);
    }

    std::pair<BitBoard, short> best_move(BitBoard board, bool black_turn) {
        TreeNode head(board, black_turn);
        std::vector<TreeNode*> edges(1, &head);
        constexpr unsigned int DEPTH = 1;

        for (unsigned int i = 0; i < DEPTH; i++) {
            std::vector<TreeNode*> new_edges;
            std::mutex m;

            std::for_each(std::execution::par, edges.begin(), edges.end(), [&new_edges, &m](TreeNode* node) {
                node->children = expand(node);

                if (node->black_turn)
                    node->eval = SHRT_MAX;
                else
                    node->eval = SHRT_MIN;

                m.lock();
                new_edges.reserve(new_edges.size() + node->children.size());
                for (auto& child : node->children)
                    new_edges.push_back(&child);
                m.unlock();
                });

            edges = new_edges;
        }


        return evaluate_tree(&head, DEPTH);
    }
}

