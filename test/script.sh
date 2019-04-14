#!/bin/sh
echo "" > result.txt
for i in 1 2 3 4 5 6 7 8 9 10
do
for j in 1 2 3 4 5 6 7 8 9 10
do
	echo "Running script for img$i and img$j"
	echo "($i,$j)" >> result.txt
	../build/fingerprint_process ../data/dataset2/img$i.png ../data/dataset2/img$j.png ../build/feature.txt ../build/feature2.txt >> result.txt
done
done
