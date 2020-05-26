#!/bin/bash

NUMBER=30 # number of threads
N=1000000 # argument

g++ task2.cpp -o out -lpthread
./out $NUMBER $N
rm out
