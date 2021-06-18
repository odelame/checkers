#! /bin/env python3

import pygame
from consts import BLACK, ROWS, COLS, RED, SQUARE_SIZE, WHITE
from piece import Piece
from collections import namedtuple


class Board:
    def __init__(self):
        self.selected_piece = None
        self.pieces = [[None] * 8 for i in range(8)]

        for r in range(3):
            for c in range(abs(r % 2 - 1), COLS, 2):
                self.pieces[c][r] = Piece("w")

        for r in range(ROWS - 1, ROWS - 4, -1):
            for c in range(abs(r % 2 - 1), COLS, 2):
                self.pieces[c][r] = Piece("b")

    def draw_cubes(self, win, rotate=False):
        win.fill(BLACK)

        for r in range(ROWS):
            if not rotate:
                for c in range(r % 2, COLS, 2):
                    pygame.draw.rect(win, WHITE, (r * SQUARE_SIZE,
                                     c * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE))
            else:
                for c in range((r + 1) % 2, COLS, 2):
                        pygame.draw.rect(win, WHITE, (r * SQUARE_SIZE,
                                     c * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE))

    def draw_pieces(self, WIN, rotate=False):
        if not rotate:
            for i, row in enumerate(self.pieces):
                for j, piece in enumerate(row):
                    if piece != None:
                        piece.draw(WIN, i, j)
        else:
            for i, row in enumerate(reversed(self.pieces)):
                for j, piece in enumerate(reversed(row)):
                    if piece != None:
                        piece.draw(WIN, COLS - i - 1, j)
    
    def __len__(self):
        return len(self.pieces)

    def draw(self, win, rotate=False):
        self.draw_cubes(win, rotate)
        self.draw_pieces(win, rotate)

    def __getitem__(self, index):
        return self.pieces[index]
