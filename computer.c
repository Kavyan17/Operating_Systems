#include "computer.h"
#include <stdio.h>
#include <stdlib.h>

int CID;

// struct PCB *pcb;
Reg regs;
int main(int argc, char *argv[]) {
  int M, cmd;
  printf("Enter the CID\n");
  scanf("%d", &CID);
  mem_init(M);
  mem_read();
  mem_write();
  printer_time();
  time_quantum();
  shell_init();
  printer_main();
  print_init(CID);
  process_init();
  int i = 0;
  while (terminate_flag != 'T') {
    shell_command();
    while (i < 1) {
      dup_register(1);
      process_set_registers();
      i++;
    }
    // process_execute();
    cpu_operation();
  }
  // shell_command();
  // dup_register();
  // process_set_registers();
  // cpu_operation();
  // printer_print();
  // print();
  // cpu_fetch_instruction();
  // print_registers();
  // process_print_readyQ();
  // process_execute();
  // printf("Next process in fetch ready queue is %d\n", readyQ.head>pid);
  // process_print_readyQ();
  // PCB *next1 = process_fetch_readyQ();
  // printf("Next process in fetch ready queue is %d\n", next1->pid);
  //  process_remove_readyQ(1);
  //  PCB *next2 = process_fetch_readyQ();
  //  printf("Next process in fetch ready queue is %d\n", next2->pid);
  // process_print_readyQ();
  //  PCB *next3 = process_fetch_readyQ();
  //  printf("Next process in fetch ready queue is %d\n", next3->pid);
  // process_print_readyQ();
  // process_print_readyQ();
  // process_print_readyQ();
  // print();
  // process_init_readyQ();
  //  char inputfilename[100];
  //  int base;
  //  printf("Enter filename and start address");
  //  scanf("%s%d", inputfilename, &base);
  //  load_prog(inputfilename, base);
  //  process_init_PCB();
  // init_registers();
  // process_set_registers();
  //  boot_system(base);
  // cpu_fetch_instruction();
  // cpu_execute_instruction();
  // cpu_operation(inputfilename);
  // shell_print_registers();
  // shell_print_memory();
  // printf("%d\n%d\n%d\n%d\n", 1,2,3,4);
  // return 0;
}

// Performs initialization of the hardware
void boot_system(int k) {
  // process_init_PCB();
  // process_set_registers();
}

void process_set_registers() {
  reg.pid = regs.pid;
  reg.PC = regs.PC;
  reg.IR0 = regs.IR0;
  reg.IR1 = regs.IR1;
  reg.AC = regs.AC;
  reg.MAR = regs.MAR;
  reg.MBR = regs.MBR;
  reg.Base = regs.Base;
  // reg.PC = pcb->PC;
  // reg.IR0 = pcb->IR0;
  // reg.IR1 = pcb->IR1;
  // reg.AC = pcb->AC;
  // reg.MAR = pcb->MAR;
  // reg.MBR = pcb->MBR;
  // reg.Base = pcb->Base;
  // printf("%d\t%d\t%d\t%d\t%d\n", reg.Base, reg.PC, reg.MBR,reg.IR0, reg.IR1);
}
