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
SRCS = main.cpp

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET) -lpthread

.PHONY: run
run: $(TARGET)
	./$(TARGET) ../../in/small.txt 1000000 1 2 16

.PHONY: clean
clean:
	$(RM) $(TARGET)