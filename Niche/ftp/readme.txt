This readme.txt file is for the ftp directory. This file contains
1. Brief information about files in this directory
2. Tips on how to build the files in this directory.
3. "FTP Client only" build

Files in this directory
-----------------------
This directory contains sources for FTP Server and Client.
The following files are shipped in this directory as of 11/2/99.
FTPCLNT.C
FTPCLNT.H
FTPCPRN.C
FTPMENU.C
FTPPORT.H
FTPSDOS.C
FTPSPORT.C
FTPSRV.C
FTPSRV.H
FTPSSOCK.C
MAKEFILE
FTPSVFS.C
FTPSDIR.C
FTPWIN32.C
README.TXT

The ftpsport.c and ftpport.h are port-dependent files. 

FTP Server supports a file system and makes certain calls to access
files in the file system. Some of these calls are dependent on the 
file system. Hence all the file system dependent code is kept in one file.
There is one such file for each file system.
1. FTPSDOS.C for DOS file system
2. FTPSVFS.C for InterNiche's Virtual File System
3. FTPWIN32.C for Window's WIN32 file system.
FTP server expects file system dependent functions to be in ftpsdir.c
Hence depending on the file system being used, the corresponding file
is copied to ftpsdir.c. This copying is done in the target directory.
For example, for the DOS port, in dosmain\makefile the file FTPSDOS.C 
would be copied to FTPSDIR.C. 


How to build
------------
Please refer to the FTP reference guide and BUILD_TN.PDF for 
detailed information on how to build. The files in this directory
have been compiled to the highest level with no warnings or errors
with the standard compilers.

By default, FTP uses InterNiche TCP/IP stack. InterNiche "makes" are
done by running the make command in the target directory,
which copies all target dependent files to ..\H_H directory. FTP
refers to files in that directory. So, once a complete build is done,
FTP.LIB can be compiled by just running the make command in 
FTP directory.

"FTP Client only" build
-----------------------
This section addresses the case when we need to do a build with 
only FTP Client. No FTP Server. 

There following are files required for a FTP Client build.
FTPCLNT.C
FTPCPRN.C
FTPMENU.C
FTPSPORT.C
FTPSSOCK.C
FTPCLNT.H
FTPPORT.H
FTPSRV.H
MAKEFILE

Hence in the makefile, don't include FTPSRV.C and FTPSDIR.C
for the build. The OBJS clause (in the makefile) looks like

OBJS= \
#       ftpsrv.$(OBJ)      \
        ftpssock.$(OBJ)    \
#       ftpsdir.$(OBJ)     \
        ftpsport.$(OBJ)    \
        ftpclnt.$(OBJ)     \
        ftpmenu.$(OBJ)     \
        ftpcprn.$(OBJ)

