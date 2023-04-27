include ../../Makefile.inc

INC = -I../include


all : switch.o  syscall.o

switch.o : switch.S
	$(AS) -c $(ASFLAGS) $<
syscall.o : syscall.S
	$(AS) -c $(ASFLAGS) $<