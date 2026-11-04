CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -O2

.PHONY: all test clean

all: project2.exe test_graph.exe

project2.exe: project2.c courses.c graph.c courses.h graph.h
	$(CC) $(CFLAGS) -o $@ project2.c courses.c graph.c

test_graph.exe: test_graph.c courses.c graph.c courses.h graph.h
	$(CC) $(CFLAGS) -o $@ test_graph.c courses.c graph.c

test: test_graph.exe
	./test_graph.exe

clean:
	$(RM) project2.exe test_graph.exe
