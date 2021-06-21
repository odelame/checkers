#include "gameapi.hpp"

CheckersApi::CheckersApi() : board(BitBoard()), black_turn(true), jumps_available(false) {}

bool is_opposite_color(const Piece source, const Piece other) {
    if (source == Piece::BLACK || source == Piece::BLACK_KING)
        return other == Piece::WHITE || other == Piece::WHITE_KING;

    return (source == Piece::WHITE || source == Piece::WHITE_KING) && (other == Piece::BLACK || other == Piece::BLACK_KING);
}

void CheckersApi::switch_turn() {
    this->black_turn = !this->black_turn;
}

bool CheckersApi::leagal_capture(const int source_x, const int source_y, const int capture_x, const int capture_y) {
    if (Piece::NONE != this->board.get(2 * capture_x - source_x, 2 * capture_y - source_y))
        return false;

    const Piece captured = this->board.get(capture_x, capture_y);
    const Piece source = this->board.get(source_x, source_y);

    if (!is_opposite_color(source, captured))
        return false;

    if (this->black_turn) {
        if (source == Piece::BLACK && capture_y - source_y == UP)
            return true;

        return source == Piece::BLACK_KING && std::abs(capture_y - source_y) == 1;
    }
    else {
        if (source == Piece::WHITE && capture_y - source_y == DOWN)
            return true;

        return source == Piece::WHITE_KING && std::abs(capture_y - source_y) == 1;
    }
}

bool CheckersApi::leagal_move(const int source_x, const int source_y, const int dest_x, const int dest_y) {
    if (this->leagal_capture(source_x, source_y, dest_y, dest_y))
        return true;

    if (this->jumps_available)
        return false;

    if (this->board.get(dest_x, dest_y) != Piece::NONE || std::abs(dest_x - source_x) != 1)
        return false;

    const Piece source = this->board.get(source_x, source_y);
    if (this->black_turn)
        return (source == Piece::BLACK && dest_y - source_y == UP) || (source == Piece::BLACK_KING && std::abs(dest_y - source_y) == 1);
    else
        return (source == Piece::WHITE && dest_y - source_y == DOWN) || (source == Piece::WHITE_KING && std::abs(dest_y - source_y) == 1);
}

/*
int main(int argc, char** argv) {
    CheckersApi api;
    std::cout << api.board << std::endl;
    std::cout << std::boolalpha << api.leagal_move(2, 5, 1, 4) << std::endl;
    std::cout << api.leagal_move(2, 5, 4, 3) << std::endl;
    std::cout << api.leagal_move(1, 4, 2, 3) << std::endl;
    api.board = api.board.move(0, 5, 1, 4);
    api.board = api.board.move(3, 2, 2, 3);
    std::cout << api.board << std::endl;

    std::cout << api.leagal_capture(1, 4, 2, 3) << std::endl;
    std::cout << api.leagal_capture(3, 5, 4, 6) << std::endl;
    std::cout << api.leagal_capture(6, 5, 5, 4) << std::endl;
    std::cout << api.leagal_capture(2, 3, 1, 4) << std::endl;
    api.switch_turn();
    std::cout << api.leagal_capture(2, 3, 1, 4) << std::endl;
}
*/
