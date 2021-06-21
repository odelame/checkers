#! /bin/env python3

from piece import Piece
import pygame
from interface import Board
from collections import namedtuple
from consts import SQUARE_SIZE, BLUE, ROWS, COLS, BLACK_WINS, WHITE_WINS
from enum import Enum

class CheckersApi:
    class x_direction(Enum):
        right = 1
        left = -1
        
        def __str__(self):
            return self.name
        
        def __repr__(self):
            return "x_direction('" + str(self) + "')"
    
    class y_direction(Enum):
        down = 1
        up = -1
        
        def __str__(self):
            return self.name
        
        def __repr__(self):
            return "y_direction('" + str(self) + "')"
    
    def __init__(self, board: Board):
        self.board = board
        self.turn = 'b'
        self.selected = None
        self.after_jump = False
        self.jumps_available = False
        
    def __getitem__(self, position):
        return self.board[position[0]][position[1]]
    
    def __setitem__(self, position, value):
        self.board[position[0]][position[1]] = value

    def select(self, x: int, y: int):
        x //= SQUARE_SIZE
        y //= SQUARE_SIZE

        if self[x, y] != None and self[x, y].color == self.turn:
            self.selected = (x, y)
            return

        if self.selected != None and (x, y) in self.moves(self.selected):
            self.move(self.selected, (x, y))
            
    def get_direction(self, position):
        return CheckersApi.y_direction.up if self.get_color(position) == "b" else CheckersApi.y_direction.down
    
    def get_color(self, position):
        return self[position].color
    
    def y_leagal_directions(self, position):
        y_leagal_direction = [self.get_direction(position).value]
        if self[position].king:
            y_leagal_direction.append(-self.get_direction(position).value)
        
        return y_leagal_direction
    
    def move(self, source, destination):
        if destination in self.jumps(source):
            self[destination] = self[source]
            self[(source[0] + destination[0]) // 2, (source[1] + destination[1]) // 2] = None
            self[source] = None
            self.after_jump = True 
            self.selected = destination
            if len(self.jumps(destination)) == 0:
                self._switch_turn()    
        elif destination in self.moves(source):
            self[destination] = self[source]
            self[source] = None
            self._switch_turn()
        else:
            return
        
        if destination[1] == 0 or destination[1] == ROWS - 1:
            self[destination].promote()
 
    def moves(self, position):
        if self.jumps_available or self.after_jump:
            return self.jumps(position)

        if not self.after_jump:
            y_leagal_direction = self.y_leagal_directions(position)
            x, y = position
            possible_moves = [(x + 1, y + y_direction) for y_direction in y_leagal_direction] + [(x - 1, y + y_direction) for y_direction in y_leagal_direction]  
        
            return [move for move in possible_moves if self.leagal_move(position, move, y_leagal_direction)] + self.jumps(position)
        return self.jumps(position)
    
    def jumps(self, position):
        y_leagal_direction = self.y_leagal_directions(position)
        x, y = position
        possible_jumps = [(x - 2, y + 2 * y_direction) for y_direction in y_leagal_direction] + [(x + 2, y + 2 * y_direction) for y_direction in y_leagal_direction]
        return [jump for jump in possible_jumps if self.leagal_jump(position, jump, y_leagal_direction)]
    
    def leagal_jump(self, source, dest, y_leagal_directions):
        return self.includes(dest) and self[dest] == None and abs(dest[0] - source[0]) == 2 and (dest[1] - source[1]) / 2 in y_leagal_directions and self[(dest[0] + source[0]) // 2, (dest[1] + source[1]) // 2] != None and self.get_color(((dest[0] + source[0]) // 2, (dest[1] + source[1]) // 2)) != self.get_color(source)
        
    def leagal_move(self, source, dest, y_leagal_directions):
        return self.includes(dest) and self[dest] == None and abs(dest[0] - source[0]) == 1 and dest[1] - source[1] in y_leagal_directions
        
    def includes(self, coordinates):
        return 0 <= coordinates[0] < len(self.board) and 0 <= coordinates[1] < len(self.board[0])
        
    def _switch_turn(self):
        if self.turn == "b":
            self.turn = "w"
        else:
            self.turn = "b"

        game_over = True
        self.after_jump = False
        self.selected = None

        self.jumps_available = False

        for i, row in enumerate(self.board.pieces):
            for j, piece in enumerate(row):
                if piece != None and piece.color == self.turn and len(self.jumps((i, j))) != 0:
                    game_over = False
                    self.jumps_available = True
                    break
                if game_over and piece != None and piece.color == self.turn and len(self.moves((i, j))) != 0:
                    game_over = False

            if not game_over and self.jumps_available:
                break

        if game_over:
            if self.turn == "w":
                pygame.event.post(pygame.event.Event(BLACK_WINS))
            else:
                pygame.event.post(pygame.event.Event(WHITE_WINS))

    def draw(self, win):
        self.board.draw(win)
        if None != self.selected:
            for x, y in self.moves(self.selected):
                pygame.draw.circle(win, BLUE, (x * SQUARE_SIZE + SQUARE_SIZE //
                                                2, y * SQUARE_SIZE + SQUARE_SIZE // 2), SQUARE_SIZE // 4)
                    
