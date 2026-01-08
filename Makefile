CXX = g++
CXXFLAGS = -std=c++17 -Wall -g
SRC = src/main.cpp src/allocator/MemoryManager.cpp src/cache/Cache.cpp src/virtual_memory/VirtualMemory.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = memsim

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ)

clean:
	rm -f $(TARGET) src/*.o src/allocator/*.o src/cache/*.o src/virtual_memory/*.o