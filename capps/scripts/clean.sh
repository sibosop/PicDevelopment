#!/bin/sh

for tmp
do
	cd $picdevc/$tmp; echo cleaning $tmp; nmake -nologo clean;
done
