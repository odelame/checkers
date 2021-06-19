#! /bin/env python3

from os import environ
environ["PYGAME_HIDE_SUPPORT_PROMPT"] = "1"

import pygame
from consts import WIDTH, HEIGHT, WHITE_WINS, BLACK_WINS, RED
from interface import Board
from gameapi import CheckersApi

FPS = 60
WIN = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("Checkers")

pygame.font.init()
MSG_FONT = pygame.font.SysFont('comicsans', 100)


def draw_msg(text: str):
    draw_text = MSG_FONT.render(text, 1, RED)
    WIN.blit(draw_text,  (WIDTH/2 - draw_text.get_width() /
             2, HEIGHT/2 - draw_text.get_height()/2))
    pygame.display.update()
    pygame.time.delay(5000)


def display():
    clock = pygame.time.Clock()
    board = Board()
    game_api = CheckersApi(board)

    game_running = True

    while game_running:
        clock.tick(FPS)

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                game_running = False

            elif event.type == pygame.MOUSEBUTTONDOWN:
                game_api.select(*pygame.mouse.get_pos())

            elif event.type == WHITE_WINS:
                draw_msg("WHITE WINS")
                game_running = False

            elif event.type == BLACK_WINS:
                draw_msg("BLACK WINS")
                game_running = False
                                

        game_api.draw(WIN)
        pygame.display.update()

    pygame.quit()


if __name__ == '__main__':
    display()
