CXXFLAGS=-O3 -Wall -std=gnu++11

SRC= $(wildcard *.cpp)
OBJ= $(SRC:.cpp=.o)

all: RFSend

RFSend: $(OBJ)
	@echo "Linking..."
	$(CXX) $(LDFLAGS) -o $@ $^
	
%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

install:
	install -m 755 RFSend /usr/bin
	
clean:
	rm *.o
	rm RFSend
