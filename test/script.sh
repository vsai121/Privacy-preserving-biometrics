#!/bin/sh
dnum=3
pnum1=1
pnum2=4
outfile=result$pnum1-$pnum2.txt
basepath=../data/dataset$dnum


echo "" > $outfile
for inum1 in 1 2 3
do
for inum2 in 1 2
do
	echo "Running script for $basepath/$pnum1/$inum1.bmp and $basepath/$pnum2/$inum2.bmp"
	echo "($inum1,$inum2)" >> $outfile
	../build/fingerprint_process $basepath/$pnum1/$inum1.bmp $basepath/$pnum2/$inum2.bmp ../build/feature.txt ../build/feature2.txt >> $outfile
done
done
