This readme.txt file is for the vfs directory. This file contains
1. Brief information about files in this directory
2. Tips on how to build the files in this directory.

Files in this directory
-----------------------
This directory contains sources for Virtual File System.
The following files are shipped in this directory as of 11/3/99.
VFSPORT.H
VFSFILES.H
VFSFILES.C
VFSUTIL.C
VFSSYNC.C
MAKEFILE
README.TXT

VFS can be used by various InterNiche products like WebServer,
FTP Server etc.

How to build
------------
Please refer to the VFS reference guide and BUILD_TN.PDF for 
detailed information on how to build. The files in this directory
have been compiled to the highest level with no warnings or errors
with the standard compilers.

By default, VFS is used by WebServer, which runs on InterNiche TCP/IP stack. 
InterNiche "makes" are done by running the make command in the target directory,
which copies all target dependent files to ..\H_H directory. VFS
refers to files in that directory. So, once a complete build is done,
VFS.LIB can be compiled by just running the make command in 
VFS directory.

