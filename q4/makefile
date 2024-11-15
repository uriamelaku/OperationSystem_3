# Makefile for graph_server

# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -pthread -std=c++11 -Wall -Wextra

# Target executable
TARGET = server

# Source files
SRCS = server.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Default target
all: $(TARGET)

# Link the target executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Compile the source files into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(TARGET) $(OBJS)

# Run the server
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run

# ./server
# nc localhost 9034