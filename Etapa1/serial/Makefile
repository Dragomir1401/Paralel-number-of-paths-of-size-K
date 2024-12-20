ifeq ($(OS),Windows_NT)
    CXX = g++
    TARGET = matrix.exe
    RM = del /Q
else
    CXX = g++
    TARGET = matrix
    RM = rm -f
endif

CXXFLAGS = -std=c++11 -Wall
SRCS = matrix.cpp

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)

.PHONY: run
run: $(TARGET)
	./$(TARGET) ../../in/small.txt 1000000 1 2

.PHONY: clean
clean:
	$(RM) $(TARGET)