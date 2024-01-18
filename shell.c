#include "computer.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define LINE_LENGTH 10

char terminate_flag = 'F';

// Initializes the shell module
int shell_init() {}

int i;
// Prints out all the registers on the screen
void shell_print_registers() {
  for (i = 0; i < LINE_LENGTH; i++) {
    printf("=");
  }
  printf("\n");
  printf("Register Dump");
  printf("\n");
  for (i = 0; i < LINE_LENGTH; i++) {
    printf("=");
  }
  printf("\n");
  printf("Registers: Contents");
  printf("\n");
  printf("Base: %d\n", reg.Base);
  printf("PC: %d\n", reg.PC);
  printf("IR0: %d\n", reg.IR0);
  printf("IR1: %d\n", reg.IR1);
  printf("AC: %d\n", reg.AC);
  printf("MAR: %d\n", reg.MAR);
  printf("MBR: %d\n", reg.MBR);
}

// Prints out all the words in memory in integer form on the screen
void shell_print_memory() {
  int size;
  FILE *fp = fopen("config.sys", "r");
  fscanf(fp, "%d", &size);
  for (i = 0; i < LINE_LENGTH; i++) {
    printf("=");
  }
  printf("\n");
  printf("Memory Dump: k = %d", reg.Base);
  printf("\n");
  for (i = 0; i < LINE_LENGTH; i++) {
    printf("=");
  }
  printf("\n");
  printf("Address: Contents");
  printf("\n");
  // Prints out all the words in memory in integer form on the screen
  int x;
  for (i = size; i > 0; i--) {
    // printf("%d: %d\n", i, Mem[i]);
    if (Mem[i] != 0) {
      x = i;
      // printf("%d",x);
      break;
    }
  }
  for (i = 0; i <= x; i++) {
    printf("%d: %d\n", i, Mem[i]);
  }
}

// Executes the shell command based on the given command code
char shell_command() {
  char inputfilename[100];
  int cmd;
  int base;
  char action[10]; // allocate an array to store user input
  // while (terminate_flag!=true) {
  while (terminate_flag != 'T') {
    printf("Enter action: \n");
    scanf("%9s",
          action); // limit the input to 9 characters to prevent buffer overflow
    if (strcmp(action, "1") == 0) {
      // handle submit command
      cmd = 1;
    } else if (strcmp(action, "0") == 0) {
      // handle terminate command
      cmd = 0;
    } else if (strcmp(action, "2") == 0) {
      // handle register command
      cmd = 2;
    } else if (strcmp(action, "3") == 0) {
      // handle memory command
      cmd = 3;
    } else if (strcmp(action, "4") == 0) {
      cmd = 4;
    } else if (strcmp(action, "5") == 0) {
      // handle PCB command
      cmd = 5;
    } else if (strcmp(action, "6") == 0) {
      // handle printer command
      cmd = 6;
    } else {
      printf("Invalid command.\n");
    }
    switch (cmd) {
    case 0: // terminate
      terminate_flag = 'T';
      break;
    case 1: // submit
      printf("Enter filename and start address\n");
      scanf("%s%d", inputfilename, &base);
      load_prog(inputfilename, base);
      // print_init_spool(1);
      // printer_init_spool();
      // process_submit();
      break;
    case 2: // dump values of all registers
      shell_print_registers();
      break;
    case 3: // dump memory
      shell_print_memory();
      break;
    case 4: // dump readyQ
      process_print_readyQ();
      break;
    case 5: // dump PCBs
      process_print_all_PCBs();
      break;
    case 6: // dump printer
      printer_dump_spool();
      break;
    }
    //}
  }
}