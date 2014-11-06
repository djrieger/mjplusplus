#!/bin/sh
for i in $1/*;do 
    echo $i 
    ../mj++ $i 
    echo $?
done
