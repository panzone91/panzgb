SOURCES = $(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)
COMPILER_FLAGS = -c -g -std=c99 -Wall -Wextra -pedantic
EXECUTABLE = panzgb

CC = clang
INCLUDE_PATHS = -I/opt/homebrew/include
LIBRARY_PATHS = -L/opt/homebrew/lib
LINKER_FLAGS = -lSDL2

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LIBRARY_PATHS) $(LINKER_FLAGS) -o $@
.c.o:
	$(CC) $(INCLUDE_PATHS) $(COMPILER_FLAGS) $< -o $@

clean:
	@rm *.o

clobber:
	@rm *.o
	@rm panzgb
