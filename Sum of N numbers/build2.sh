#!/bin/bash

NUMBER=30
N=1000000

g++ 2thread.cpp -o out -lpthread
./out $NUMBER $N
rm out
