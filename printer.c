#include "computer.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int fd[2];
int fd1[2];

void printer_init() {
  int nbytes = 0;
  char buffer[BUFSIZ] = {0};
  if (pipe(fd) == -1) {
    perror("pipe");
    exit(EXIT_FAILURE);
  }
  if (write(fd[1], ACK, strlen(ACK)) == -1) {
    perror("write");
    exit(EXIT_FAILURE);
  }

  printf("Initialising printer file\n");
  //FILE *printer_out = fopen("printer.out", "w");
 // if (printer_out == NULL) {
   // perror("Failed to open printer.out file");
    //exit(EXIT_FAILURE);
  //}
}

void printer_init_spool() {
  int nbyte = 0;
  char buff[BUFSIZ] = {0};
  nbyte = read(fd[0], buff, BUFSIZ);

  char spool_filename[256] = {0};

  sprintf(spool_filename, "spool_%s.txt",buff);

  FILE *spool_file = fopen(spool_filename, "w");
  if (spool_file == NULL) {

    return;
  }
  close(fd[0]);
  fclose(spool_file);
}

void printer_print() {

  char AC_str[16];
  char pid_str[16];
  int nbyte = read(fd[0], pid_str, 16);
  printf("spool file pid %s", pid_str);
  pid_str[nbyte] = '\0';
  int nbytes = read(fd1[0], AC_str, 16);
  AC_str[nbytes] = '\0';
  printf("spool file AC %s\n", AC_str);

  char spool_filename[256] = {0};
  sprintf(spool_filename, "spool_%s.txt", pid_str);
  FILE *spool_file = fopen(spool_filename, "a");
  if (spool_file == NULL) {
    printf("Error: Could not open spool file %s\n", spool_filename);
    return;
  }

  fprintf(spool_file, "AC:%s\n", AC_str);
  fclose(spool_file);
}

int a, b, pt;

void printer_time() {
  FILE *fp = fopen("config.sys", "r");
  if (fp == NULL) {
    printf("Error opening config file\n");
    exit(1);
  }
  fscanf(fp, "%d", &a);
  fscanf(fp, "%d", &b);
  fscanf(fp, "%d", &pt);

  fclose(fp);
}

void printer_end_spool() {
  //char pid_str[16];
  //int nbyte = read(fd[0], pid_str, 16);
  //char spool_filename[256] = {0};
 // char c = '\0';
  //FILE *printer_out_file = fopen("printer.out", "w");
  //sprintf(spool_filename, "spool_%s.txt", pid_str);
  //FILE *spool_file = fopen(spool_filename, "r");
  //if (spool_file == NULL) {
   // printf("Error: Could not open spool file %s\n", spool_filename);
    //return;
  //}

  //while ((c = fgetc(spool_file)) != EOF) {
    //fputc(c, printer_out_file);
  //}
  //char line[BUFSIZ];

 // fclose(spool_file);
  //fclose(printer_out_file);
}

void printer_terminate() { printf("Printer terminated\n"); }

void printer_main() { printer_init(); }

void printer_dump_spool() {
  int j = 0;
  printf("===================\n");
  printf("Printer Dump\n");
  printf("===================\n");
  printf("Index: PID\n");
  int pid;
  for (pid = 1; pid < MAX_PROCESSES; pid++) {
    char spool_filename[256] = {0};

    sprintf(spool_filename, "spool_%d.txt", pid);

    FILE *spool_file = fopen(spool_filename, "r");
    if (spool_file != NULL) {
      printf("%d  %d\n", j, pid);
      fclose(spool_file);
      j++;
    }
  }
}

