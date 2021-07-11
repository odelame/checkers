# Checkers

This is an implementation of the checkers game with there rules:
    1. Each player starts with 12 pieces in the standard checkers position.
    2. Black moves first.
    3. Captures are mandatory, if multiple captures are available player can choose which one.
    4. Chain captures: the act of capturing multiple pieces one after the other, is allowed, furthermore once a player captures he or she must continue capturing with that piece until that is no longer possible.
    5. Reaching the last row promotes a piece to a king.
    6. A king can move backwards.
    7. Only a king can capture backwards.

## Requirements:

[Python3.9](https://www.python.org/downloads/) to run the game.

Use the package manager [pip](https://pip.pypa.io/en/stable/) to install pygame and pybind11.

The c++ [Boost](https://www.boost.org/users/download/) library is used.
Make sure boost in on the system include path.

## Setup:

Run:
```
pip install -r requirements.txt && make
```

## Usage:

To start the game in 1 player vs the computer mode run:
```
python3 main.py
```
Click the piece you desire to move and the destination, will only move if movement is legal.
Press space to get a hint: The piece the engine thinks is the one you should move will be marked in red.

For explanation of all the possibilities: 
```
python3 main.py --help
```

If you want to change the wait time in seconds between moves, default it 0.5.
```
python3 main.py --delay x
```

To change the depth the engine will look into when playing a move (depth=>how many moves ahead will the computer look into), default is 6.
NOTE: Increasing depth may significantly slow down performance.
```
python3 main.py --depth x
```

You can flip the board with:
```
python3 main.py --flip
```

You may choose to play with the white pieces with:
```
python3 main.py --color "white"
```

You may want to match two engines of set depth with:
```
python3 main.py --match --depth-black x --depth-white y
```
where x and y are the depth of black and white, default to 6.



Run example:

```
python3 main.py --depth 8 --delay 0
```

```
python3 main.py --match --depth-black 8 --depth-white 5 --delay 0
```

```
python3 main.py --match --depth-black 8 --depth-white 10 --delay 0 --flip
```


