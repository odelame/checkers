#include "engine.hpp"

TreeNode::TreeNode(const BitBoard state, TreeNode* father, short eval) :
    board{ state }, father{ father }, eval{ eval }, children{ std::vector<TreeNode>() } {}

TreeNode::TreeNode(const TreeNode& other) : TreeNode(other.board, other.father, other.eval) {}

bool TreeNode::operator<(const TreeNode& other) const {
    return this->eval < other.eval;
}

TreeNode& TreeNode::operator=(TreeNode&& other) {
    this->children = std::move(other.children);
    this->eval = other.eval;
    this->father = other.father;
    other.eval = 0;
    other.father = nullptr;
    this->board = other.board;
    return *this;
}

TreeNode& TreeNode::operator=(const TreeNode& other) {
    this->children = other.children;
    this->eval = other.eval;
    this->father = other.father;
    this->board = other.board;
    return *this;
}

std::size_t hash_position::operator()(const Position& position) const {
    std::size_t seed = 0;
    boost::hash_combine(seed, BitBoard::hasher()(position.first));
    boost::hash_combine(seed, position.second);
    return seed;
}

/**
 * @brief sets node's children to all the possible next position from it assuming it's black's turn is black_turn.
 * returns true is there was a capture.
 *
 * @param black_turn
 * @return true
 * @return false
 */
bool TreeNode::expand(bool black_turn) {
    this->children.clear();
    // for each coordinate non-empty on the board
    for (auto [x, y] : this->board) {
        // if the piece is of the wrong color: skip it.
        if (black_turn && !this->board.is_black(x, y))
            continue;
        if (!black_turn && !this->board.is_white(x, y))
            continue;

        // reached_jumps: boards we have just reached.
        std::vector<BitBoard> reached_jumps = { this->board };
        // will keep track in correspondence to reaches_jumps of the last coordinate we jumped to.
        std::vector<std::pair<unsigned int, unsigned int>> jump_coords = { {x, y} };

        // while we jumped somewhere new:
        while (jump_coords.size() != 0) {
            // next iterations of previously explained vectors.
            std::vector<BitBoard> new_reached_jumps;
            std::vector<std::pair<unsigned int, unsigned int>> new_coords_jumps;

            // for each last coordinate and position:
            for (auto [jump_coords, current_position] : boost::combine(jump_coords, reached_jumps)) {
                auto [source_x, source_y] = jump_coords;
                auto candidates = get_candidate_locations(source_x, source_y);

                // try ro jump from it, add new jumps to new_reaches_jumps and new_coords_jumps.
                bool captured = false;
                for (auto [capture_x, capture_y] : candidates) {
                    if (current_position.legal_capture(black_turn, source_x, source_y, capture_x, capture_y)) {
                        new_reached_jumps.push_back(current_position.capture(source_x, source_y, capture_x, capture_y));
                        new_coords_jumps.push_back(get_end_capture_pos(source_x, source_y, capture_x, capture_y));
                        captured = true;
                    }
                }

                // if no new reached jumps <=> captured == false which means add this to the children of the node.
                if (!captured && current_position != this->board)
                    this->children.emplace_back(current_position, this);
            }

            reached_jumps = std::move(new_reached_jumps);
            jump_coords = std::move(new_coords_jumps);
        }
    }

    // if found jumps
    if (this->children.size() != 0)
        return true;

    // look for regular moves.
    for (auto [x, y] : this->board) {
        auto new_moves = this->board.moves(black_turn, x, y);
        this->children.reserve(this->children.size() + new_moves.size());

        for (auto& new_move : new_moves) {
            this->children.emplace_back(new_move, this);
        }
    }

    return false;
}

/**
 * @brief evaluate a TreeNode without looking at any depth of the position.
 *
 * @return short
 */
short TreeNode::evaluate() {
    this->eval = 2 * board.num_white() + board.num_white_kings() - 2 * board.num_black() - board.num_black_kings();
    this->eval <<= 4;

    // center is worth more because it can impact more
    for (unsigned int x = NUM_COLS / 4; x <= NUM_COLS - NUM_COLS / 4; x++) {
        for (unsigned int y = (x & 1) + NUM_ROWS / 4 + 1; y < NUM_ROWS - NUM_ROWS / 4; y += 2) {
            this->eval += (board.get(x, y) == Piece::WHITE) + (board.get(x, y) == Piece::WHITE_KING)
                - (board.get(x, y) == Piece::BLACK) - (board.get(x, y) == Piece::BLACK_KING);
        }
    }

    this->eval <<= 1;

    // Sides of the board are worth more because cannot be taken
    for (unsigned int y = 1; y < NUM_ROWS; y += 2) {
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

Engine::Engine() : position_history(std::unordered_map<const Position, unsigned int, hash_position>()), since_capture(0) {}

unsigned int Engine::increment_position_history_counter(const Position& position) {
    return ++this->position_history[position];
}

void Engine::increment_since_capture() {
    this->since_capture++;
}

void Engine::reset_since_capture() {
    this->since_capture = 0;
}

unsigned int Engine::get_since_capture() {
    return this->since_capture;
}

/**
 * @brief steps down in tree of alpha-beta pruning algorithm and updates all the necessary help data
 *
 * @param node
 * @param minimize
 * @param appeared
 * @param indexes
 * @param level_ab
 * @param this_position
 * @param level
 */
void step_down(TreeNode*& node, bool& minimize, std::unordered_set<Position, hash_position>& appeared, std::vector<unsigned int>& indexes, std::vector<std::pair<short, short>>& level_ab, Position& this_position, unsigned int& level) {
    // from this point on, this position has happened, will be removed by step_up later on.
    appeared.insert(this_position);
    // step down.
    node = &node->children[indexes[level]];
    // update this position and the minimize
    minimize = !minimize;
    this_position = Position(node->board, minimize);
    // update the level in the tree
    level++;
    // when going down for the first time alpha-beta is simply the parent alpha-beta
    level_ab[level] = level_ab[level - 1];
    // this will ensure stepping down from this node works properly.
    indexes[level] = 0;
}

/**
 * @brief steps up in tree of alpha-beta pruning algorithm and updates all the necessary help data
 *
 * @param node
 * @param minimize
 * @param appeared
 * @param history
 * @param indexes
 * @param level_ab
 * @param level
 * @param this_position
 */
void step_up(TreeNode*& node, bool& minimize, std::unordered_set<Position, hash_position>& appeared, const std::unordered_map<const Position, unsigned int, hash_position>& history, std::vector<unsigned int>& indexes, std::vector<std::pair<short, short>>& level_ab, unsigned int& level, Position& this_position, int& action_level) {
    // father is maximize
    if (minimize) {
        node->father->eval = std::max(node->father->eval, node->eval);
        // if you do not understand this => look up alpha-beta pruning algorithm
        short& alpha = std::get<0>(level_ab[level - 1]);
        alpha = std::max(alpha, node->eval);
    }
    // father is minimize
    else {
        node->father->eval = std::min(node->father->eval, node->eval);
        // if you do not understand this => look up alpha-beta pruning algorithm
        short& beta = std::get<1>(level_ab[level - 1]);
        beta = std::min(beta, node->eval);
    }

    // step up
    node = node->father;
    if (action_level == (int)level)
        action_level = -1;
    level--;
    indexes[level]++;
    minimize = !minimize;
    // update this position
    this_position = Position(node->board, minimize);
    // if this position was not in history i.e. a position that happened on the board, it is no longer in the set of positions that have accored in this line.
    if (!history.contains(this_position))
        appeared.erase(this_position);
}

/**
 * @brief implementation of alpha-beta pruning algorithm to find the best move given a depth.
 *
 * @param root
 * @param black_turn
 * @param depth
 * @return short
 */
short Engine::alpha_beta_analysis(TreeNode* root, bool black_turn, const unsigned int depth) const {
    std::vector<unsigned int> indexes(depth + 1, 0);
    std::vector<std::pair<short, short>> level_ab(depth + 1, std::pair(SHRT_MIN, SHRT_MAX));
    bool minimize = black_turn;
    TreeNode* node = root;
    unsigned int level = 0;

    const unsigned int draw_no_action = NO_CAPTURE_DRAW - this->since_capture;
    int action_level = -1;

    std::unordered_set<Position, hash_position> appeared;
    Position this_position(root->board, minimize);

    do {
        // if this node already appeared: if there is a good/bad move from it we would have found it in the "parent" copy, 
        // if the best move causes is to loop to it again => it's a draw.
        // so we can always evaluate it as a draw.
        // another draw is by no action.
        if (appeared.contains(this_position) || (level >= draw_no_action && action_level == -1)) {
            node->eval = 0;

            if (level == 0)
                break;
            step_up(node, minimize, appeared, this->position_history, indexes, level_ab, level, this_position, action_level);
        }
        // last nodes in tree needs to be evaluated
        else if (level == depth) {
            node->evaluate();
            step_up(node, minimize, appeared, this->position_history, indexes, level_ab, level, this_position, action_level);
        }
        // if first time in this node:
        else if (indexes[level] == 0) {
            // default evaluation is worst, so we will change it from children for sure.
            if (minimize)
                node->eval = SHRT_MAX;
            else
                node->eval = SHRT_MIN;

            if (node->expand(minimize) && action_level == -1)
                action_level = level + 1;

            // if this node is infact a loss:
            if (node->children.size() == 0) {
                // this means we lost the game
                if (level == 0)
                    break;
                // move back up, father will be a winning move.
                step_up(node, minimize, appeared, this->position_history, indexes, level_ab, level, this_position, action_level);
            }
            else
                step_down(node, minimize, appeared, indexes, level_ab, this_position, level);
        }
        // if we checked all this nodes children, or rendered then unimportant due to alpha-beta pruning
        else if (indexes[level] >= node->children.size()) {
            // unreachable??
            if (level == 0)
                break;
            step_up(node, minimize, appeared, this->position_history, indexes, level_ab, level, this_position, action_level);
        }
        // default option: we need to move down.
        else {
            // alpha-beta cutoff, alpha cutoff
            if (minimize && node->eval <= level_ab[level].first) {
                indexes[level] = node->children.size();
                continue;
            }
            // alpha-beta cutoff, beta cutoff
            if (!minimize && node->eval >= level_ab[level].second) {
                indexes[level] = node->children.size();
                continue;
            }
            step_down(node, minimize, appeared, indexes, level_ab, this_position, level);
        }
    } while (indexes[0] < root->children.size());
    // while root was not fully evaluated.

    return root->eval;
}

/**
 * @brief get the best possible next position accourding to engine analysis and the engines evaluation of it.
 *
 * @param board
 * @param black_turn
 * @param depth
 * @return std::pair<BitBoard, short>
 */
std::pair<BitBoard, short> Engine::best_move(BitBoard board, bool black_turn, const unsigned int depth) const {
    TreeNode root(board);
    root.expand(black_turn);
    std::atomic<short> eval;

    if (black_turn)
        eval = SHRT_MAX;
    else
        eval = SHRT_MIN;

    // optimize the move-search with parallel processing
    std::for_each(std::execution::par, root.children.begin(), root.children.end(), [this, &eval, black_turn, depth](TreeNode& root) {
        short temp_eval = this->alpha_beta_analysis(&root, !black_turn, depth - 1);
        if (black_turn)
            eval = std::min(eval.load(), temp_eval);
        else
            eval = std::max(eval.load(), temp_eval);
    });

    for (auto& child : root.children) {
        if (child.eval == eval)
            return std::pair<BitBoard, short>(child.board, eval);
    }

    // unreachable
    return std::pair(board, 0);
}

