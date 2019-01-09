This readme.txt file is for the ip directory. This file contains
1. Brief information about files in this directory
2. Tips on how to build the files in this directory.

	This directory contains files which provide the IP and 
releated protocol layers, includeing ICMP and UDP. This is the
NicheStack full-featured IP; NicheLite mini-IP is in ..\mip.

	A list of the files provided in this directory is in
the NicheStack Technical Reference.

How to build
------------
Please refer to the TCP/IP  reference guide and BUILD_TN.PDF for 
detailed information on how to build. The files in this directory
have been compiled to the highest level with no warnings or errors
with the standard compilers.

InterNiche "makes" are done by running the make command in the target 
directory. This copies target dependent header files to ..\H directory. 
The files here in "ip" refer to files in that directory, so only after a 
complete build is done from the target directory can ip.lib be compiled 
by just running the make command in this directory.

