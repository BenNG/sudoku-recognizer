#!/bin/bash

cd build/training
j=0
for k in {1..9}
    do cd $k
    for i in *.jpg
        do j=`expr $j + 1`
        mv $i $j.jpg ;
    done
    cd ..
    j=0
done


