PYBIND11_INCLUDES = $(shell python3 -m pybind11 --includes)
PYLIB_SUFFIX = $(shell python3-config --extension-suffix)
LINK.o = $(LINK.cpp)
CPP = c++
STD = c++20
OPT = Ofast
CPPFLAGS = -std=$(STD) -Wall -pedantic -$(OPT) -fPIC
LIB_NAME = checkers
LIB = -ltbb
LINK.o = $(LINK.cpp)

all: bitboard.o	gameapi.o 	helpFuncs.o 	engine.o
	$(LINK.o) -shared $(CPPFLAGS) $^ -o $(LIB_NAME)$(PYLIB_SUFFIX) $(LIB)

checkers:	all
	./main.py

engine.o: engine.cpp 	engine.hpp 	helpFuncs.hpp 	consts.hpp
	$(CPP) $(CPPFLAGS) $^ -c

gameapi.o: gameapi.hpp	gameapi.cpp engine.hpp	bitboard.hpp 	helpFuncs.hpp 	consts.hpp
	$(CPP) $(CPPFLAGS) $(PYBIND11_INCLUDES) $^ -c
	
bitboard.o: bitboard.hpp	bitboard.cpp 	consts.hpp
	$(CPP) $(CPPFLAGS) $^ -c
	
helpFuncs.o: helpFuncs.cpp 	helpFuncs.hpp 	consts.hpp
	$(CPP) $(CPPFLAGS) $^ -c

clean:
	rm -f *.o
	rm -f *.gch
