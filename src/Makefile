CC=g++
#CC=clang
CFLAGS=-I./inc -O3
#CFLAGS=-I./inc -ggdb -g3 -O0 -DDEBUG -pg
LDFLAGS=-lboost_system -lboost_filesystem -pg
LD=g++
SOURCES=main.cpp worker.cpp huffman.cpp lwz.cpp reader.cpp
OBJECTS=$(SOURCES:.cpp=.o)
BIN=compress

all: $(BIN) $(SOURCES)

$(BIN): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS)

$(OBJECTS): %.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm *.o $(BIN)
