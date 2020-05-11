#!/bin/bash

ARG=30

g++ 1thread.cpp -o out -lpthread
./out $ARG
rm out
