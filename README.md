# Checkers Game

To start install the required boost c++ library and pybind11 python module.
Then run make.

You can start playing with python3 main.py

usage: python3 main.py [-h] [-m] [-d DEPTH] [-db DEPTH_BLACK] [-dw DEPTH_WHITE]
               [-dl DELAY] [-c COLOR] [-f]

optional arguments:
  -h, --help            show this help message and exit
  -m, --match           make the engine play against itself
  -d DEPTH, --depth DEPTH
                        The depth of moves the engine will look into, no
                        effect with --match
  -db DEPTH_BLACK, --depth-black DEPTH_BLACK
                        The depth of moves black engine will look into, only
                        effective with --match
  -dw DEPTH_WHITE, --depth-white DEPTH_WHITE
                        The depth of moves white engine will look into, only
                        effective with --match
  -dl DELAY, --delay DELAY
                        The delay between moves, may be more due to engine
                        computation time
  -c COLOR, --color COLOR
                        Choose a color to play with
  -f, --flip            Flip the board
usage: main.py [-h] [-m] [-d DEPTH] [-db DEPTH_BLACK] [-dw DEPTH_WHITE]
               [-dl DELAY] [-c COLOR] [-f]
