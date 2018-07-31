CC = g++
CFLAGS = -std=c++11
PROF = -pg
DEBUG = -g

default: cgl

cgl: cgl.o
	$(CC) $(CFLAGS) -o cgl cgl.o
cgl.o: cgl.cpp cgl.hpp
	$(CC) $(CFLAGS) -c cgl.cpp
prof: cgl.cpp cgl.hpp
	mkdir ./prof
	$(CC) $(CFLAGS) -c cgl.cpp $(PROF)
	$(CC) $(CFLAGS) -o ./prof/cgl cgl.o $(PROF)
debug: cgl.cpp cgl.hpp
	mkdir ./debug
	$(CC) $(CFLAGS) -c cgl.cpp $(DEBUG)
	$(CC) $(CFLAGS) -o ./debug/cgl cgl.o $(DEBUG)
clean:
	rm -f cgl *.o *~
	rm -rf ./prof
	rm -rf ./debug
