#include "computer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

PCB pcb_list[MAX_PROCESSES];
ReadyQueue readyQ;

int num_processes = 0;

void process_init_PCBs(int pid, int base, int address, char state,
                       char *fname) {
  pcb_list[pid].pid = pid;
  pcb_list[pid].PC = address;
  pcb_list[pid].state = state;
  pcb_list[pid].Base = base;
  pcb_list[pid].next = NULL;
  // allocate memory for filename and copy input string
  pcb_list[pid].filename = malloc(strlen(fname) + 1);
  strcpy(pcb_list[pid].filename, fname);
}

void dup_register(int n) {
  regs.pid = pcb_list[n].pid;
  regs.PC = pcb_list[n].PC;
  regs.IR0 = pcb_list[n].IR0;
  regs.IR1 = pcb_list[n].IR1;
  regs.AC = pcb_list[n].AC;
  regs.MAR = pcb_list[n].MAR;
  regs.MBR = pcb_list[n].MBR;
  regs.Base = pcb_list[n].Base;
  // printf("%d\t%d\t%d\t%d\t%d\n", regs.Base, regs.PC, regs.MBR,regs.IR0,
  // regs.IR1);
}

void process_submit() {
  // dup_register();
  process_set_registers();
  cpu_operation();
}

void process_init_readyQ() {
  readyQ.head = NULL;
  readyQ.tail = NULL;
}

// Remove a process from the ready queue
void process_remove_readyQ(int pid) {
  PCB *prev_pcb = NULL;
  PCB *current_pcb = readyQ.head;

  while (current_pcb != NULL && current_pcb->pid != pid) {
    prev_pcb = current_pcb;
    current_pcb = current_pcb->next;
  }

  if (current_pcb == NULL) {
    // printf("Error 1: Process not found in ready queue.\n");
    // printf("All Processes completed\n");
    load_prog1("prog-idle", 0);
    dup_register(0);
    process_set_registers();
    while (1) {
      cpu_fetch_instruction();
      cpu_execute_instruction();
    }
    return;
  }

  if (current_pcb == readyQ.head) {
    readyQ.head = current_pcb->next;
  } else {
    prev_pcb->next = current_pcb->next;
  }

  if (current_pcb == readyQ.tail) {
    readyQ.tail = prev_pcb;
  }

  current_pcb->next = NULL;
}

// Insert into ready queue
void process_insert_readyQ(int pid) {
  PCB *pcb = &pcb_list[pid];
  if (pcb->pid < 0) {
    printf("Error: Invalid process ID.\n");
    return;
  }
  if (readyQ.head == NULL) {
    readyQ.head = pcb;
    readyQ.tail = pcb;
    pcb->next = NULL;
  } else {
    readyQ.tail->next = pcb;
    readyQ.tail = pcb;
    pcb->next = NULL;
  }
}

PCB *process_fetch_readyQ() {
  // Check if the ready queue is empty
  if (readyQ.head == NULL) {
    return NULL;
  }

  // Fetch the first process in the ready queue
  PCB *pcb = readyQ.head;
  // printf("FetchHead=%d",pcb->pid);
  readyQ.head = pcb->next;
  // pcb= pcb;

  if (readyQ.head == NULL) {
    readyQ.tail = NULL;
  }

  // Move the fetched process to the end of the queue
  if (pcb->next == NULL) {
    pcb->next = readyQ.head;
    readyQ.tail = pcb;
  } else {
    PCB *current_pcb = readyQ.head;
    while (current_pcb->next != NULL) {
      current_pcb = current_pcb->next;
      // printf("%d\n",current_pcb);
    }
    current_pcb->next = pcb;
    readyQ.tail = pcb;
  }
  pcb->next = NULL;
  // Return the fetched PCB pointer
  return pcb;
}

// Print readyQ
void process_print_readyQ() {
  int i = 0;
  PCB *current_pcb = readyQ.head;
  printf("====================\n");
  printf("ReadyQ Dump:\n");
  printf("====================\n");
  printf("Index: Process ID\n ");
  while (current_pcb != NULL) {
    printf("%d \t %d\n", i, current_pcb->pid);
    current_pcb = current_pcb->next;
    i++;
  }
}

// Print PCB dump
void process_print_PCB(struct PCB pcb) {
  printf("PID: %d\n", pcb.pid);
  printf("PC: %d\n", pcb.PC);
  printf("Filename: %s\n", pcb.filename);
  printf("State: %c\n", pcb.state);
  printf("Base: %d\n", pcb.Base);
  printf("IR0: %d\n", pcb.IR0);
  printf("IR1: %d\n", pcb.IR1);
  printf("AC: %d\n", pcb.AC);
  printf("MAR: %d\n", pcb.MAR);
  printf("MBR: %d\n", pcb.MBR);
}

// void process_print_all_PCBs() {
//     int i = 1;
//     while (pcb_list[i].filename != NULL) {
//         printf("PCB %d:\n", i);
//         process_print_PCB(pcb_list[i]);
//         printf("\n");
//         i++;
//     }
// }

void process_print_all_PCBs() {
  struct PCB pcb;
  int j = 0;
  int i = 1;
  printf("===============\n");
  printf("PCB Dump\n");
  printf("===============\n");
  printf("Index: [ Filename: XXXXX, PID:#, BASE:#, PC:#, IRO:#, IR1:#, AC:#, "
         "MAR:#, MBR:# ]\n");
  while (pcb_list[i].filename != NULL) {

    while (j < i) {
      printf("%d \t [ Filename: %s, PID: %d, BASE: %d, PC: %d, IRO:%d, IR1: "
             "%d, AC:%d, MAR:%d, MBR:%d ]\n",
             j, pcb_list[i].filename, pcb_list[i].pid, pcb_list[i].Base,
             pcb_list[i].PC, pcb_list[i].IR0, pcb_list[i].IR1, pcb_list[i].AC,
             pcb_list[i].MAR, pcb_list[i].MBR);
      //   printf("for comparing\n");
      // process_print_PCB(pcb_list[i]);
      // printf("\n");
      j++;
    }
    i++;
  }
}

// Context switch
void process_context_switch(PCB *pcb, PCB *next_pcb) {
  // load register in current pcb
  pcb->PC = reg.PC;
  pcb->IR0 = reg.IR0;
  pcb->IR1 = reg.IR1;
  pcb->MAR = reg.MAR;
  pcb->MBR = reg.MBR;
  pcb->AC = reg.AC;
  // load next pcb in register
  reg.pid = next_pcb->pid;
  reg.PC = next_pcb->PC;
  reg.IR0 = next_pcb->IR0;
  reg.IR1 = next_pcb->IR1;
  reg.AC = next_pcb->AC;
  reg.MAR = next_pcb->MAR;
  reg.MBR = next_pcb->MBR;
  reg.Base = next_pcb->Base;
}

// dispose a PCB for an exiting process
void process_dispose_PCB(int pid) {
  PCB *pcb = &pcb_list[pid];
  free(pcb);
}

// Exit process
void process_exit(int pid) {
  PCB *pcb = &pcb_list[pid];
  if (readyQ.head == pcb) {
    readyQ.head = pcb->next;
  }
  if (readyQ.tail == pcb) {
    readyQ.tail = NULL;
  }
  process_remove_readyQ(pid);
  process_insert_readyQ(pid);
  // process_dispose_PCB(pid);
}

void process_init() {
  // process_init_PCBs();
  process_init_readyQ();
  // printf("%d\t%d\t%d\t%d\t%d\t%d\t%c\t%c\t%s\n", pcb_list[0].pid,
  // pcb_list[1].pid, pcb_list[0].PC,pcb_list[1].PC,
  // pcb_list[0].Base,pcb_list[1].Base,pcb_list[0].state,pcb_list[1].state,pcb_list[0].filename);
}

void process_execute() {
  while (readyQ.head != NULL) {
    printf("Loop = %d\n", 100);
    cpu_operation();
  }
}
