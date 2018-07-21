
C = gcc
GDB = gdb

CFLAGS = -std=c89 -Wall -Werror -Wfatal-errors

SOURCES := $(wildcard *.c)
OBJECTS_FOR_WIN := $(patsubst %.c, windows/objects/%.o, $(SOURCES))

WIN_SOURCES := $(wildcard windows/*.c)
WIN_OBJECTS := $(patsubst windows/%.c, windows/objects/%.o, $(WIN_SOURCES))

RESOURCES := $(wildcard resources/*)
RESOURCES_FILES := $(patsubst resources/%, %, $(RESOURCES))


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

build: windows resources docs

docs:
	@doxygen doxygen/smol_config

windows: $(OBJECTS_FOR_WIN) $(WIN_OBJECTS)
	$(C) $(DEFINES) $(OPTIMIZE) $(CFLAGS) $(OBJECTS_FOR_WIN) $(WIN_OBJECTS) $(INCLUDE) -o output/smol_windows $(LDFLAGS) -s
	
resources: $(RESOURCES_FILES)

%: ./resources/%
	xcopy .\\resources\\$@ .\\output\\$@ /f /y
	
./windows/objects/%.o: ./%.c  ./%.h
	$(C) $(DEFINES) $(CFLAGS) $(INCLUDE) -c $< -o $@

./windows/objects/%.o: ./windows/%.c ./windows/%.h
	$(C) $(DEFINES) $(CFLAGS) $(INCLUDE) -c $< -o $@

./windows/objects/smol.o: ./smol.h ./smol.c ./windows/smol.inl
	$(C) $(DEFINES) $(CFLAGS) $(INCLUDE) -c ./smol.c -o $@

./windows/objects/main.o: ./windows/main.c
	$(C) $(DEFINES) $(CFLAGS) $(INCLUDE) -c $< -o $@
	
clean: clean_windows

clean_windows:
	-rm ./windows/objects/*.o ./windows/output/*.exe -f
	-del .\\windows\\objects\\*.o .\\windows\\output\\*.exe /F /Q
	
debug:
	$(GDB) $(GDBFLAGS) smol_windows.exe