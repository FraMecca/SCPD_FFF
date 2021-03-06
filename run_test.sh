#!/bin/bash
set -e
alias mpirun=/home/ferraris/openmpi/bin/mpirun
alias mpicc=/home/ferraris/openmpi/bin/mpicc

DIR=/home/ferraris/scpd_fff
RES=$DIR/bench/results
SEQ=$DIR/cgl_seq
SHM=$DIR/cgl_shm
MPI=$DIR/cgl_mpi
BIN=$DIR/bench/bin

### Tests:
# cgl_seq with -O2
SET1=$DIR/bench/settings/settings1.hpp

# cgl_shm with -O2 (stenc
SET2=$DIR/bench/settings/settings2.hpp

# cgl_mpi with -O2 and:
## seq no scatter
SETM1=$DIR/bench/settings/settingsm1.hpp
## seq with scatter
SETM2=$DIR/bench/settings/settingsm2.hpp
## stencil no scatter
SETM3=$DIR/bench/settings/settingsm3.hpp
## stencil with scatter
SETM4=$DIR/bench/settings/settingsm4.hpp

## hostfile
HOST=$DIR/bench/settings/hostfile

# results directory
rm -r $RES
mkdir -p $RES
mkdir -p $BIN

# sequential
cd $SEQ
cp $SET1 $SEQ/settings.hpp
make opt
mv $SEQ/seq $BIN/seq

# shared memory (stencil)
mkdir -p $RES/shm
cd $SHM
cp $SET2 $SHM/settings.hpp
make  opt
mv $SHM/shm $BIN/shm

## MPI + seq / shm (all the possibilities)
mkdir -p $RES/mpi
cd $MPI
cp $HOST .

cp $SETM1 $MPI/settings.hpp
make  aldinuc
mv $MPI/mpi $BIN/mpi_seq

cp $SETM2 $MPI/settings.hpp
make  aldinuc
mv $MPI/mpi $BIN/mpi_seq_sc

cp $SETM3 $MPI/settings.hpp
make  aldinuc
mv $MPI/mpi $BIN/mpi_st

cp $SETM4 $MPI/settings.hpp
make  aldinuc
mv $MPI/mpi $BIN/mpi_st_sc

cd $BIN
./seq --target

mpirun --hostfile $HOST $BIN/mpi_seq
mkdir -p $RES/mpi/seq/
mv $BIN/time* $RES/mpi/seq/

mpirun --hostfile $HOST $BIN/mpi_seq_sc
mkdir -p $RES/mpi/seq_sc/
mv $BIN/time* $RES/mpi/seq_sc/

mpirun --hostfile $HOST $BIN/mpi_st
mkdir -p $RES/mpi/st/
mv $BIN/time* $RES/mpi/st/

mpirun --hostfile $HOST $BIN/mpi_st_sc
mkdir -p $RES/mpi/st_sc/
mv $BIN/time* $RES/mpi/st_sc/

./seq
mkdir -p $RES/seq
mv $BIN/time* $RES/seq/

./shm
mkdir -p $RES/shm
mv $BIN/time* $RES/shm/
