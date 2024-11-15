CXX = g++
CXXFLAGS = -pthread -std=c++11
TARGET = server
SRCS = server.cpp proactor.cpp
HEADERS = server.hpp proactor.hpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: all clean run
