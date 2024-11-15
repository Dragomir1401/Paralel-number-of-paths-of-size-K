# Compiler
CXX = g++

# Compiler and Linker Flags
CXXFLAGS = -I"C:/Program Files (x86)/Microsoft SDKs/MPI/Include" -O2 -std=c++11
LDFLAGS = -L"C:/Program Files (x86)/Microsoft SDKs/MPI/Lib/x64" -lmsmpi

# Target executable
TARGET = matrix.exe
SRC = matrix.cpp

# Build rule
all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) $(LDFLAGS) -o $(TARGET)

# Clean rule
clean:
	rm -f $(TARGET)
