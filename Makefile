CXX = clang++
CXXFLAGS = -std=c++20 -Wall -Wextra

SRC = nostermus.cpp
OBJ = nostermus.o
TARGET = libnostermus.a

all: $(TARGET)

$(TARGET): $(OBJ)
	ar rcs $(TARGET) $^
	@rm $(OBJ)

$(OBJ): $(SRC)
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -rf $(TARGET) $(OBJ)

.PHONY: all clean
