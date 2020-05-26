#!/bin/bash

ARG=30

g++ task1.cpp -o out -lpthread
./out $ARG
rm out
