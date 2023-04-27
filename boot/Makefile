include ../Makefile.inc

all : boot.o vector.o

boot.o : boot.s
	$(AS) -c $(ASFLAGS) $<
vector.o : vector.s
	$(AS) -c $(ASFLAGS) $<