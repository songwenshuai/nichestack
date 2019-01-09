This readme.txt file is for the net directory. This file contains
1. Brief information about files in this directory
2. Tips on how to build the files in this directory.

	This directory contains file which provide network utilities
such as packet managment and interface control. It also contains some
Internet-related protocol files which are common to the mini InterNiche
stack (NicheLite) and the full size stack. Examples of this are SLIP
and the DHCP client.

Files in this directory
-----------------------
The following files are shipped in this directory as of 04/01/2001

dhcpclnt.c
dhcputil.c
dnsclnt.c
iface.dif
macloop.c
makefile
ping.c
pktalloc.c
q.c
rcs
readme.bak
readme.txt
slip.c
slip.h
slipif.c
slipport.h
sources.mak
udp_open.c


How to build
------------
Please refer to the TCP/IP  reference guide and BUILD_TN.PDF for 
detailed information on how to build. The files in this directory
have been compiled to the highest level with no warnings or errors
with the standard compilers.

InterNiche "makes" are done by running the make command in the target 
directory. This copies target dependent header files to ..\H directory. 
The files here in "net" refer to files in that directory, so only after a 
complete build is done from the target directory can net.lib be compiled 
by just running the make command in the net directory.

