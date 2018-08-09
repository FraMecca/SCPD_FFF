CC = g++
CFLAGS = -std=c++17
PROF = -pg
DEBUG = -g

default: bin/cgl

bin/cgl: objs/cgl_main.o objs/cgl.o
	$(CC) $(CFLAGS) -o bin/cgl objs/cgl_main.o

objs/cgl_main.o: ./cgl_seq/cgl_main.cpp ./cgl/cgl.hpp
	$(CC) $(CFLAGS) -c ./cgl_seq/cgl_main.cpp -o ./objs/cgl_main.o

objs/cgl.o: ./cgl/cgl.cpp ./cgl/cgl.hpp
	$(CC) $(CFLAGS) -c ./cgl/cgl.cpp -o ./objs/cgl.o

prof: ./cgl/cgl.cpp ./cgl/cgl.hpp
	mkdir ./bin/prof -p
	$(CC) $(CFLAGS) -c ./cgl/cgl.cpp -o objs/cgl.o $(PROF)
	$(CC) $(CFLAGS) -c ./cgl_seq/cgl_main.cpp -o ./objs/cgl_main.o $(PROF)
	$(CC) $(CFLAGS) -o ./bin/prof/cgl objs/cgl.o objs/cgl_main.o $(PROF)

debug: ./cgl/cgl.cpp ./cgl/cgl.hpp
	mkdir ./bin/debug -p
	$(CC) $(CFLAGS) -c ./cgl/cgl.cpp $(DEBUG) -o ./objs/cgl.o
	$(CC) $(CFLAGS) -c ./cgl_seq/cgl_main.cpp -o ./objs/cgl_main.o $(DEBUG)
	$(CC) $(CFLAGS) -o ./bin/debug/cgl objs/cgl.o objs/cgl_main.o $(DEBUG) 

test:
	$(CC) $(CFLAGS) -c cgl/cgl_test.cpp -o objs/cgl_test.o $(DEBUG)
	$(CC) $(CFLAGS) -o bin/cgl_test objs/cgl_test.o $(DEBUG)

clean:
	rm objs/*.o
	rm bin/* -rf
