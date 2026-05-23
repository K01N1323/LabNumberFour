CXX = clang++
CXXFLAGS = -std=c++17 -Wall -I.

SRCS = main.cpp ComplexSequences/BitSequence.cpp tests/Tests.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = main

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)
