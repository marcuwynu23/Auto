# LLVM/Clang makefile for Auto (used by CI)
BIN=bin/auto.exe
ICO=resources/icon.rc
RES=resources/resource.res
SRC=$(wildcard src/*.cpp)
SRC_LIB=$(filter-out src/main.cpp,$(SRC))
SRC_TEST=$(wildcard test/*.cpp)
CXX=clang++
CXXFLAGS=-std=c++20 -Wall -Wextra -Wpedantic
RC=windres

all: compile_icon compile_source

run:
	$(BIN)

compile_source:
	$(CXX) $(CXXFLAGS) $(RES) -o $(BIN) -I include $(SRC)

compile_icon:
	$(RC) $(ICO) -O coff -o $(RES)

test:
	$(CXX) $(CXXFLAGS) -I include -o test.exe $(SRC_TEST) $(SRC_LIB)

test-run: test
	@echo "Running tests..."
	./test.exe

clean:
	-del $(BIN) $(RES) test.exe 2>nul
