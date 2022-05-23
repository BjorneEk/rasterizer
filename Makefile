TARGET = rasterizer

CC = gcc
INCLUDE_DIR = src/include
CFLAGS = -F /Library/Frameworks -I$(INCLUDE_DIR)
LIBS = -framework SDL2
C_SOURCES = $(wildcard src/*.c *.c )
DEPS = $(wildcard $(INCLUDE_DIR)/*.h *.h src/*.h)
OBJ = ${C_SOURCES:.c=.o}

# First rule is the one executed when no parameters are fed to the Makefile


%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBS)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

run: $(TARGET)
	./$(TARGET)

clean:
	$(RM) src/*.bin src/*.o src/*.dis src/*.elf
	$(RM) lib/*.o
