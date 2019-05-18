CXXFLAGS=-O3 -Wall -std=gnu++11

SRC= $(wildcard *.cpp)
OBJ= $(SRC:.cpp=.o)

all: WL

WL: $(OBJ)
	@echo "Linking..."
	$(CXX) $(LDFLAGS) -o $@ $^
	
%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

install:
	install -m 755 WL /usr/bin
	
clean:
	rm *.o
	rm WL
