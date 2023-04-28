include ../Makefile.inc

INC = -Iinclude


all : clock.o memory.o panic.o \
	  sched.o syscall.o timer.o \
	  trap.o tty.o nvic.o

trap.o : trap.c
	$(CC) -c $(CFLAGS) $^
memory.o : memory.c
	$(CC) -c $(CFLAGS) $^
sched.o : sched.c
	$(CC) -c $(CFLAGS) $^
clock.o : clock.c
	$(CC) -c $(CFLAGS) $^
panic.o : panic.c
	$(CC) -c $(CFLAGS) $^
syscall.o : syscall.c
	$(CC) -c $(CFLAGS) $^
timer.o : timer.c
	$(CC) -c $(CFLAGS) $^
tty.o : tty.c
	$(CC) -c $(CFLAGS) $^
nvic.o : nvic.c
	$(CC) -c $(CFLAGS) $^
