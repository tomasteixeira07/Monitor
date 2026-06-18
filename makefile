CXX = g++
CXXFLAGS = -Wall -Wextra -O3 -std=c++11
LDFLAGS = -lncurses
TARGET = monitor
SRC = monitor.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)
	strip -s $(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all clean