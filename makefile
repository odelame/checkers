INC = $(shell python3 -m pybind11 --includes)
LINK.o = $(LINK.cpp)
CPP=c++
CPPFLAGS = -std=c++20 -Wall -pedantic -Ofast

engine: bitboard.o	gameapi.o
	$(CPP) $(CPPFLAGS) $^ -o $@

gameapi.o: gameapi.hpp	gameapi.cpp 	bitboard.hpp	consts.hpp
	$(CPP) $(CPPFLAGS) $^ -c
	
bitboard.o: bitboard.hpp	bitboard.cpp 	gameapi.hpp	consts.hpp
	$(CPP) $(CPPFLAGS) $^ -c

clean:
	rm -f *.o
	rm -f *.gch
	
cclean: engine	clean

run: engine
	./engine

runclean: run 	clean
