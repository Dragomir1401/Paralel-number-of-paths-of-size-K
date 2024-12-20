ifeq ($(OS),Windows_NT)
    CXX = g++
    CXXFLAGS = -I"C:/Program Files (x86)/Microsoft SDKs/MPI/Include" -O2 -std=c++11
    LDFLAGS = -L"C:/Program Files (x86)/Microsoft SDKs/MPI/Lib/x64" -lmsmpi
    TARGET = matrix.exe
else
    CXX = mpic++
    CXXFLAGS = -O2 -std=c++11
    LDFLAGS = -lmpi
    TARGET = matrix
endif

SRC = matrix.cpp
NPROCS ?= 8
ARGS = ../../in/small.txt 1000000 1 2

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) $(LDFLAGS) -o $(TARGET)

.PHONY: run
run: $(TARGET)
	@mpirun --oversubscribe -np $(NPROCS) ./$(TARGET) $(ARGS)

.PHONY: clean
clean:
	rm -f matrix matrix.exe