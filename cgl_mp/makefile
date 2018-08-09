CFLAGS = -std=c++17 -fopenmp
CC = g++
DEBUG = -g -Wall -Wpedantic -DDEBUG

default: cgl_mp

cgl_mp: cgl_mp.cpp partition.hpp
	$(CC) cgl_mp.cpp -o cgl_mp $(CFLAGS) -DPARALLEL
seq: cgl_mp.cpp partition.hpp
	$(CC) cgl_mp.cpp -o cgl_mp $(CFLAGS)
debug:
	$(CC) cgl_mp.cpp -o cgl_mp $(CFLAGS) $(DEBUG) -DPARALLEL
clean:
	rm -vf ./cgl_mp
