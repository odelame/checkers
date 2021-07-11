#! /bin/env python3

from os import environ
# surpress the pygame msg
environ["PYGAME_HIDE_SUPPORT_PROMPT"] = "1"

import pygame
from consts import *
from checkers import Api
from argparse import ArgumentParser

FPS = 60

pygame.font.init()
MSG_FONT = pygame.font.SysFont('comicsans', 100)

def select(x: int, y: int, rotation=False):
    """gets the x, y coordinates of a mouse click and wether or not the screen was rotated and returns the x,y coordinates on the board.

    Args:
        x (int): x of the clicked coordinate
        y (int): y of the clicked coordinate
        rotation (bool, optional): wether the screen was rotated or not. Defaults to False.

    Returns:
        tuple[int]: (x, y) coordinates of the clicked square
    """
    x //= SQUARE_SIZE
    y //= SQUARE_SIZE
    
    if rotation:
        y = ROWS - y - 1
        x = ROWS - x - 1
      
    return (x, y)

def draw_msg(win, text: str, color: tuple[int]=RED, time: int=5):
    """prints a text colored text msg to the screen for an amount of time

    Args:
        win (pygame.surface): screen to print on
        text (str): text to print
        color (tuple[int], optional): color to print. Defaults to RED.
        time (int, optional): time in seconds for the msg on the board.
    """
    draw_text = MSG_FONT.render(text, 1, color)
    win.blit(draw_text,  (WIDTH/2 - draw_text.get_width() /
             2, HEIGHT/2 - draw_text.get_height()/2))
    pygame.display.update()
    pygame.time.delay(time * 1000)

def circle_square(win, color: tuple[int], coords: tuple[int], radius: int =2 * SQUARE_SIZE // 5, rotation: bool=False):
    """given coordinates color a circle in that coordinates.

    Args:
        win (pygame.serface]): the window to draw on.
        color (tuple[int]): RGB color.
        coords (tuple[int]): the x and y to print on.
        radius (int, optional): the radius of the circle to print. Defaults to 2*SQUARE_SIZE//5.
        rotation (bool, optional): wether or not the board is rotated. Defaults to False.
    """
    x, y = coords
    # pos will be where to print on
    if not rotation:
        pos = (x * SQUARE_SIZE + SQUARE_SIZE // 2, y * SQUARE_SIZE + SQUARE_SIZE // 2)
    else:
        pos = ((COLS - x) * SQUARE_SIZE - SQUARE_SIZE // 2, (ROWS - y) * SQUARE_SIZE - SQUARE_SIZE // 2)
    pygame.draw.circle(win, color, pos, radius)
    
def draw(win, checkers_api: Api, selection: tuple[int]=None, hint: tuple[int]=None, rotation: bool=False):
    """draw the checkers board to the screen

    Args:
        win (pygame.serface): The screen to print on.
        checkers_api (Api): The game api to print info from. 
        selection (tuple[int], optional): The coordinates selected by the user. Defaults to None.
        hint (tuple[int], optional): The hint coordinate, a coordinate to color with a circle. Defaults to None.
        rotation (bool, optional): Wether or not the screen rotated. Defaults to False.
    """
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
        for x, y in checkers_api.leagal_moves(*selection):
            circle_square(win, BLUE, (x, y), radius=SQUARE_SIZE // 4, rotation=rotation)
        
def handle_game(depth: int=6, color: str="black", delay: float=0.5, flip: bool=False):
    """handle the game between the user and the engine

    Args:
        depth (int, optional): The number depth of moves the engine will look into. Defaults to 6.
        color (str, optional): The color the user wants to play. Defaults to "black".
        rotate (bool, optional): Wether or not the user wants a rotated screen. Defaults to False.
    """    
    win = pygame.display.set_mode((WIDTH, HEIGHT))  
    pygame.display.set_caption("Checkers")
    # the usuall rotation of the board is color == "white", if user asked to rotate != rotate rotates it again.
    rotation = (color == "white") != flip
    clock = pygame.time.Clock()
    game_api = Api(depth)
    selection = None
    hint = None

    game_running = True
    
    # clock will tick in 1/delay, if delay is 0 we use inf to prevent a crash
    if delay == 0:
        from math import inf
        ticking_time = inf
    else:
        ticking_time = 1 / delay
    
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
                        hint = None
                        moves, eval = game_api.best_move()
                        draw(win, game_api, selection, hint, rotation)
                        pygame.display.update()
                        # play move by move wait moves a second until the engine is done.
                        for move in moves:
                            clock.tick(ticking_time)    
                            source, dest = move
                            game_api.move(*source, *dest)
                            draw(win, game_api, selection, hint, rotation)
                            pygame.display.update()

                else:  
                    selection = select(*pygame.mouse.get_pos(), rotation)
                    if selection != hint: hint = None
                    
            elif event.type == pygame.KEYDOWN and event.key == pygame.K_SPACE:
                hint = game_api.hint()
                             
        draw(win, game_api, selection, hint, rotation)
        if game_api.game_over:
            if game_api.draw:
                draw_msg(win, "DRAW !!")
            elif game_api.black_move:
                draw_msg(win, "WHITE WINS!!")
            else:
                draw_msg(win, "BLACK WINS!!")
            game_running = False   
        pygame.display.update()

    pygame.quit()
    
def match(black_depth: int=6, white_depth: int=6, delay: float=0.5, flip: bool=False):
    """play a match between two engines of set depth

    Args:
        black_depth (int, optional): The depth of moves the black engine will look into. Defaults to 6.
        white_depth (int, optional): The depth of moves the white engine will look into. Defaults to 6.
        delay (float, optional): Minimum time between moves, if engine takes more time will not effect waitint time. Defaults to 0.5.
    """
    win = pygame.display.set_mode((WIDTH, HEIGHT))
    pygame.display.set_caption("Checkers")
    clock = pygame.time.Clock()
    engine_black = Api(black_depth)
    engine_white = Api(white_depth)
    
    game_running = True
    black_turn = True
    
    # clock will tick in 1/delay, if delay is 0 we use inf to prevent a crash
    if delay == 0:
        from math import inf
        ticking_time = inf
    else:
        ticking_time = 1 / delay

    while game_running:
        clock.tick(ticking_time)

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                game_running = False
           
        if black_turn:
            # play the black move step by step.
            moves, eval = engine_black.best_move()
            for source, dest in moves:
                clock.tick(ticking_time)
                engine_black.move(*source, *dest)
                engine_white.move(*source, *dest)
                draw(win, engine_black, rotation=flip)
                pygame.display.update()
        else:
            moves, eval = engine_white.best_move()
            # play the white move step by step.
            for source, dest in moves:
                clock.tick(ticking_time)
                engine_black.move(*source, *dest)
                engine_white.move(*source, *dest)
                draw(win, engine_black, rotation=flip)
                pygame.display.update()

        black_turn = engine_black.black_move;
        
        if engine_white.game_over:
            if engine_white.draw:
                draw_msg(win, "DRAW !!")
            elif engine_white.black_move:
                draw_msg(win, "WHITE WINS!!")
            else:
                draw_msg(win, "BLACK WINS!!")
            game_running = False  
            pygame.display.update()
            
    pygame.quit()


if __name__ == '__main__':
    parser = ArgumentParser()
    parser.add_argument("-m", "--match", help="make the engine play against itself", action="store_true", default=False)
    parser.add_argument("-d", "--depth", help="The depth of moves the engine will look into, no effect with --match", action="store",type=int, default=6)
    parser.add_argument("-db", "--depth-black", help="The depth of moves black engine will look into, only effective with --match", action="store",type=int, default=6)
    parser.add_argument("-dw", "--depth-white", help="The depth of moves white engine will look into, only effective with --match", action="store",type=int, default=6)
    parser.add_argument("-dl", "--delay", help="The delay between moves, may be more due to engine computation time", action="store", type=float, default=0.5)
    parser.add_argument("-c", "--color", help="Choose a color to play with", action="store",type=str, default="black")
    parser.add_argument("-f", "--flip", help="Flip the board",  action="store_true", default=False)
    args = parser.parse_args()

    if not args.match:
        handle_game(args.depth, args.color, args.delay, args.flip)
    else:
        match(args.depth_black, args.depth_white, args.delay, args.flip)
