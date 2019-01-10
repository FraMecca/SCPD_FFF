#!/bin/bash

# shm con 2, 4 partizioni
# shm con 1024, 2048 di griglia
set -e

DIR=/home/ferraris/scpd_fff
RES=$DIR/bench/results3
SHM=$DIR/cgl_shm
BIN=$DIR/bench/bin

### Tests:
SET1=$DIR/bench/settings/shm_512_speedup/settings_2p.hpp
SET2=$DIR/bench/settings/shm_512_speedup/settings_4p.hpp
SET3=$DIR/bench/settings/shm_512_speedup/settings_1024g.hpp
SET4=$DIR/bench/settings/shm_512_speedup/settings_2048g.hpp

# results directory
rm -r $RES -f
mkdir -p $RES
mkdir -p $BIN

# shared memory (stencil)
mkdir -p $RES/shm
cd $SHM
cp $SET1 $SHM/settings.hpp
make opt
mv $SHM/shm $BIN/shm_2p

mkdir -p $RES/shm
cd $SHM
cp $SET2 $SHM/settings.hpp
make opt
mv $SHM/shm $BIN/shm_4p

mkdir -p $RES/shm
cd $SHM
cp $SET3 $SHM/settings.hpp
make opt
mv $SHM/shm $BIN/shm_1024g

mkdir -p $RES/shm
cd $SHM
cp $SET4 $SHM/settings.hpp
make opt
mv $SHM/shm $BIN/shm_2048g


# run them
cd $BIN
$BIN/shm_2p --target
$BIN/shm_2p
mkdir -p $RES/shm_2p
mv $BIN/time* $RES/shm_2p/

$BIN/shm_4p --target
$BIN/shm_4p
mkdir -p $RES/shm_4p
mv $BIN/time* $RES/shm_4p/

$BIN/shm_1024g --target
$BIN/shm_1024g
mkdir -p $RES/shm_1024g
mv $BIN/time* $RES/shm_1024g/

$BIN/shm_2048g --target
$BIN/shm_2048g
mkdir -p $RES/shm_2048g
mv $BIN/time* $RES/shm_2048g/

