#include "computer.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#define BUFSIZ 100
#define MAX_BUFFER_SIZE 100

char terminate;
int sd;

void print_init(int CID) {

  int nbytes = 0;
  char buffer[BUFSIZ] = {0};

  nbytes = read(fd[0], buffer, BUFSIZ);

  if (nbytes < 0) {
    perror("read");
    exit(EXIT_FAILURE);
  }
  if (strcmp(buffer, ACK) != 0) {
    fprintf(stderr, "print_init: printer failed to initialize\n");
    exit(EXIT_FAILURE);
  } else {
    printf("Printer initialized successfully!\n");
  }
  int a1, loop, CONNECTION_PORT;
  FILE *fp = fopen("config.sys", "r");
  if (fp == NULL) {
    printf("Error opening config file\n");
    exit(1);
  }
  for (loop = 0; loop < 6; loop++) {
    fscanf(fp, "%d", &a1);
  }
  fscanf(fp, "%d", &CONNECTION_PORT);
  fclose(fp);
  // client socket creation
  struct sockaddr_in server_address;
  char rbuffer[100];
  sd = socket(AF_INET, SOCK_STREAM, 0);
  if (sd < 0) {
    perror("Socket creation failed");
    exit(EXIT_FAILURE);
  }
  int status = 0;
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(CONNECTION_PORT);
  server_address.sin_addr.s_addr = INADDR_ANY;
  server_address.sin_zero[8] = '\0';
  status =
      connect(sd, (struct sockaddr *)&server_address, sizeof(server_address));
  if (status < 0) {
    perror("Couldn't connect with the server");
    exit(EXIT_FAILURE);
  }
  read(sd, rbuffer, 100);
  printf("Client %s\n", rbuffer);
}

void print_init_spool(int pid) {
  int nbyte = 0;
  char buff[BUFSIZ] = {0};
  char pid_str[16];
  sprintf(pid_str, "%d", pid);

  if (pipe(fd) < 0) {
    perror("pipe");
    exit(1);
  }

  write(fd[1], pid_str, strlen(pid_str));
  close(fd[1]);
  printf("Opening spool file\n");
}

void print_print(int AC, int pid) {
  int nbyte = 0;
  int nbytes = 0;
  char buff[BUFSIZ] = {0};
  char buffs[BUFSIZ] = {0};
  char pid_str[16];
  char AC_str[16];
  sprintf(pid_str, "%d", pid);
  sprintf(AC_str, "%d", AC);
  if (pipe(fd) < 0) {
    perror("pipe");
    exit(1);
  }
  if (pipe(fd1) < 0) {
    perror("pipe");
    exit(1);
  }
  write(fd[1], pid_str, strlen(pid_str));
  write(fd1[1], AC_str, strlen(AC_str));
  close(fd[1]);
  close(fd1[1]);
}

void print_end_spool(int pid) {
  int nbyte = 0;
  char buff[BUFSIZ] = {0};
  char pid_str[16];
  sprintf(pid_str, "%d", pid);

  if (pipe(fd) < 0) {
    perror("pipe");
    exit(1);
  }
  write(fd[1], pid_str, strlen(pid_str));
  close(fd[1]);
  // open spool file and read its contents to store in a buffer and send it to
  // server socket
  if (pid != 0) {
    char spool_filename[32];
    sprintf(spool_filename, "spool_%s.txt", pid_str);
    FILE *spool_file = fopen(spool_filename, "r");
    if (spool_file == NULL) {
      perror("Error opening spool file");
      exit(EXIT_FAILURE);
    }
    int nbytes;
    char send_buffer[MAX_BUFFER_SIZE + 64] = {
        0}; // increase buffer size to include cid and pid
    sprintf(send_buffer, "CID=%d PID=%d ", CID,
            pid); // add cid and pid to the start of the buffer
   // printf("Contents of Computer-%d Process PID-%d are being sent to Printer "
     //      "Manager",
       //    CID, pid);
    //printf("\nprint loop\n");
    while ((nbytes = fread(send_buffer + strlen(send_buffer), 1,
                           MAX_BUFFER_SIZE, spool_file)) > 0) {
                           //memset(send_buffer, 0, MAX_BUFFER_SIZE + 64);
      // Send the contents of the buffer over the socket
      if (send(sd, send_buffer, strlen(send_buffer) + nbytes, 0) <
          0) { // send entire buffer including cid and pid
        perror("Error sending spool file contents");
        fclose(spool_file);
        exit(EXIT_FAILURE);
      }
    }
    
    //memset(send_buffer + strlen("CID=%d PID=%d "), 0, MAX_BUFFER_SIZE); // Clear the buffer after sending
  }
  printer_end_spool();
}

//void print_terminate() {
  //terminate = 'Y';
  //close(sd);
 // printer_terminate();
//}

void print_terminate() {
  char message[100];
  int terminate_value = 1;
  sprintf(message, "CID=%d Terminate=%d", CID, terminate_value);
  if (send(sd, message, strlen(message), 0) < 0) {
    perror("Error sending terminate signal");
  }
  terminate = 'Y';
  close(sd);
  printer_terminate();
}


