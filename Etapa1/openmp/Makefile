# Makefile for OpenMP program

# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -O2 -std=c++11 -fopenmp

# Target executable
TARGET = matrix_power_openmp

# Source files
SRCS = main.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Default rule
all: $(TARGET)

# Build the target executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile source files into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -f $(OBJS) $(TARGET)

# Run the program with default parameters
run:
	./$(TARGET) ../../in/large.txt 1000000 1 2 16
