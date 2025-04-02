CC = clang
CFLAGS = -O3 -lm -lz
TARGET = main
SOURCES = main.c

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES) -lz

clean:
	rm -f $(TARGET)