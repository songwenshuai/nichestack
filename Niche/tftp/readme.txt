This readme.txt file is for the tftp directory. This file contains
1. Brief information about files in this directory
2. Tips on how to build the files in this directory.

Files in this directory
-----------------------
This directory contains sources for TFTP (Trivial File Transfer Protocol).
The following files are shipped in this directory as of 11/3/99.
TFTPCLI.C
TFTPSRV.C
TFTPPORT.C
TFTPMENU.C
TFTPPORT.H
MAKEFILE
TFTPUDP.C
TFTPUTIL.C
TFTP.H
README.TXT

The tftpport.c and tftpport.h are port-dependent files. 

How to build
------------
Please refer to the TFTP reference guide and BUILD_TN.PDF for 
detailed information on how to build. The files in this directory
have been compiled to the highest level with no warnings or errors
with the standard compilers.

By default, TFTP is being used by InterNiche TCP/IP stack. 
InterNiche "makes" are done by running the make command in the target dir,
which copies all target dependent files to ..\H_H directory. TFTP
refers to files in that directory. So, once a complete build is done,
TFTP.LIB can be compiled by just running the make command in 
TFTP directory.

