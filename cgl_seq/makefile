CC = g++
CFLAGS = -std=c++11

default: cgl

cgl: cgl.o
	$(CC) $(CFLAGS) -o cgl cgl.o
cgl.o: cgl.cpp cgl.hpp
	$(CC) $(CFLAGS) -c cgl.cpp
clean:
	rm -f cgl *.o *~

