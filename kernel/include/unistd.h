#pragma once
#include "stddef.h"

// system
void printk(const char *msg);
void panic(const char *fmg);
void oops(char *msg);

// task
int  fork(void);
void exit(void);

// memory
void memset(void *addr, unsigned char val, int size);
void* malloc(size_t size);
void free(void *obj);

