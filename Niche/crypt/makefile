# makefile for NetPort CRYPT directory. This builds a 
# library containing the protocols needed for Cryptography
# in NetPort/InterNiche products.
#
#       This makefile should work with most development environments. 
# tool names, paths & flags should be set the included file "..\cflags.mak"
#
# Copyright 1997-1998 by InterNiche Technologies Inc. All rights reserved.
# Copyright 1993-1996 by NetPort Software. All rights reserved.
#
# 7/9/97 - Created from HTTP makefile. -JB-

#set tools, and environment
!include ..\cflags.mak

OBJS= \
        md5.$(OBJ) \
        md4.$(OBJ)

# local file dependancies
DFILES=$(DEPEND_ALL) md5.h

# local includes
INCLUDE=$(ICMD).

# build target 
LIBNAME=crypt.lib

all: $(LIBNAME)

$(LIBNAME): $(OBJS) $(DFILES) makefile
        $(LIB2) $(LIBCMD2)

#
# Build rules:
#
.c.$(OBJ):
	$(CC) $(CFLAGS) $(INCLUDE) $*.c
	$(LIB) $(LIBCMD)

#
# "make clean" support:
#
clean:
        $(RM) *.$(OBJ)
        $(RM) *.lib
        $(CLEANCMD)

#
# Dependancy lists:
#
md5.$(OBJ): md5.c md5.h $(DFILES) makefile

md4.$(OBJ): md4.c md4.h $(DFILES) makefile

