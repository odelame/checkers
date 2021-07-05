#! /bin/env python3

from os import environ
environ["PYGAME_HIDE_SUPPORT_PROMPT"] = "1"

import pygame
from consts import *
from checkers import Api
from argparse import ArgumentParser

FPS = 60
WIN = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("Checkers")

pygame.font.init()
MSG_FONT = pygame.font.SysFont('comicsans', 100)

def select(x: int, y: int, rotation=False):
    x //= SQUARE_SIZE
    y //= SQUARE_SIZE
    
    if rotation:
        y = ROWS - y - 1
        x = ROWS - x - 1
      
    return (x, y)

def draw_msg(text: str):
    draw_text = MSG_FONT.render(text, 1, RED)
    WIN.blit(draw_text,  (WIDTH/2 - draw_text.get_width() /
             2, HEIGHT/2 - draw_text.get_height()/2))
    pygame.display.update()
    pygame.time.delay(5000)
    
def draw(win, checkers_api, selection, rotation=False):
    win.fill(BLACK)
        
    for r in range(ROWS):  
            for c in range(r % 2, COLS, 2):
                pygame.draw.rect(win, WHITE, (r * SQUARE_SIZE,
                                    c * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE))
           
    for x in range(COLS):
        for y in range(ROWS):
            if not rotation:
                pos = (x * SQUARE_SIZE + SQUARE_SIZE // 2,
                    y * SQUARE_SIZE + SQUARE_SIZE // 2)
            else:
                pos = ((COLS - x) * SQUARE_SIZE - SQUARE_SIZE // 2, (ROWS - y) * SQUARE_SIZE - SQUARE_SIZE // 2)
                
            if checkers_api.is_white(x, y):
                pygame.draw.circle(win, WHITE, pos, SQUARE_SIZE * 2 // 5)
            elif checkers_api.is_black(x, y):
                pygame.draw.circle(win, WHITE, pos, SQUARE_SIZE * 2 // 5)
                pygame.draw.circle(win, BLACK, pos, SQUARE_SIZE * 2 // 5 - WRAP)
            if checkers_api.is_king(x, y):
                pygame.draw.circle(win, YELLOW, pos, SQUARE_SIZE // 5)

    if None != selection:
        for x, y in checkers_api.moves(*selection):
            if not rotation:
                pos = (x * SQUARE_SIZE + SQUARE_SIZE // 2,
                    y * SQUARE_SIZE + SQUARE_SIZE // 2)
            else:
                pos = ((COLS - x) * SQUARE_SIZE - SQUARE_SIZE // 2, (ROWS - y) * SQUARE_SIZE - SQUARE_SIZE // 2)
            pygame.draw.circle(win, BLUE, pos, SQUARE_SIZE // 4)
        
def display(depth=6, color="black"):
    rotation = (color == "white")
    clock = pygame.time.Clock()
    game_api = Api(depth)
    selection = None

    game_running = True
    
    if color=="white":
        game_api.play()

    while game_running:
        clock.tick(FPS)

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                game_running = False

            elif event.type == pygame.MOUSEBUTTONDOWN:
                if None != selection:
                    selection = game_api.move(*selection, *select(*pygame.mouse.get_pos(), rotation))
                    if None == selection and not game_api.game_over:
                        game_api.play()
                else:  
                    selection = select(*pygame.mouse.get_pos(), rotation)
                             
        draw(WIN, game_api, selection, rotation)
        
        if game_api.game_over:
            if game_api.black_move:
                draw_msg("WHITE WINS!!")
            else:
                draw_msg("BLACK WINS!!")
            game_running = False   
          
        pygame.display.update()

    pygame.quit()


if __name__ == '__main__':
    parser = ArgumentParser()
    parser.add_argument("-d", "--depth", action="store",type=int, default=6)
    parser.add_argument("-c", "--color", action="store",type=str, default="black")
    args = parser.parse_args()
    display(args.depth, args.color)
