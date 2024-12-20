CXX = g++
CXXFLAGS = -O2 -std=c++11 -fopenmp
TARGET = matrix_power_openmp
SRCS = main.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

run:
	./$(TARGET) ../../in/small.txt 1000000 1 2 16
