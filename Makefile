BIN_NAME=not-so-bright.exe
BIN_OBJ=dist\\main.obj
BIN_LIBS=User32.lib Gdi32.lib

all: $(BIN_NAME)

clean:
	del $(BIN_OBJ)
	del $(BIN_NAME)
	rmdir dist


init:
	if not exist dist mkdir dist

$(BIN_NAME): init $(BIN_OBJ)
	link /nologo /out:$(BIN_NAME) $(BIN_OBJ) $(BIN_LIBS)

dist\\main.obj: src\main.cpp
	cl /c /O2 /nologo /Fo:$@ src\\main.cpp