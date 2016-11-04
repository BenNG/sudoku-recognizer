#!/bin/bash
i=0;
for file in $1/*; do
mv ${file} $1/s$i.jpg
((++i))
done
