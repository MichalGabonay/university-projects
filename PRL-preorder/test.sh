#!/usr/bin/env bash

if [ $# -ne 1 ];then
    echo "run: test.sh <values>"
    exit;
fi;

#preklad cpp zdrojaku
mpic++ --prefix /usr/local/share/OpenMPI -o pro pro.cpp

#pocet uzlov v grafe
NODES=$(echo $1 | wc -c)
NODES=$((NODES - 1))

#pocet procesov
PROCESSES=$((2*NODES - 2))

#spusteni
mpirun --prefix /usr/local/share/OpenMPI -np $PROCESSES  pro $1

rm pro