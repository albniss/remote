CXXFLAGS=-O3 -Wall -std=gnu++11
LDFLAGS=-static-libgcc -static-libstdc++

SRC= $(wildcard *.cpp)
OBJ= $(SRC:.cpp=.o)

all: Lutron.exe

Lutron.exe: $(OBJ)
	@echo "Linking..."
	$(CXX) $(LDFLAGS) -o $@ $^
	
%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
	
clean:
	rm *.o
	rm Lutron