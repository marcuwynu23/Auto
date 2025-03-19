# Constants
BIN = bin/auto.exe
ICO = resources/icon.rc
RES = resources/resource.res
SRC = $(wildcard src/*.cpp)
SRC_TEST = $(wildcard test/*.cpp)
INCLUDE = -I include

# Compiler
CXX = clang++
CXXFLAGS = -std=c++20 -Wall -Wextra -Wpedantic
RC = windres

# Ensure output directory exists
all: create_dirs compile_icon compile_source

# Run the executable
run: $(BIN)
	$(BIN)

# Compile source files
compile_source: $(SRC)
	$(CXX) $(CXXFLAGS) $(RES) -o $(BIN) $(INCLUDE) $(SRC)

# Compile icon resource
compile_icon: $(ICO)
	$(RC) $(ICO) -O coff -o $(RES)

# Compile experimental test files
experimental: $(SRC_TEST)
	$(CXX) $(CXXFLAGS) -o test.exe $(SRC_TEST)

# Create bin directory if not exists
create_dirs:
	@mkdir -p bin

# Clean build files
clean:
	rm -f $(BIN) test.exe $(RES)
