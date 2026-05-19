CXX := clang++
CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic -Iinclude
TARGET := cvmpp
SRC := $(wildcard src/*.cpp)
OBJ := $(SRC:.cpp=.o)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $@

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)
