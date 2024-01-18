#include "computer.h"
#include <stdio.h>
#include <stdlib.h>

int *Mem, MAR, MBR;

// Initializes the memory with the given size M
void mem_init(int M) {
  FILE *fp = fopen("config.sys", "r");
  if (fp == NULL) {
    printf("Error opening config file\n");
    exit(1);
  }
  fscanf(fp, "%d", &M);
  Mem = malloc(M * sizeof(int));
  printf("Array 'Mem' is initialized with size: %d\n", M);
  fclose(fp);
}

// Reads the value at the memory address in MAR and stores it in MBR
void mem_read() { reg.MBR = Mem[reg.MAR]; }

// //Writes the value in MBR to the memory address in MAR
void mem_write() { Mem[reg.MAR] = reg.MBR; }

void memory_dump() { shell_print_memory(); }
