#ifndef BITBOARD_HPP
#define BITBOARD_HPP

#include <iostream>
#include <bitset>
#include <iomanip>
#include "consts.hpp"
#include <tuple>
#include <vector>
#include <algorithm>
#include <boost/functional/hash.hpp>
#include <sstream>

class BitBoard {
private:
    // 4293918720ULL => the number that in the bitboard represents all the starting black squares.
    std::bitset<NUMBER_OF_REACHABLE_SQUARES> black_is_in = { 4293918720ULL };
    // 4095ULL => the number that in the bitboard represents all the starting white squares.
    std::bitset<NUMBER_OF_REACHABLE_SQUARES> white_is_in = { 4095ULL };
    std::bitset<NUMBER_OF_REACHABLE_SQUARES> kings = { 0ULL };

    void set_king(const unsigned int x, const unsigned int y);
    void set(const unsigned int x, const unsigned int y, const Piece value);

public:
    BitBoard();
    BitBoard(const BitBoard& other);

    Piece get(const unsigned int x, const unsigned int y) const;

    bool is_king(const unsigned int x, const unsigned int y) const;
    bool is_black(const unsigned int x, const unsigned int y) const;
    bool is_white(const unsigned int x, const unsigned int y) const;
    bool is_black(const std::pair<const unsigned int, const unsigned int> coords) const;
    bool is_white(const std::pair<const unsigned int, const unsigned int> coords) const;
    bool is_king(const std::pair<const unsigned int, const unsigned int> coords) const;
    void set_king(const std::pair<const unsigned int, const unsigned int> coords);

    short num_white() const;
    short num_black() const;
    short piece_count() const;
    short king_count() const;
    short num_white_kings() const;
    short num_black_kings() const;

    BitBoard move(const unsigned int source_x, const unsigned int source_y, const unsigned int dest_x, const unsigned int dest_y) const;
    BitBoard capture(const unsigned int source_x, const unsigned int source_y, const unsigned int dest_x, const unsigned int dest_y) const;

    bool legal_capture(bool black_turn, const unsigned int source_x, const unsigned int source_y, const unsigned int capture_x, const unsigned int capture_y) const;
    bool legal_move(const bool black_turn, const unsigned int source_x, const unsigned int source_y, const unsigned int dest_x, const unsigned int dest_y) const;

    std::vector<BitBoard> moves(const bool black_turn, const unsigned int x, const unsigned int y) const;
    std::vector<BitBoard> captures(const bool black_turn, const unsigned int x, const unsigned int y) const;

    std::vector<BitBoard> captures(const bool black_turn) const;
    std::vector<BitBoard> moves(const bool black_turn) const;

    std::bitset<NUMBER_OF_REACHABLE_SQUARES> operator^(const BitBoard& other) const;
    std::bitset<NUMBER_OF_REACHABLE_SQUARES> operator&(const BitBoard& other) const;
    std::bitset<NUMBER_OF_REACHABLE_SQUARES> operator^(std::bitset<NUMBER_OF_REACHABLE_SQUARES> other) const;
    std::bitset<NUMBER_OF_REACHABLE_SQUARES> operator&(std::bitset<NUMBER_OF_REACHABLE_SQUARES> other) const;

    bool operator==(const BitBoard& other) const;
    bool operator!=(const BitBoard& other) const;
    BitBoard& operator=(const BitBoard& other);
    class Iterator {
    private:
        unsigned int index;
        const BitBoard& board;
        void skip_to_next();
        void skip_to_pre();
    public:
        Iterator(const unsigned int index, const BitBoard& board);
        Iterator& operator++();
        Iterator& operator--();
        Iterator operator++(int);
        Iterator operator--(int);
        std::pair<unsigned int, unsigned int> operator*();
        bool operator==(const Iterator& other) const;
        bool operator!=(const Iterator& other) const;
    };

    Iterator begin() const;
    Iterator end() const;

    struct hasher {
        std::size_t operator()(const BitBoard& board) const;
    };
    friend std::ostream& operator<<(std::ostream& strm, const BitBoard& board);
};

std::bitset<NUMBER_OF_REACHABLE_SQUARES> operator^(std::bitset<NUMBER_OF_REACHABLE_SQUARES> bits, const BitBoard& board);
std::bitset<NUMBER_OF_REACHABLE_SQUARES> operator&(std::bitset<NUMBER_OF_REACHABLE_SQUARES> bits, const BitBoard& board);

std::stringstream& operator<<(std::stringstream& strm, const BitBoard& board);
std::stringstream& operator<<(std::stringstream& strm, Piece piece);
std::ostream& operator<<(std::ostream& strm, const BitBoard& board);
std::ostream& operator<<(std::ostream& strm, Piece piece);
bool in_bounds(const unsigned int index);

#include "helpFuncs.hpp"

#endif // BITBOARD_HPP
