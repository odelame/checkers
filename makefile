INC = $(shell python3 -m pybind11 --includes)
SUFFIX = $(shell python3-config --extension-suffix)
LINK.o = $(LINK.cpp)
CPP=c++
CPPFLAGS = -std=c++20 -Wall -pedantic -O -fPIC 
LIB = -ltbb

checkers: bitboard.o	gameapi.o 	helpFuncs.o 	engine.o 	timer.o
	$(CPP) $(CPPFLAGS) $(INC) $^ -shared -o $@$(SUFFIX) $(LIB)
	
engine.o: engine.cpp 	engine.hpp 	helpFuncs.hpp
	$(CPP) $(CPPFLAGS) $(INC) $^ -c

gameapi.o: gameapi.hpp	gameapi.cpp engine.hpp	bitboard.hpp 	helpFuncs.hpp
	$(CPP) $(CPPFLAGS) $(INC) $^ -c
	
bitboard.o: bitboard.hpp	bitboard.cpp
	$(CPP) $(CPPFLAGS) $(INC) $^ -c
	
helpFuncs.o: helpFuncs.cpp 	helpFuncs.hpp
	$(CPP) $(CPPFLAGS) $(INC) $^ -c
	
timer.o: timer/timer.cpp 	timer/timer.hpp
	$(CPP) $(CPPFLAGS) $(INC) $^ -c
	

clean:
	rm -f *.o
	rm -f *.gch
	
cclean: engine	clean

run: engine
	./engine

runclean: run 	clean
