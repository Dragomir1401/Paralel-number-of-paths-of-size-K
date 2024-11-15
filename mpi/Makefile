# Detect the operating system
ifeq ($(OS),Windows_NT)
    # Windows settings
    CXX = g++
    CXXFLAGS = -I"C:/Program Files (x86)/Microsoft SDKs/MPI/Include" -O2 -std=c++11
    LDFLAGS = -L"C:/Program Files (x86)/Microsoft SDKs/MPI/Lib/x64" -lmsmpi
    TARGET = matrix.exe
else
    # Linux settings
    CXX = mpic++
    CXXFLAGS = -O2 -std=c++11
    LDFLAGS = -lmpi
    TARGET = matrix
endif

# Source file
SRC = matrix.cpp

# Default target
all: $(TARGET)

# Build target
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) $(LDFLAGS) -o $(TARGET)

# Clean target
clean:
	rm -f matrix matrix.exe
