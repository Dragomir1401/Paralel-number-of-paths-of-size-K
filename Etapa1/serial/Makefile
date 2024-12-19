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

clean:
	$(RM) $(TARGET)
