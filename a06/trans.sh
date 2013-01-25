#!/bin/bash
echo 156
./enum -a a z "START /x/ ID"  
./enum -a A Z "START /x/ ID"  
./enum -a a z "ID /x/ ID"  
./enum -a A Z "ID /x/ ID"  
./enum 0 9 "ID /x/ ID"  
./enum 1 9 "START /x/ NUM"  
./enum 0 9 "NUM /x/ NUM"
cat transm
