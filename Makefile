CC=g++
CFLAGS=-std=c++20 -I./includes -pthread -g

SRCDIR=src
SOURCES=$(wildcard $(SRCDIR)/*.cpp)
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=temple
SHARED_LIB=libtemple.so
STATIC_LIB=libtemple.a

all: $(STATIC_LIB)

$(SHARED_LIB): $(OBJECTS)
	$(CC) -shared $(CFLAGS) -o $@ $^

$(STATIC_LIB): $(OBJECTS)
	ar rcs $@ $^

$(EXECUTABLE): $(SOURCES)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(EXECUTABLE)

run: all
	./$(EXECUTABLE)
