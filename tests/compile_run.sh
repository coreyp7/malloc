#!/bin/bash
gcc test1.c ../mymalloc.c -o test.exe
./test.exe > output.log
