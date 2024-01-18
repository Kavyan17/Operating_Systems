#include "computer.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Register struct for storing register values
Registers reg;
int m_addr = 0;

// Fetches the instruction from memory and stores it in the instruction
// registers
void cpu_fetch_instruction() {
  reg.MAR = reg.PC;
  mem_read();
  reg.IR0 = reg.MBR;
  reg.MAR = reg.PC + 1;
  mem_read();
  reg.IR1 = reg.MBR;
  reg.PC += 2;
  // printf("Base = %d\n", base);
  // printf("%d\t%d\t%d\t%d\t%d\n", reg.PC, reg.MAR, reg.MBR, reg.IR0, reg.IR1);
}

char state = 'R';
// Executes the instruction based on the opcode and operand
char cpu_execute_instruction() {
  // int m_addr;
  switch (reg.IR0) {
  case 1: // load constant to AC
    reg.AC = reg.IR1;
    // reg.PC += 2;
    // printf("AC = %d\n", reg.AC);
    state = 'R';
    return state;
    break;
  case 2: // load Mem[m-addr] into AC
    m_addr = reg.IR1 + reg.Base;
    reg.AC = Mem[m_addr];
    // reg.AC = reg.MBR;
    // reg.PC += 2;
    // printf("AC = %d\n", reg.AC);
    state = 'R';
    return state;
    break;
  case 3: // load Mem[m-addr] into MBR, add MBR to AC
    m_addr = reg.IR1 + reg.Base;
    reg.MBR = Mem[m_addr];
    reg.AC += reg.MBR;
    // reg.PC += 2;
    // printf("MBR = %d\t AC = %d\n", reg.MBR, reg.AC);
    state = 'R';
    return state;
    break;
  case 4: // multiply memory to AC
    m_addr = reg.IR1 + reg.Base;
    reg.MBR = Mem[m_addr];
    reg.AC *= reg.MBR;
    // reg.PC += 2;
    // printf("MBR = %d\t AC = %d\n", reg.MBR, reg.AC);
    state = 'R';
    return state;
    break;
  case 5: // store AC to Mem[m-addr]
    m_addr = reg.IR1 + reg.Base;
    Mem[m_addr] = reg.AC;
    // mem_write();
    // reg.PC += 2;
    // printf("Mem[m-addr] = %d\n", Mem[m_addr]);
    state = 'R';
    return state;
    break;
  case 6: // If AC > 0 then go to the address given in Mem[m-addr]. Otherwise,
          // continue to the next instruction
    // m_addr = cpu_mem_address(reg.IR1);
    // reg.PC = Mem[m_addr];
    // mem_read();
    if (reg.AC > 0) {
      m_addr = reg.IR1 + reg.Base;
      reg.PC = Mem[m_addr];
    } else //{
           // reg.PC += 2;
      //}
      state = 'R';
    return state;
    break;
  case 7: // Print the value in AC
    printf("AC = %d\n", reg.AC);
    // printf("AC = %d\n, PID= %d\n", reg.AC, reg.pid);
    print_print(reg.AC, reg.pid);
    printer_print();
    // reg.PC += 2;
    state = 'R';
    return state;
    break;
  case 8: // Sleep for the given “time” in microseconds, which is the operand
    usleep(reg.IR1);
    // reg.PC += 2;
    state = 'R';
    return state;
    break;
  case 9: // shell command
    shell_command(reg.IR1);
    // reg.PC += 2;
    state = 'R';
    return state;
    break;
  // case 0: // End of the current program,
  //     state='E';
  //     exit(0);
  //     break;
  case 0: // End of the current program
    print_end_spool(reg.pid);
    state = 'E';
    return state;
    // print_end_spool(reg.pid);
    exit(0);
    break;
  }
}

// Computes the memory address to be accessed
int cpu_mem_address(int base, int m_address) {
  // printf("B = %d\t%d\n", base, m_address);
  return base + m_address;
}

int D, TQ;
void time_quantum() {
  FILE *fp = fopen("config.sys", "r");
  if (fp == NULL) {
    printf("Error opening comfig file\n");
    exit(1);
  }
  fscanf(fp, "%d", &D);
  fscanf(fp, "%d", &TQ);
  printf("TQ is initialized with size: %d\n", TQ);
  fclose(fp);
}

void cpu_operation() {
  // printf("Loop = %d\n", 100);
  //  PCB *pcb = process_fetch_readyQ();
  //  printf("Head=%d",pcb->pid);
  //  PCB *pcb = readyQ.head;
  //  printf("Head=%d",pcb->pid);
  while (readyQ.head != NULL) {
    int i = TQ;
    PCB *pcb = process_fetch_readyQ();
    // printf("Current pcb = %d\n", pcb->pid);
    if (pcb != NULL) {
      // process_set_registers(pcb);
      while (i > 0) {
        // printf("Loop = %d\n", i);
        cpu_fetch_instruction();
        if (state == 'R') {
          // printf("134 line");
          state = cpu_execute_instruction();
          // printf("State = %c\n", state);
          //  process_print_all_PCBs();
        } else {
          print_end_spool(pcb->pid);
          process_remove_readyQ(pcb->pid);
          // process_dispose_PCB(pcb->pid);
          // print_end_spool(pcb->pid);
          // process_print_readyQ();
        }
        process_print_all_PCBs();
        usleep(100000); // 10 milliseconds
        i--;
        // printf("Loop = %d\n", i);
        // process_print_PCB(pcb_list[pcb->pid]);
      }
      // perform a context switch if there is another process ready to run
      PCB *next_pcb = readyQ.head;
      // printf("Next pcb = %d\n", next_pcb->pid);
      if (next_pcb != NULL) {
        // PCB *pcb = PCB *next_pcb;
        printf("===============\n");
        printf("Process Switch\n");
        printf("===============\n");
        printf("Switched-out process:%d\n Swicthed-in process:%d \n", pcb->pid,
               next_pcb->pid);
        process_context_switch(pcb, next_pcb);
        // process_print_PCB(pcb_list[pcb->pid]);
        // process_print_PCB(pcb_list[next_pcb->pid]);
      }
      if (state == 'R') {
        process_insert_readyQ(pcb->pid);
        process_print_readyQ();
      }
      // else {
      //      process_remove_readyQ(pcb->pid);
      //      process_dispose_PCB(pcb);
      //      process_print_readyQ();
      //  }
    }
    usleep(100000);
  }
  // process_print_readyQ();
  print_terminate();
}

void cpu_dump_registers() {
  printf("Dumping registers:\n");
  printf("PC = %d\n", reg.PC);
  printf("IR0 = %d\n", reg.IR0);
  printf("IR1 = %d\n", reg.IR1);
  printf("AC = %d\n", reg.AC);
  printf("MBR = %d\n", reg.MBR);
  printf("MAR = %d\n", reg.MAR);
  printf("Base = %d\n", reg.Base);
  printf("m_addr = %d\n", m_addr);
}
