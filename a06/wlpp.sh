#!/bin/bash

echo 80
./enum 0 9 
./enum -a a z 
./enum -a A Z 
cat alphabet

echo 26
cat states 

echo START

echo 24
cat final_state

bash trans.sh
