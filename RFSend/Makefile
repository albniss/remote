CXXFLAGS=-O3 -Wall -std=gnu++11

SRC= $(wildcard *.cpp)
OBJ= $(SRC:.cpp=.o)

all: Lutron

Lutron: $(OBJ)
	@echo "Linking..."
	$(CXX) $(LDFLAGS) -o $@ $^
	
%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
	
clean:
	rm *.o
	rm Lutron