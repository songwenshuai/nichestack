# md4test.mak for NetPort CRYPT directory. This builds a 
# Windows DOS box executable for verifying the md4.c port
#

#set tools, and environment
!include ..\cflags.mak


OBJS =  md4test.$(OBJ) \
	md4.$(OBJ)

HFILES = md4.h ..\h_h\ppp_port.h

LNK = link.lnk

all: md4test.exe

md4test.exe: $(OBJS) $(HFILES) md4test.mak
	$(MD4LINK1)
	$(MD4LINK2)
	$(MD4LINK3)
	$(MD4LINK4)
	$(LINK) @$(LNK)

#
# Build rules:
#
.c.$(OBJ):
	$(CC) $(CFLAGS) $*.c


