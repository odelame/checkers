INC = $(shell python3 -m pybind11 --includes)
SUFFIX = $(shell python3-config --extension-suffix)
LINK.o = $(LINK.cpp)
CPP=c++
CPPFLAGS = -std=c++20 -Wall -pedantic -Ofast -fPIC 
LIB = -ltbb -lpthread
LINK.o = $(LINK.cpp)

checkers: bitboard.o	gameapi.o 	helpFuncs.o 	engine.o 	timer.o
	$(LINK.o) $^ -fPIC -shared -o $@$(SUFFIX) $(LIB)
	
engine.o: engine.cpp 	engine.hpp 	helpFuncs.hpp
	$(CPP) $(CPPFLAGS) $^ -c

gameapi.o: gameapi.hpp	gameapi.cpp engine.hpp	bitboard.hpp 	helpFuncs.hpp
	$(CPP) -std=c++20 -Wall -pedantic -O -fPIC $(INC) $^ -c
	
bitboard.o: bitboard.hpp	bitboard.cpp
	$(CPP) $(CPPFLAGS) $^ -c
	
helpFuncs.o: helpFuncs.cpp 	helpFuncs.hpp
	$(CPP) $(CPPFLAGS) $^ -c

timer.o: timer.hpp 	timer.cpp
	$(CPP) $(CPPFLAGS) $^ -c

clean:
	rm -f *.o
	rm -f *.gch
	
cclean: engine	clean

run: engine
	./engine

runclean: run 	clean
