CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Ibase_structures -Isequences -Icomplex_sequences -Iui -Itests

SRCS = main.cpp ui/Menu.cpp tests/Tests.cpp complex_sequences/BitSequence.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = main

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)
