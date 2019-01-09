This readme.txt file is for the telnet directory. This file contains
1. Brief information about files in this directory
2. Tips on how to build the files in this directory.

Files in this directory
-----------------------
This directory contains sources for TELNET Server.
The following files are shipped in this directory as of 11/2/99.
MAKEFILE
TELERR.C
TELMENU.C
TELNET.C
TELNET.H
TELPARSE.C
TELPORT.C
TELPORT.H
README.TXT

The files telport.c and telport.h are port-dependent files. 

How to build
------------
Please refer to the TELNET reference guide and BUILD_TN.PDF for 
detailed information on how to build. The files in this directory
have been compiled to the highest level with no warnings or errors
with the standard compilers.

By default, TELNET uses InterNiche TCP/IP stack. InterNiche "makes" are
done by running the make command in the target directory,
which copies all target dependent files to ..\H_H directory. TELNET
refers to files in that directory. So, once a complete build is done,
TELNET.LIB can be compiled by just running the make command in 
TELNET directory.

