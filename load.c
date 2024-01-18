#include "computer.h"
#include <stdio.h>
#include <stdlib.h>

// Loads the user program into memory starting from the given base address
int pid = 1;
int N, mem_address = 0;
void load_prog(char *fname, int base) {
  int word;
  char state = 'R';
  int address = cpu_mem_address(base, mem_address);
  // printf("Address=%d \n",address);
  process_init_PCBs(pid, base, address, state, fname);
  process_insert_readyQ(pid);
  // printer_init();
  // print_init();
  print_init_spool(pid);
  printer_init_spool();
  // printf("Head=%d",readyQ.head->pid);
  // process_print_readyQ();
  pid++;
  FILE *fp = fopen(fname, "r");
  if (fp == NULL) {
    printf("Error opening file.\n");
    exit(1);
  }

  // int Ncode;
  //    FILE *fp1 = fopen(fname, "r");
  //    if (fscanf(fp1, "%d", &word) == 1)
  //    {
  //       Ncode = word;
  //       printf("Number of instructions: %d\n", Ncode);
  //    }
  //    else
  //    {
  //       printf("Failed to read an integer from the file.\n");
  //    }
  // Ncode = fscanf(fp, "%d", &word);

  // skip first line
  char buffer[10];
  fgets(buffer, 10, fp);

  // Read from the second line of the file
  while (fscanf(fp, "%d", &word) == 1) {
    Mem[address] = word;
    address++;
    N++;
  }
  mem_address = address;
  // printf("%d\t%d\t%d\n",N,address,mem_address);
  load_finish(fp);
}

void load_prog1(char *fname, int base) {
  int pid = 0;
  int word;
  char state = 'R';
  int memory_address = 0;
  int address = cpu_mem_address(base, memory_address);
  // printf("Address=%d \n",address);
  process_init_PCBs(pid, base, address, state, fname);
  process_insert_readyQ(pid);
  print_init_spool(pid);
  printer_init_spool();
  FILE *fp1 = fopen(fname, "r");
  if (fp1 == NULL) {
    printf("Error opening file.\n");
    exit(1);
  }
  // skip first line
  char buffer[10];
  fgets(buffer, 10, fp1);

  // Read from the second line of the file
  while (fscanf(fp1, "%d", &word) == 1) {
    Mem[address] = word;
    address++;
    N++;
  }
  mem_address = address;
  load_finish(fp1);
}

// Cleans up and closes the program file
void load_finish(FILE *fp) { fclose(fp); }
