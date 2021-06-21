INC = $(shell python3 -m pybind11 --includes)
LINK.o = $(LINK.cpp)
CPP=c++

engine: bitboard.o	gameapi.o
	$(CPP) $^ $(INC) -o $@

gameapi.o: bitboard.o 	gameapi.hpp	gameapi.cpp
	$(CPP) $^ $(INC) -c
	
bitboard.o: bitboard.cpp	bitboard.hpp
	$(CPP) $^ $(INC) -c

clean:
	rm -f *.o
	rm -f *.gch
	
cclean: engine	clean

run: engine
	./engine

runclean: run 	clean
