#! /bin/env python3

from os import environ
environ["PYGAME_HIDE_SUPPORT_PROMPT"] = "1"

import pygame
from consts import *
from checkers import Api
from argparse import ArgumentParser

FPS = 60

pygame.font.init()
MSG_FONT = pygame.font.SysFont('comicsans', 100)

def select(x: int, y: int, rotation=False):
    x //= SQUARE_SIZE
    y //= SQUARE_SIZE
    
    if rotation:
        y = ROWS - y - 1
        x = ROWS - x - 1
      
    return (x, y)

def draw_msg(win, text: str):
    draw_text = MSG_FONT.render(text, 1, RED)
    win.blit(draw_text,  (WIDTH/2 - draw_text.get_width() /
             2, HEIGHT/2 - draw_text.get_height()/2))
    pygame.display.update()
    pygame.time.delay(5000)
    
def circle_square(win, color, coords, radius=2 * SQUARE_SIZE // 5, rotation=False):
    x, y = coords
    if not rotation:
        pos = (x * SQUARE_SIZE + SQUARE_SIZE // 2, y * SQUARE_SIZE + SQUARE_SIZE // 2)
    else:
        pos = ((COLS - x) * SQUARE_SIZE - SQUARE_SIZE // 2, (ROWS - y) * SQUARE_SIZE - SQUARE_SIZE // 2)
    pygame.draw.circle(win, color, pos, radius)
    
def draw(win, checkers_api, selection=None, hint=None, rotation=False):
    win.fill(BLACK)
        
    for r in range(ROWS):  
            for c in range(r % 2, COLS, 2):
                pygame.draw.rect(win, WHITE, (r * SQUARE_SIZE,
                                    c * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE))
    for x in range(COLS):
        for y in range(ROWS): 
            if checkers_api.is_white(x, y):
                circle_square(win, WHITE, (x, y), rotation=rotation)
            elif checkers_api.is_black(x, y):
                circle_square(win, WHITE, (x, y), rotation=rotation)
                circle_square(win, BLACK, (x, y), radius=SQUARE_SIZE * 2 // 5 - WRAP, rotation=rotation)
            if checkers_api.is_king(x, y):
                circle_square(win, YELLOW, (x, y), radius=SQUARE_SIZE // 5, rotation=rotation)
    
    if hint:
        circle_square(win, RED, hint, radius=SQUARE_SIZE // 5, rotation=rotation)  

    if None != selection:
        for x, y in checkers_api.moves(*selection):
            circle_square(win, BLUE, (x, y), radius=SQUARE_SIZE // 4, rotation=rotation)
        
def display(depth=6, color="black", rotate=False):
    win = pygame.display.set_mode((WIDTH, HEIGHT))  
    pygame.display.set_caption("Checkers")
    rotation = (color == "white")
    rotation = rotate != rotation
    clock = pygame.time.Clock()
    game_api = Api(depth)
    selection = None
    hint = None

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
                        hint = None
                else:  
                    selection = select(*pygame.mouse.get_pos(), rotation)
                    if selection != hint: hint = None
                    
            elif event.type == pygame.KEYDOWN and event.key == pygame.K_SPACE:
                hint = game_api.hint()
                             
        draw(win, game_api, selection, hint, rotation)
        
        if game_api.game_over:
            if game_api.black_move:
                draw_msg(win, "WHITE WINS!!")
            else:
                draw_msg(win, "BLACK WINS!!")
            game_running = False   
          
        pygame.display.update()

    pygame.quit()
    
def match(black_depth, white_depth, delay):
    win = pygame.display.set_mode((WIDTH, HEIGHT))
    pygame.display.set_caption("Checkers")
    clock = pygame.time.Clock()
    engine_black = Api(black_depth)
    engine_white = Api(white_depth)
    
    game_running = True
    black_turn = True

    while game_running:
        clock.tick(1 / delay)

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                game_running = False
           
        if black_turn:
            moves = engine_black.best_move()
            source, dest = moves[0]
            engine_black.move(*source, *dest)
            engine_white.move(*source, *dest)
        else:
            moves = engine_white.best_move()
            source, dest = moves[0]
            engine_white.move(*source, *dest)
            engine_black.move(*source, *dest)     

        draw(win, engine_black)
        black_turn = engine_black.black_move;
        
        if engine_black.game_over:
            if engine_black.black_move:
                draw_msg(win, "WHITE WINS!!")
            else:
                draw_msg(win, "BLACK WINS!!")
            game_running = False   
          
        pygame.display.update()

    pygame.quit()


if __name__ == '__main__':
    parser = ArgumentParser()
    parser.add_argument("-m", "--match", action="store_true", default=False)
    parser.add_argument("-d", "--depth", action="store",type=int, default=6)
    parser.add_argument("-db", "--depth-black", action="store",type=int, default=6)
    parser.add_argument("-dw", "--depth-white", action="store",type=int, default=6)
    parser.add_argument("-dl", "--delay", action="store", type=float, default=0.5)
    parser.add_argument("-c", "--color", action="store",type=str, default="black")
    parser.add_argument("-r", "--rotate", action="store_true", default=False)
    args = parser.parse_args()
    
    if not args.match:
        display(args.depth, args.color, args.rotate)
    else:
        match(args.depth_black, args.depth_white, args.delay)
