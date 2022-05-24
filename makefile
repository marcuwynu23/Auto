#constants variable
BIN=bin/auto.exe
SRC=$(wildcard src/*.cpp)

# compiling source file
all:
	c++ -std=c++11 -o $(BIN)  -I include $(SRC) 

# running executable
run:
	$(BIN)


# cleaning executable
clean:
	del $(BIN)
