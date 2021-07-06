#include "engine.hpp"

TreeNode::TreeNode(const BitBoard state, TreeNode* father, short eval) :
    board{ state }, father{ father }, eval{ eval } {}

TreeNode::TreeNode(const TreeNode& other) : TreeNode(other.board, other.father, other.eval) {}

std::size_t hash_position::operator()(const Position& position) const {
    std::size_t seed = 0;
    boost::hash_combine(seed, BitBoard::hasher()(position.first));
    boost::hash_combine(seed, position.second);
    return seed;
}

void TreeNode::expand(bool black_turn) {
    this->children.clear();

    iter_on_board([this, black_turn](const int x, const int y) {
        std::vector<BitBoard> reached_jumps = { this->board };
        std::vector<std::pair<int, int>> jump_coords = { {x, y} };

        while (jump_coords.size() != 0) {
            std::vector<BitBoard> new_reached_jumps;
            std::vector<std::pair<int, int>> new_coords_jumps;

            for (auto [jump_coords, current_position] : boost::combine(jump_coords, reached_jumps)) {
                auto [source_x, source_y] = jump_coords;
                auto candidates = get_candidate_locations(source_x, source_y);

                bool captured = false;
                for (auto [capture_x, capture_y] : candidates) {
                    if (current_position.leagal_capture(black_turn, source_x, source_y, capture_x, capture_y)) {
                        new_reached_jumps.push_back(current_position.capture(source_x, source_y, capture_x, capture_y));
                        new_coords_jumps.push_back(get_end_capture_pos(source_x, source_y, capture_x, capture_y));
                        captured = true;
                    }
                }

                if (!captured && current_position != this->board)
                    this->children.emplace_back(current_position, this);
            }

            reached_jumps = std::move(new_reached_jumps);
            jump_coords = std::move(new_coords_jumps);

        }
        });

    if (this->children.size() != 0)
        return;

    iter_on_board([this, black_turn](const int x, const int y) {
        auto new_moves = this->board.moves(black_turn, x, y);
        this->children.reserve(this->children.size() + new_moves.size());

        for (auto& new_move : new_moves) {
            this->children.emplace_back(new_move, this);
        }
        });
}

short TreeNode::evaluate() {
    this->eval = 2 * board.num_white() + board.num_white_kings() - 2 * board.num_black() - board.num_black_kings();
    this->eval <<= 4;

    // center is worth more because it can impact more
    for (int x = NUM_COLS / 4; x <= NUM_COLS - NUM_COLS / 4; x++) {
        for (int y = (x & 1) + NUM_ROWS / 4 + 1; y < NUM_ROWS - NUM_ROWS / 4; y += 2) {
            this->eval += (board.get(x, y) == Piece::WHITE) + (board.get(x, y) == Piece::WHITE_KING)
                - (board.get(x, y) == Piece::BLACK) - (board.get(x, y) == Piece::BLACK_KING);
        }
    }

    this->eval <<= 1;

    // Sides of the board are worth more because cannot be taken
    for (int y = 1; y < NUM_ROWS; y += 2) {
        this->eval += board.is_white(0, y) - board.is_black(0, y);
        this->eval += board.is_white(NUM_COLS - 1, y - 1) - board.is_black(NUM_COLS - 1, y - 1);

        this->eval += board.is_white(y, 0) - board.is_black(y, 0);
        this->eval += board.is_white(y - 1, NUM_ROWS - 1) - board.is_black(y - 1, NUM_ROWS - 1);
    }

    return this->eval;
}

std::ostream& operator<<(std::ostream& stream, std::vector<TreeNode>& nodes) {
    auto it = nodes.begin();

    stream << '[';
    for (; it != nodes.end().operator--(); it++)
        stream << it->eval << ", ";
    stream << it->eval;
    stream << ']';

    return stream;
}

std::ostream& operator<<(std::ostream& strm, TreeNode& root) {
    strm << '[' << root.eval << ']' << std::endl;
    std::vector<std::vector<TreeNode>*> edges = { &root.children };
    while (edges.size() != 0) {
        std::vector<std::vector<TreeNode>*> new_edges;

        for (auto& level : edges) {
            strm << *level << " ";

            for (auto& node : *level) {
                if (node.children.size() != 0)
                    new_edges.push_back(&node.children);
            }
        }
        strm << std::endl;
        edges = std::move(new_edges);
    }

    return strm;
}

Engine::Engine() : position_history(std::unordered_map<const Position, unsigned int, hash_position>()) {}

unsigned int Engine::increment_get_position_counter(const Position& position) {
    return ++this->position_history[position];
}

short Engine::alpha_beta_analysis(TreeNode* root, bool black_turn, std::mutex& m, const unsigned int depth) {
    std::vector<unsigned int> indexes(depth + 1, 0);
    std::vector<std::pair<short, short>> level_ab(depth + 1, std::pair(SHRT_MIN, SHRT_MAX));
    bool minimize = black_turn;
    TreeNode* node = root;
    unsigned int level = 0;

    do {
        m.lock();
        if (level != 0 && DRAW_REPETITION - 1 == this->position_history[Position(node->board, minimize)]) {
            m.unlock();
            node->eval = 0;
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

            indexes[level] = 0;
            level--;
            indexes[level]++;
            minimize = !minimize;
            node = node->father;
            continue;
        }
        m.unlock();
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
            node->expand(minimize);
            if (minimize)
                node->eval = SHRT_MAX;
            else
                node->eval = SHRT_MIN;

            if (node->children.size() == 0) {
                if (level == 0)
                    break;

                // father is maximize, this node is winning for father, -level to ensure progress across moves in winning
                if (minimize)
                    node->father->eval = SHRT_MAX;
                // father is minimize, this node is winning for father, +level to ensure progress across moves in winning
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

std::pair<BitBoard, short> Engine::best_move(BitBoard board, bool black_turn, const unsigned int depth) {
    Timer t(std::cout);
    TreeNode root(board);
    root.expand(black_turn);
    std::mutex lock;
    short eval;
    if (black_turn)
        eval = SHRT_MAX;
    else
        eval = SHRT_MIN;

    // optimize with parallel processing
    std::for_each(std::execution::par, root.children.begin(), root.children.end(), [this, &lock, &eval, black_turn, depth](TreeNode& root) {
        short temp_eval = this->alpha_beta_analysis(&root, !black_turn, lock, depth - 1);
        lock.lock();
        if (black_turn)
            eval = std::min(eval, temp_eval);
        else
            eval = std::max(eval, temp_eval);
        lock.unlock();
        });

    std::cout << eval << std::endl;
    for (auto& child : root.children) {
        if (child.eval == eval)
            return std::pair<BitBoard, short>(child.board, eval);
    }

    // unreachable
    return std::pair(board, 0);
}

