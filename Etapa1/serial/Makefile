# Makefile for Serial Program

# Detect the operating system
ifeq ($(OS),Windows_NT)
    # Windows settings
    CXX = g++
    TARGET = matrix.exe
    RM = del /Q
else
    # Linux settings
    CXX = g++
    TARGET = matrix
    RM = rm -f
endif

# Compiler flags
CXXFLAGS = -std=c++11 -Wall

# Source files
SRCS = matrix.cpp

# Default target
all: $(TARGET)

# Build the executable
$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)

# Run the program with default parameters
.PHONY: run
run: $(TARGET)
	./$(TARGET) ../../in/small.txt 1000000 1 2

# Clean up the build
.PHONY: clean
clean:
	$(RM) $(TARGET)