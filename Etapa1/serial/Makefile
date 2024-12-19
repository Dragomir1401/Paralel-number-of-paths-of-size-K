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

# Clean up the build
clean:
	$(RM) $(TARGET)
