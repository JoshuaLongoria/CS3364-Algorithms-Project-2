
CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -Werror -O2
SOURCES = graph.c courses.c

all: project2 test_graph

project2: project2.c $(SOURCES) graph.h courses.h
	$(CC) $(CFLAGS) -o $@ project2.c $(SOURCES)

test_graph: test_graph.c $(SOURCES) graph.h courses.h
	$(CC) $(CFLAGS) -o $@ test_graph.c $(SOURCES)

test: test_graph
	./test_graph

sanitize:
	$(CC) -std=c11 -Wall -Wextra -Wpedantic -g -fsanitize=address,undefined -fno-omit-frame-pointer -o test_graph_san test_graph.c $(SOURCES)
	./test_graph_san

clean:
	rm -f project2 test_graph test_graph_san

.PHONY: all test sanitize clean
