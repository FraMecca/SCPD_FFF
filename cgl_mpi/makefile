CC = mpicxx
CFLAGS=-std=c++17 -I. -I../libcgl -L/usr/lib64/mpich/lib/ -lboost_mpi-mt -lboost_serialization -fopenmp
SOURCES = mpi_main.cpp
DEBUG=-g -Wall 
PROF=-pg
OUT = mpi_cgl

default:
	$(CC) $(SOURCES) -o mpi $(CFLAGS)
opt:
	$(CC) $(SOURCES) -o mpi $(CFLAGS) -O2
debug:
	$(CC) $(SOURCES) -o mpi_dbg $(CFLAGS) $(DEBUG)
prof:
	$(CC) $(SOURCES) -o mpi_prof $(CFLAGS) $(PROF)
profdebug:
	$(CC) $(SOURCES) -o mpi_profdbg $(CFLAGS) $(DEBUG) $(PROF)
valgrind: debug
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes ./mpi_dbg
callgrind: default
	rm -f callgrind.out.*
	valgrind --dsymutil=yes --tool=callgrind ./mpi
	gprof2dot --format=callgrind --output=out.dot ./callgrind.out.*
	dot -Tsvg out.dot -o graph.svg
clean:
	rm -f callgrind.out.*
	rm -f out.dot
	rm -f graph.svg
	rm -f gmon.out
	rm -f mpi mpi_dbg mpi_prof mpi_profdbg

aldinuc:
	/home/ferraris/openmpi/bin/mpic++ -O2 -std=c++17 -I. -I../libcgl -L/home/ferraris/openmpi/lib/ -L/home/ferraris/boosto/lib/ -lboost_mpi  -lboost_serialization -fopenmp -I/home/ferraris/boosto/include/ mpi_main.cpp -o mpi
