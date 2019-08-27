This readme.txt file is for the crypt directory. This file contains
1. Brief information about files in this directory
2. Tips on how to build the files in this directory.

Files in this directory
-----------------------
This directory contains sources for MD5 Authentication.
The following files are shipped in this directory as of 11/3/99.
MAKEFILE
MD5.C
MD5.H
README.TXT

MD5 Authentication is being used by PPP, WebServer and SNMPv3 modules.

How to build
------------
Please refer to the TOOLNOTE.DOC for detailed information on how to build. 
The files in this directory have been compiled to the highest level 
with no warnings or errors with the standard compilers.

By default, CRYPT is used by InterNiche modules which used InterNiche TCP/IP 
stack. InterNiche "makes" are done by running the make command in the 
target directory, which copies all target dependent files to ..\H_H directory. 
CRYPT refers to files in that directory. So, once a complete build is done,
CRYPT.LIB can be compiled by just running the make command in 
CRYPT directory.

