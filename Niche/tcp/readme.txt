This readme.txt file is for the tcp directory. This file contains
1. Brief information about files in this directory
2. Tips on how to build the files in this directory.

	A list of the files provided in this directory is in
the NicheStack Technical Reference.

This directory is part of the NicheStack TCP/IP distribution. ..\IP
directory contains sources for IP.

How to build
------------
Please refer to the NicheStack Technical Reference and BUILD_TN.PDF for 
detailed information on how to build. The files in this directory
have been compiled to the highest level with no warnings or errors
with the standard compilers.

InterNiche "makes" are done by running the make command in the target 
directory, which copies all target dependent files to ..\H_H directory. 
TCP refers to files in that directory. So, once a complete build is done,
TCP.LIB can be compiled by just running the make command in 
TCP directory.

