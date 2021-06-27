#! /bin/env python3

import pygame

WIDTH = HEIGHT = 800
ROWS = COLS = 8
SQUARE_SIZE = WIDTH // COLS
WRAP = SQUARE_SIZE // 14

RED = (255, 0, 0)
WHITE = (255, 255, 255)
BLACK = (0, 0, 0)
BLUE = (0, 0, 255)
YELLOW = (255, 255, 0)

WHITE_WINS = pygame.USEREVENT + 1
BLACK_WINS = pygame.USEREVENT + 2
