CC=g++
CFLAGS=-std=c++20 -I./includes -g

SRCDIR=src
SOURCES=$(wildcard $(SRCDIR)/*.cpp)
EXECUTABLE=temple

all: $(EXECUTABLE)

$(EXECUTABLE): $(SOURCES)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f $(EXECUTABLE)

run: all
	./$(EXECUTABLE)
