
C = gcc
GDB = gdb

CFLAGS = -std=c89 -static-libgcc -Wall -Werror -Wfatal-errors

SOURCES := $(wildcard *.c)
OBJECTS := $(patsubst %.c, %.o, $(SOURCES))
INCLUDE :=
LDFLAGS := -lopengl32 -lgdi32
GDBFLAGS = -cd ./

all: main

main: $(OBJECTS)
	$(C) -Os $(CFLAGS) $(SOURCES) $(INCLUDE) -o main $(LDFLAGS)

debug: $(OBJECTS)
	$(C) -g $(CFLAGS) $(SOURCES) $(INCLUDE) -o main $(LDFLAGS)

./%.o: ./%.c
	$(C) $(CFLAGS) $(INCLUDE) -c $< -o $@

clean: rm del

rm:
	-rm ./*.o ./*.exe -f

del:
	-del .\\*.o .\\*.exe /F /Q

run_debug:
	$(GDB) $(GDBFLAGS) main.exe