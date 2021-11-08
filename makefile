INC = $(shell python3 -m pybind11 --includes)
SUFFIX = $(shell python3-config --extension-suffix)
LINK.o = $(LINK.cpp)
CPP=c++
CPPFLAGS = -std=c++20 -Wall -pedantic -O -fPIC 
LIB = -ltbb
LINK.o = $(LINK.cpp)

checkers: bitboard.o	gameapi.o 	helpFuncs.o 	engine.o
	$(LINK.o) -shared $(CPPFLAGS) $^ -o $@$(SUFFIX) $(LIB)
	
engine.o: engine.cpp 	engine.hpp 	helpFuncs.hpp 	consts.hpp
	$(CPP) $(CPPFLAGS) $^ -c

gameapi.o: gameapi.hpp	gameapi.cpp engine.hpp	bitboard.hpp 	helpFuncs.hpp 	consts.hpp
	$(CPP) $(CPPFLAGS) $(INC) $^ -c
	
bitboard.o: bitboard.hpp	bitboard.cpp 	consts.hpp
	$(CPP) $(CPPFLAGS) $^ -c
	
helpFuncs.o: helpFuncs.cpp 	helpFuncs.hpp 	consts.hpp
	$(CPP) $(CPPFLAGS) $^ -c

clean:
	rm -f *.o
	rm -f *.gch
