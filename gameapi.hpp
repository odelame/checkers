#ifndef _GAMEAPI_HPP_
#define _GAMEAPI_HPP_

#include "bitboard.hpp"
#include <tuple>
#include <vector>
#include <list>
#include "consts.hpp"


class CheckersApi {
private:
    BitBoard board;
    bool black_turn;
    bool jumps_available;
    void switch_turn();
    bool leagal_capture(const int source_x, const int source_y, const int capture_x, const int capture_y);
    bool leagal_move(const int source_x, const int source_y, const int dest_x, const int dest_y);
public:
    CheckersApi();
    // CheckersApi(BitBoard&& board, bool black_turn);
    bool move(const int source_x, const int source_y, const int dest_x, const int dest_y);
    std::vector<BitBoard> moves(const int x, const int y);
    std::vector<BitBoard> jumps(const int x, const int y);
    std::vector<BitBoard> moves();
    friend int main(int argc, char** argv);
};

std::vector<std::pair<int, int>> get_candidate_locations(const int x, const int y);

#endif // _GAMEAPI_HPP_
