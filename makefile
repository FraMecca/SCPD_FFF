CC = g++
CFLAGS = -std=c++17
PROF = -pg
DEBUG = -g -Wall -Wpedantic -DDEBUG
MPCFLAGS = -std=c++17 -fopenmp

default: bin/cgl # sequential

# sequential
bin/cgl: objs/cgl_main.o objs/cgl.o
	$(CC) $(CFLAGS) -o bin/cgl objs/cgl_main.o

# parallel: partitioning + openmp
bin/cgl_mp: objs/cgl_mp.o objs/cgl.o
	$(CC) $(MPCFLAGS) -o bin/cgl_mp objs/cgl_mp.o -DPARALLEL

# object files for sequential
objs/cgl_main.o: ./cgl_seq/cgl_main.cpp ./cgl/cgl.hpp
	$(CC) $(CFLAGS) -c ./cgl_seq/cgl_main.cpp -o ./objs/cgl_main.o

# object files for cgl library
objs/cgl.o: ./cgl/cgl_eval.hpp ./cgl/cgl.hpp
	$(CC) $(CFLAGS) -c ./cgl/cgl_eval.hpp -o ./objs/cgl.o

# object files for parallel
objs/cgl_mp.o: cgl_mp/cgl_mp.cpp cgl_mp/partition.hpp
	$(CC) $(MPCFLAGS) -c cgl_mp/cgl_mp.cpp -o objs/cgl_mp.o -DPARALLEL

prof: ./cgl/cgl_eval.hpp ./cgl/cgl.hpp
	mkdir ./bin/prof -p
	$(CC) $(CFLAGS) -c ./cgl/cgl_eval.hpp -o objs/cgl.o $(PROF)
	$(CC) $(CFLAGS) -c ./cgl_seq/cgl_main.cpp -o ./objs/cgl_main.o $(PROF)
	$(CC) $(CFLAGS) -o ./bin/prof/cgl objs/cgl.o objs/cgl_main.o $(PROF)
	$(CC) $(MPCFLAGS) -o ./bin/prof/cgl_mp objs/cgl.o objs/cgl_mp.o $(PROF)

debug: ./cgl/cgl_eval.hpp ./cgl/cgl.hpp
	mkdir ./bin/debug -p
	$(CC) $(CFLAGS) -c ./cgl/cgl_eval.hpp -o ./objs/cgl.o $(DEBUG)
	$(CC) $(CFLAGS) -c ./cgl_seq/cgl_main.cpp -o ./objs/cgl_main.o $(DEBUG) 
	$(CC) $(CFLAGS) objs/cgl.o objs/cgl_main.o -o bin/debug/cgl $(DEBUG) 
	$(CC) $(MPCFLAGS) -o ./bin/debug/cgl_mp objs/cgl.o objs/cgl_mp.o $(DEBUG) 

test:
	$(CC) $(CFLAGS) -c cgl/cgl_test.cpp -o objs/cgl_test.o $(DEBUG)
	$(CC) $(CFLAGS) -o bin/cgl_test objs/cgl_test.o $(DEBUG)

clean:
	rm objs/*.o
	rm bin/* -rf
