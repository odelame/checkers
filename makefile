INC = $(shell python3 -m pybind11 --includes)
LINK.o = $(LINK.cpp)
CPP=c++

engine: bitboard.o
	$(CPP) $^ $(INC) -o $@
	
bitboard.o: bitboard.hpp	bitboard.cpp
	$(CPP) $^ $(INC) -c

clean:
	rm -f *.o
	rm -f *.gch
	
cclean: engine	clean

run: engine
	./engine

runclean: run 	clean
