CC=zig cc
CFLAGS=-I/opt/homebrew/Cellar/raylib/5.0/include
LFLAGS=-L/opt/homebrew/Cellar/raylib/5.0/lib -lraylib -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL
TARGET=Snake
SOURCE=main.c

# Default target to clean, build, and run your application
all: clean build run

# Target to build your application
build: $(SOURCE)
	$(CC) $(CFLAGS) $(LFLAGS) -o $(TARGET) $(SOURCE)

# Target to clean up the previous build
clean:
	rm -f $(TARGET)

# Target to run your application
run:
	./$(TARGET)

