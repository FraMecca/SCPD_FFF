CC = g++
CFLAGS = -std=c++11
PROF = -pg
DEBUG = -g

default: cgl

cgl: cgl_main.o
	$(CC) $(CFLAGS) -o cgl cgl_main.o cgl.o
cgl_main.o: cgl_main.cpp cgl.hpp
	$(CC) $(CFLAGS) -c cgl_main.cpp
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
test: cgl_test.o
	$(CC) $(CFLAGS) -o cgl_test cgl_test.o
cgl_test.o: cgl_test.cpp catch.hpp
	$(CC) $(CFLAGS) -c cgl_test.cpp
clean:
	rm -rf cgl cgl_test *.o *~
	rm -rf ./prof
	rm -rf ./debug
