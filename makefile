#constants variable
BIN=bin/auto.exe
# icon resource 
ICO=resources/icon.rc
RES=resources/resource.res
SRC=$(wildcard src/*.cpp)

# compiling source file
all: compile_icon compile_source
# running executable
run:
	$(BIN)
	
compile_source:
	c++ -std=c++11 $(RES) -o $(BIN)  -I include $(SRC) 

compile_icon:
	windres $(ICO) -O coff -o $(RES)


# cleaning executable
clean:
	del $(BIN)