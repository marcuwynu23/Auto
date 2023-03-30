# constants variable
BIN=bin/auto.exe


# icon resource 
ICO=resources/icon.rc
RES=resources/resource.res

# source files
SRC=$(wildcard src/*.cpp)

# compiling source files
all: compile_icon compile_source

# running executable
run: $(BIN)
	$(BIN)

# compiling source files
compile_source: $(BIN)

$(BIN): $(SRC)
	c++ $(CXXFLAGS) $(LDFLAGS) $(RES) -o $@ $^

# compiling icon resource
compile_icon: $(RES)

$(RES): $(ICO)
	windres $^ -O coff -o $@

# cleaning executable
clean:
	del $(BIN) $(RES)
