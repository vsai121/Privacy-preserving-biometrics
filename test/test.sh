#!/bin/sh

./keyGen
./encrypt feature.txt
./enroll feature2.txt
./compute
./decrypt
