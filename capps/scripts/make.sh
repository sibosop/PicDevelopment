#!/bin/csh

cd $picdevc/c73blib; echo making c73blib ; nmake -nologo ; cd ..
cd $picdevc/c71lib; echo making c71lib; nmake -nologo ; cd ..

for tmp
do
	cd $picdevc/$tmp; echo making $tmp ; nmake -nologo ; 
done


