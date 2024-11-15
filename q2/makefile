CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pg

SRCS = main.cpp Kosaraju.cpp KosarajuDeque.cpp KosarajuList.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = kosaraju_test

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

profile: $(TARGET)
	./$(TARGET)
	gprof $(TARGET) gmon.out > profile_report.txt

clean:
	rm -f $(OBJS) $(TARGET) gmon.out profile_report.txt

.PHONY: all clean profile
