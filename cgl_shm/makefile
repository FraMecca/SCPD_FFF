DEBUG=-g -Wall -Wpedantic -Wextra
PROF=-pg
CC=g++
CFLAGS=-std=c++11 -fopenmp
SOURCES=shm_main.cpp

default:
	$(CC) $(SOURCES) -o shm $(CFLAGS)
opt:
	$(CC) $(SOURCES) -o shm $(CFLAGS) -O2
debug:
	$(CC) $(SOURCES) -o shm_dbg $(CFLAGS) $(DEBUG)
prof:
	$(CC) $(SOURCES) -o shm_prof $(CFLAGS) $(PROF)
profdebug:
	$(CC) $(SOURCES) -o shm_profdbg $(CFLAGS) $(DEBUG) $(PROF)
valgrind: debug
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes ./shm_dbg
callgrind: default
	rm -f callgrind.out.*
	valgrind --dsymutil=yes --tool=callgrind ./shm
	gprof2dot --format=callgrind --output=out.dot ./callgrind.out.*
	dot -Tsvg out.dot -o graph.svg
clean:
	rm -f callgrind.out.*
	rm -f out.dot
	rm -f graph.svg
	rm -f gmon.out
	rm -f shm shm_dbg shm_prof shm_profdbg




