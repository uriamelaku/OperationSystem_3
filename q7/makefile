CXX = g++
CXXFLAGS = -pthread -std=c++11 -Wall -Wextra
TARGET = server
SRCS = server.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: all clean run

# ./server
# nc localhost 9034