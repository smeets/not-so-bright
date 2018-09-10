BIN_NAME=not-so-bright.exe
BIN_OBJ=build\\main.obj
BIN_LIBS=User32.lib Gdi32.lib

all: $(BIN_NAME)

clean:
	del $(BIN_OBJ)
	del $(BIN_NAME)
	rmdir build


init:
	if not exist build mkdir build

$(BIN_NAME): init $(BIN_OBJ)
	link /nologo /out:$(BIN_NAME) $(BIN_OBJ) $(BIN_LIBS)

build\\main.obj: src\main.cpp
	cl /c /O2 /nologo /Fo:$@ src\\main.cpp