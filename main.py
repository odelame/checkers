#! /bin/env python3

from os import environ
environ["PYGAME_HIDE_SUPPORT_PROMPT"] = "1"

import pygame
from consts import *
from checkers import Api, Piece

FPS = 60
WIN = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("Checkers")

pygame.font.init()
MSG_FONT = pygame.font.SysFont('comicsans', 100)

def select(x: int, y: int):
    x //= SQUARE_SIZE
    y //= SQUARE_SIZE
    
    return (x, y)


def draw_msg(text: str):
    draw_text = MSG_FONT.render(text, 1, RED)
    WIN.blit(draw_text,  (WIDTH/2 - draw_text.get_width() /
             2, HEIGHT/2 - draw_text.get_height()/2))
    pygame.display.update()
    pygame.time.delay(5000)
    
    
def draw(win, checkers_api, selection):
    win.fill(BLACK)
        
    for r in range(ROWS):       
        for c in range(r % 2, COLS, 2):
            pygame.draw.rect(win, WHITE, (r * SQUARE_SIZE,
                                c * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE))
    
    for x in range(COLS):
        for y in range(ROWS):
            pos = (x * SQUARE_SIZE + SQUARE_SIZE // 2,
                y * SQUARE_SIZE + SQUARE_SIZE // 2)

            if checkers_api.is_white((x, y)):
                pygame.draw.circle(win, WHITE, pos, SQUARE_SIZE * 2 // 5)
            elif checkers_api.is_black((x, y)):
                pygame.draw.circle(win, WHITE, pos, SQUARE_SIZE * 2 // 5)
                pygame.draw.circle(win, BLACK, pos, SQUARE_SIZE * 2 // 5 - WRAP)

    if None != selection:
        for x, y in checkers_api.moves(selection):
            pygame.draw.circle(win, BLUE, (x * SQUARE_SIZE + SQUARE_SIZE //
                                            2, y * SQUARE_SIZE + SQUARE_SIZE // 2), SQUARE_SIZE // 4)

def move(checkers_api, source, destination):
    if checkers_api.move(*source, *destination):
        return None
    else:
        return destination   
        
        
def display():
    clock = pygame.time.Clock()
    game_api = Api()
    selection = None

    game_running = True

    while game_running:
        clock.tick(FPS)

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                game_running = False

            elif event.type == pygame.MOUSEBUTTONDOWN:
                if None != selection:
                    selection = move(game_api, selection, select(*pygame.mouse.get_pos()))
                else:  
                    selection = select(*pygame.mouse.get_pos())

            elif event.type == WHITE_WINS:
                draw_msg("WHITE WINS")
                game_running = False

            elif event.type == BLACK_WINS:
                draw_msg("BLACK WINS")
                game_running = False
                                

        draw(WIN, game_api, selection)
        pygame.display.update()

    pygame.quit()


if __name__ == '__main__':
    display()
