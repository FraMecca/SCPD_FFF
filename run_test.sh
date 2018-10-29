#!/bin/bash

if [[ $1 == "-s" || $1 == "--save" ]]; then
	SAVE=true
else
	SAVE=false
fi

DIR=$(pwd)
RES=$DIR/results
SEQ=$DIR/test/cgl_seq
SHM=$DIR/test/cgl_shm
MPI=$DIR/test/cgl_mpi

### Tests:
# cgl_seq with -O2
SET1=$DIR/settings/settings1.hpp

# cgl_shm with -O2 (stencil)
SET2=$DIR/settings/settings2.hpp

# cgl_mpi with -O2 and:
## seq no scatter
SETM1=$DIR/settings/settingsm1.hpp
## seq with scatter
SETM2=$DIR/settings/settingsm2.hpp
## stencil no scatter
SETM3=$DIR/settings/settingsm3.hpp
## stencil with scatter
SETM4=$DIR/settings/settingsm4.hpp

## hostfile
HOST=$DIR/settings/hostfile

# results directory
mkdir -p $RES

# sequential
cd $SEQ
cp $SET1 $SEQ/settings.hpp
make -j12 opt
echo "--- Creating target with $SEQ"
$SEQ/seq --target
echo "--- Running $SEQ"
$SEQ/seq
# save results
if [[ $SAVE ]]; then
	rm -r $RES/seq
	mkdir -p $RES/seq
	cp $SEQ/time* $RES/seq
fi

 set target
TARGET=$SEQ/target.bin

# shared memory (stencil)
mkdir -p $RES/shm
cd $SHM
cp $TARGET .
cp $SET2 $SHM/settings.hpp
make -j12 opt
echo "--- Running $SHM"
$SHM/shm
# save results
if [[ $SAVE ]]; then
	rm -r $RES/shm
	mkdir -p $RES/shm
	cp $SHM/time* $RES/shm
fi

## MPI + seq / shm (all the possibilities)
mkdir -p $RES/mpi
cd $MPI
cp $TARGET .
cp $HOST .

cp $SETM1 $MPI/settings.hpp
make -j12 opt
echo "--- Running $MPI (seq, no scatter)"
mpirun --hostfile $MPI/hostfile -np 12 $MPI/mpi
## save results
if [[ $SAVE ]]; then
	rm -r $RES/mpi/1/
	mkdir -p $RES/mpi/1/
	cp $MPI/time* $RES/mpi/1/
fi

cp $SETM2 $MPI/settings.hpp
make -j12 opt
echo "--- Running $MPI (seq, with scatter)"
mpirun --hostfile $MPI/hostfile -np 12 $MPI/mpi
## save results
if [[ $SAVE ]]; then
	rm -r $RES/mpi/2/
	mkdir -p $RES/mpi/2/
	cp $MPI/time* $RES/mpi/2/
fi

cp $SETM3 $MPI/settings.hpp
make -j12 opt
echo "--- Running $MPI (shm, no scatter)"
mpirun --hostfile $MPI/hostfile -np 4 $MPI/mpi
## save results
if [[ $SAVE ]]; then
	rm -r $RES/mpi/3/
	mkdir -p $RES/mpi/3/
	cp $MPI/time* $RES/mpi/3/
fi

cp $SETM4 $MPI/settings.hpp
make -j12 opt
echo "--- Running $MPI (shm, with scatter)"
mpirun --hostfile $MPI/hostfile -np 4 $MPI/mpi
## save results
if [[ $SAVE ]]; then
	rm -r $RES/mpi/4/
	mkdir -p $RES/mpi/4/
	cp $MPI/time* $RES/mpi/4/
fi
