INC = $(shell python3 -m pybind11 --includes)
LINK.o = $(LINK.cpp)
CPP=c++

engine: engine.o
	$(CPP) $^ $(INC) -o $@
	
engine.o: engine.hpp	engine.cpp
	$(CPP) $^ $(INC) -c

clean:
	rm -f *.o
	rm -f *.gch
