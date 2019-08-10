#!/bin/bash

name="sim_cuda"
defines=""
name="$1"
defines="$2 $3 $4 $5 $6 $7 $8 $9"
/usr/local/cuda-9.2/bin/nvcc -x cu -std=c++11 sim_cuda.cpp ldpc/ldpc.cpp ldpc/decoder.cpp device/kernel.cpp sim/ldpcsim.cpp sim/start.cpp -o "$1" -gencode=arch=compute_61,code=sm_61 -rdc=true -O3 -w $defines
