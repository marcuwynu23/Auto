#constants variable
BIN=bin/auto.exe
# icon resource 
ICO=resources/icon.rc
RES=resources/resource.res
SRC=$(wildcard src/*.cpp)
SRC_LIB=$(filter-out src/main.cpp,$(SRC))
SRC_TEST=$(wildcard test/*.cpp)
# compiling source file
all: compile_icon compile_source
# running executable
run:
	$(BIN)
	
compile_source:
	c++ -std=c++11 $(RES) -o $(BIN)  -I include $(SRC) 

compile_icon:
	windres $(ICO) -O coff -o $(RES)

# Compile test files
test:
	c++ -std=c++11 -I include -o test.exe $(SRC_TEST) $(SRC_LIB)

# Run tests
test-run: test
	@echo "Running tests..."
	./test.exe

experimental:
	c++ -std=c++11  -o test.exe  $(SRC_TEST)

# cleaning executable
clean:
	del $(BIN)