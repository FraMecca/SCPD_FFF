DEBUG=-g -Wall -Wpedantic -Wextra
PROF=-pg
CC=g++
SOURCES=seq_main.cpp
CFLAGS=-std=c++17 -I. -I../libcgl -lboost_serialization

default:
	$(CC) $(SOURCES) -o seq $(CFLAGS)
opt:
	$(CC) $(SOURCES) -o seq $(CFLAGS) -O2
debug:
	$(CC) $(SOURCES) -o seq_dbg $(CFLAGS) $(DEBUG)
prof:
	$(CC) $(SOURCES) -o seq_prof $(CFLAGS) $(PROF)
profdebug:
	$(CC) $(SOURCES) -o seq_profdbg $(CFLAGS) $(DEBUG) $(PROF)
valgrind: debug
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes ./seq_dbg
callgrind: default
	rm -f callgrind.out.*
	valgrind --dsymutil=yes --tool=callgrind ./seq
	gprof2dot --format=callgrind --output=out.dot ./callgrind.out.*
	dot -Tsvg out.dot -o graph.svg
clean:
	rm -f callgrind.out.*
	rm -f out.dot
	rm -f graph.svg
	rm -f gmon.out
	rm -f seq seq_dbg seq_prof seq_profdbg




