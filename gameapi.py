#! /bin/env python3

from piece import Piece
import pygame
from interface import Board
from collections import namedtuple
from consts import SQUARE_SIZE, BLUE, ROWS, COLS, BLACK_WINS, WHITE_WINS


class CheckersApi:
    def __init__(self, board: Board):
        self.board = board
        self.turn = 'b'
        self.coordinates = namedtuple("coordinates", ["x", "y"])
        self.selected = None

    def select(self, x: int, y: int):
        x //= SQUARE_SIZE
        y //= SQUARE_SIZE

        if self.board[x][y] != None and self.board[x][y].color == self.turn:
            self.selected = self.coordinates(x, y)
            return

        if self.selected != None and self.leagal_move(self.selected, self.coordinates(x, y)):
            self.move(self.selected, self.coordinates(x, y))
            self.selected = None
            self.switch_turn()

    def move(self, origin, result):
        self.board[result.x][result.y] = self.board[origin.x][origin.y]
        self.board[origin.x][origin.y] = None
        if abs(result.x - origin.x) == 2:
            self.board[(origin.x + result.x) //
                       2][(origin.y + result.y) // 2] = None

        if result.y == 0 or result.y == ROWS - 1:
            self.board[result.x][result.y].promote()

    def leagal_moves(self, position):
        moves = []

        x, y = position
        direction = -1 if self.board[x][y].color == "b" else 1

        if x - 1 >= 0 and 0 <= y + direction < ROWS and self.board[x - 1][y + direction] == None:
            moves.append((x - 1, y + direction))
        if x + 1 < COLS and 0 <= y + direction < ROWS and self.board[x + 1][y + direction] == None:
            moves.append((x + 1, y + direction))

        if x - 2 >= 0 and 0 <= y + 2 * direction < ROWS and self.board[x - 2][y + 2 * direction] == None and None != self.board[x - 1][y + direction] and self.board[x - 1][y + direction].color != self.board[x][y].color:
            moves.append((x - 2, y + direction * 2))

        if x + 2 < ROWS and 0 <= y + 2 * direction < ROWS and self.board[x + 2][y + 2 * direction] == None and None != self.board[x + 1][y + direction] and self.board[x + 1][y + direction].color != self.board[x][y].color:
            moves.append((x + 2, y + direction * 2))

        if self.board[x][y].king:
            direction = -direction
            if x - 1 >= 0 and 0 <= y + direction < ROWS and self.board[x - 1][y + direction] == None:
                moves.append((x - 1, y + direction))
            if x + 1 < COLS and 0 <= y + direction <= ROWS and self.board[x + 1][y + direction] == None:
                moves.append((x + 1, y + direction))

            if x - 2 >= 0 and 0 <= y + 2 * direction < ROWS and self.board[x - 2][y + 2 * direction] == None and None != self.board[x - 1][y + direction] and self.board[x - 1][y + direction].color != self.board[x][y].color:
                moves.append((x - 2, y + direction * 2))

            if x + 2 < ROWS and 0 <= y + 2 * direction < ROWS and self.board[x + 2][y + 2 * direction] == None and None != self.board[x + 1][y + direction] and self.board[x + 1][y + direction].color != self.board[x][y].color:
                moves.append((x + 2, y + direction * 2))

        return moves

    def leagal_move(self, origin, result):
        return result in self.leagal_moves(origin)

    def switch_turn(self):
        if self.turn == "b":
            self.turn = "w"
        else:
            self.turn = "b"

        game_over = True

        for i, row in enumerate(self.board.pieces):
            for j, piece in enumerate(row):
                if piece != None and piece.color == self.turn and len(self.leagal_moves(self.coordinates(i, j))) != 0:
                    game_over = False
                    break
            if not game_over:
                break

        if game_over:
            if self.turn == "w":
                pygame.event.post(pygame.event.Event(BLACK_WINS))
            else:
                pygame.event.post(pygame.event.Event(WHITE_WINS))

    def draw(self, win):
        self.board.draw(win)
        if None != self.selected:
            for x, y in self.leagal_moves(self.selected):
                pygame.draw.circle(win, BLUE, (x * SQUARE_SIZE + SQUARE_SIZE //
                                               2, y * SQUARE_SIZE + SQUARE_SIZE // 2), SQUARE_SIZE // 4)
