#!/bin/sh

for tmp
do
	cd $picdevel/$tmp; nmake clean
done
