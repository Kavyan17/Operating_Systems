#include <stdio.h>
#include <semaphore.h>
#ifndef COMPUTER_H
#define COMPUTER_H
// #define MAX_PROCESSES 1000
#define MAX_PROCESSES 10
#define ACK "READY"
//#define queue 10

// Registers
typedef struct {
  int pid;
  int PC;   // program counter
  int IR0;  // instruction register 0
  int IR1;  // instruction register 1
  int AC;   // accumulator
  int MAR;  // memory address register
  int MBR;  // memory buffer register
  int Base; // base register
} Registers;
extern Registers reg;

// Duplicate Registers
typedef struct {
  int pid;
  int PC;   // program counter
  int IR0;  // instruction register 0
  int IR1;  // instruction register 1
  int AC;   // accumulator
  int MAR;  // memory address register
  int MBR;  // memory buffer register
  int Base; // base register
} Reg;
extern Reg regs;

// TimeQuantum
extern int TQ;

// typedef struct {
//   int PC;   // program counter
//   int IR0;  // instruction register 0
//   int IR1;  // instruction register 1
//   int AC;   // accumulator
//   int MAR;  // memory address register
//   int MBR;  // memory buffer register
//   int Base; // base register
//   int PID;  // PID
// } PCB;
// extern PCB pcb;

// PCB
typedef struct PCB {
  int pid;
  int IR0, IR1, PC, MAR, MBR, Base, AC;
  char *filename;
  char state;
  struct PCB *next;
} PCB;
// typedef struct {
//   int PC;   // program counter
//   int IR0;  // instruction register 0
//   int IR1;  // instruction register 1
//   int AC;   // accumulator
//   int MAR;  // memory address register
//   int MBR;  // memory buffer register
//   int Base; // base register
//   int PID;  // PID
//   char state;
//   char filename;
// } PCBS;
// extern PCBS pcbs[MAX_PROCESSES];

// ReadyQ
typedef struct {
  PCB *head;
  PCB *tail;
} ReadyQueue;
extern PCB pcb_list[MAX_PROCESSES];
extern ReadyQueue readyQ;

// Number of instructions
extern int Ncode;

// Number of processess

// Memory
extern int *Mem;
extern int M;

// User Input File
extern char *inputfilename;
extern int base;

// Physical memory address
extern int m_addr;

// Memory functions
void mem_init(int M);
void mem_read();
void mem_write();
void memory_dump();

// CPU functions
void cpu_fetch_instruction();
char cpu_execute_instruction();
int cpu_mem_address(int base, int m_addr);
void cpu_operation();
void time_quantum();

// Loader functions
void load_prog(char *fname, int base);
void load_prog1(char *fname, int base);
void load_finish(FILE *f);

// Shell functions
int shell_init();
void shell_print_registers();
void shell_print_memory();
char shell_command();

// Computer functions
void process_init_PCB();
void process_set_registers();
void boot_system(int k);
void cpu_dump_registers();

// Scheduler functions
void process_init_PCBs(int pid, int base, int address, char state, char *fname);
void dup_register(int n);
void process_init();
void process_insert_readyQ(int pid);
void process_init_readyQ();
void process_exit(int pid);
void process_dispose_PCB(int pid);
void process_context_switch(PCB *pcb, PCB *next_pcb);
void process_print_readyQ();
void process_print_all_PCBs();
PCB *process_fetch_readyQ();
void process_execute();
void print();
void print_registers();
void process_print_PCB(struct PCB pcb);
void process_submit();
void process_remove_readyQ(int pid);

// Terminate flag for printer
extern char pterminate;
extern char terminate_flag;
extern int CID; 

// Pipes
extern int fd[2];
extern int fd1[2];
extern int fd2[2];
extern int pt;

// PrinterManager
// void printer_manager_init();

//MessageQueue Structure

//Connection Queue


// Print
void print_print(int AC, int pid);
void print_terminate();
void print_end_spool(int pid);
void print_init_spool(int pid);
void print_init();

// Printer
void printer_main();
void printer_time();
void printer_init_spool();
void printer_init();
void printer_print();
void printer_dump_spool();
void printer_end_spool();
void printer_end_spool();
void printer_terminate();
#endif
