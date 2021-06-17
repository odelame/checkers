#! /bin/env python3

from consts import SQUARE_SIZE, WHITE, BLACK, YELLOW
import pygame


class Piece:
    WRAP = SQUARE_SIZE // 14

    def __init__(self, color):
        self._color = color
        self.king = False

    def promote(self):
        self.king = True

    def draw(self, WIN, x, y):
        pos = (x * SQUARE_SIZE + SQUARE_SIZE // 2,
               y * SQUARE_SIZE + SQUARE_SIZE // 2)
        if self._color == "w":
            pygame.draw.circle(WIN, WHITE, pos,
                               SQUARE_SIZE * 2 // 5)
        else:
            pygame.draw.circle(
                WIN, WHITE, pos, SQUARE_SIZE * 2 // 5)
            pygame.draw.circle(WIN, BLACK, pos,
                               SQUARE_SIZE * 2 // 5 - self.WRAP)

        if self.king:
            pygame.draw.circle(WIN, YELLOW, pos, SQUARE_SIZE // 8)

    def __repr__(self):
        return str(self.color)

    @property
    def color(self):
        return self._color
