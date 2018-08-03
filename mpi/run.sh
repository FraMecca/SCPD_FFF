#!/bin/bash

NP=4
if [ $# -eq 0 ]; then
    echo "Running with 4 cores."
else
    NP=$1
    echo "Running with $1 cores."
fi

mpirun --hostfile hostfile -np $NP mpi_cgl
