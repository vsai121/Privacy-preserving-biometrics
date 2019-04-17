#!/bin/sh
dnum=4
pnum1=4
pnum2=4
outfile=result$pnum1-$pnum2.txt
basepath=../data/dataset$dnum

echo "" > $outfile
for inum1 in 1 2 3 4 5 
do
for inum2 in 1 2 3 4 5
do
	echo "Running script for $basepath/10$pnum1"_"$inum1.bmp and $basepath/10$pnum2"_"$inum2.bmp"
	echo "($inum1,$inum2)" >> $outfile
	../build/fingerprint_process $basepath/10$pnum1"_"$inum1.bmp $basepath/10$pnum2"_"$inum2.bmp ../build/feature.txt ../build/feature2.txt >> $outfile
done
done
