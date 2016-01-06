#!/bin/bash

#load_klee
export PATH=/home/lijiaying/Research/llvm-2.9/Release+Asserts/bin:/home/lijiaying/Research/llvm-gcc4.2-2.9-x86_64-linux/bin:/home/lijiaying/Research/klee/Release+Asserts/bin:/home/lijiaying/Research/clang+llvm-2.9/bin:$PATH

llvm-gcc --emit-llvm -c -g i2i.c
#klee -write-smt2s i2i.o 1>i2i.klee_output 2>&1
klee -write-smt2s i2i.o
ret=$?

cd klee-last
for filename in `ls test*.smt2`
do
    echo $filename
    z3 -smt2 $filename 
done

for filename in `ls test*.assert.err`
do
    echo -n "Error::"
    echo $filename
done

cd ..

rm -rf ./klee-*

#unload_klee
source ~/.bashrc

exit $ret 

