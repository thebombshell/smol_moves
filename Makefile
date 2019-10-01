
C = gcc
GDB = gdb

CFLAGS = -std=c89 -Wall -Werror -Wfatal-errors

SOURCES := $(wildcard *.c)
OBJECTS := $(patsubst %.c, objects/%.o, $(SOURCES))

INCLUDE :=
LDFLAGS := -lopengl32 -lgdi32 -shared-libgcc
GDBFLAGS = -cd ./output/

DEBUG = FALSE
ifeq ($(DEBUG), TRUE)
DEFINES =
OPTIMIZE = -g
else
DEFINES = -DNDEBUG
OPTIMIZE = -Os
endif

all: clean build

build: $(OBJECTS) main/main.c main/main.h
	$(C) $(DEFINES) $(OPTIMIZE) $(CFLAGS) $(OBJECTS) $(INCLUDE) -Imain main/main.c -o output/main.exe $(LDFLAGS) -s

./objects/%.o: ./%.c  ./%.h
	$(C) $(DEFINES) $(CFLAGS) $(INCLUDE) -c $< -o $@

clean:
	-rm ./objects/*.o ./output/*.exe -f
	-del .\\objects\\*.o .\\output\\*.exe /F /Q
	
debug:
	$(GDB) $(GDBFLAGS) main.exe