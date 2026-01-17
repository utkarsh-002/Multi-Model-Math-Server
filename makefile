# Compiler and Flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall

# Target Executables
TARGETS = server1 server2 server3

# Common Source for Math Logic
COMMON_SRC = Calculator.cpp
COMMON_HDR = Calculator.h

# Default rule: build all servers
all: $(TARGETS)

# Rule for Server 1 (Single Process)
server1: server1.cpp $(COMMON_SRC) $(COMMON_HDR)
	$(CXX) $(CXXFLAGS) server1.cpp $(COMMON_SRC) -o server1

# Rule for Server 2 (Multi-Process/Fork)
server2: server2.cpp $(COMMON_SRC) $(COMMON_HDR)
	$(CXX) $(CXXFLAGS) server2.cpp $(COMMON_SRC) -o server2

# Rule for Server 3 (I/O Multiplexing/Select)
server3: server3.cpp $(COMMON_SRC) $(COMMON_HDR)
	$(CXX) $(CXXFLAGS) server3.cpp $(COMMON_SRC) -o server3

# Clean up build files
clean:
	rm -f $(TARGETS)