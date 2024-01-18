#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>


#define CONNECTION_PORT 3512
#define QUEUE_SIZE 10
#define mqs 10

typedef struct {
    char messages[mqs][5000];
    int front, rear;
    pthread_mutex_t mutex;
} MessageQueue;

MessageQueue message_queues[mqs];
sem_t sync_pc;
pthread_mutex_t printer_mutex;
sem_t queue_semaphore;
int connection_queue[QUEUE_SIZE];
int front = 0, rear = 0;
FILE *fp;

bool is_message_queue_empty(int queue_index) {
    if (queue_index < 0 || queue_index >= mqs) {
        return true;
    }

    MessageQueue *queue = &message_queues[queue_index];
    return queue->front == queue->rear;
}

void add_connection_to_queue(int client_socket) {
  sem_wait(&queue_semaphore);
  if (rear == QUEUE_SIZE) {
    printf("Connection queue is full. Dropping connection.\n");
    close(client_socket);
  } else {
    connection_queue[rear++] = client_socket;
    printf("Connection added to queue. Queue size: %d\n", rear - front);
  }
  sem_post(&queue_semaphore);
}

int remove_connection_from_queue() {
  sem_wait(&queue_semaphore);
  if (front == rear) {
    sem_post(&queue_semaphore);
    return -1;
  } else {
    int client_socket = connection_queue[front++];
    printf("Connection removed from queue. Queue size: %d\n", rear - front);
    sem_post(&queue_semaphore);
    return client_socket;
  }
}



void init_message_queues() {
    int i;
    for (i = 0; i < mqs; i++) {
        message_queues[i].front = 0;
        message_queues[i].rear = 0;
        pthread_mutex_init(&message_queues[i].mutex, NULL);
    }
}

void add_message_to_queue(int index, char *message) {
    pthread_mutex_lock(&message_queues[index].mutex);
    if ((message_queues[index].rear + 1) % mqs == message_queues[index].front) {
        printf("Message queue is full. Dropping message.\n");
    } else {
        strcpy(message_queues[index].messages[message_queues[index].rear], message);
        message_queues[index].rear = (message_queues[index].rear + 1) % mqs;
    }
    pthread_mutex_unlock(&message_queues[index].mutex);
}

void print_message_queue(int index) {
    pthread_mutex_lock(&message_queues[index].mutex);

    printf("Message Queue (Thread: %d):\n", index);
    int i = message_queues[index].front;
    while (i != message_queues[index].rear) {
        printf("  Message: %s\n", message_queues[index].messages[i]);
        i = (i + 1) % mqs;
    }

    pthread_mutex_unlock(&message_queues[index].mutex);
}


void *client_handler(void *arg) {
  int index = *((int *)arg);

  while (1) {
    int client_socket = remove_connection_from_queue();
    if (client_socket != -1) {
      char storage_buffer[5000];
      int termination = 0;

      // Keep reading messages from the client until termination
      while (!termination) {
        ssize_t bytes_read =
            recv(client_socket, storage_buffer, sizeof(storage_buffer) - 1, 0);
        if (bytes_read > 0) {
          storage_buffer[bytes_read] = '\0';
          int cid, pid;
          sscanf(storage_buffer, "CID=%d PID=%d", &cid, &pid);
          
          // Check for termination signal
          if (strstr(storage_buffer, "TERMINATE") != NULL) {
            termination = 1;
            printf("Received termination signal from client (CID=%d)\n", cid);
          } else {
            printf("Message from client (CID=%d, PID=%d): %s \n", cid, pid,
                   storage_buffer);

            // Add the received message to the message queue
            add_message_to_queue(index, storage_buffer);
          }
        } else {
          // If read fails or connection is closed, terminate
          termination = 1;
        }
      }
      printf("Client connection closed\n");
      // Close the connection
      close(client_socket);
      // Print message queue
      print_message_queue(index);
    }
  }
  return NULL;
}

void printer_init() {
  fp = fopen("printer.out", "a");
  if (fp == NULL) {
    perror("Error opening file");
    exit(EXIT_FAILURE);
  }
  // Initialize sync_pc semaphore for synchronizing between the printer and the communicators
  sem_init(&sync_pc, 0, 0);
  // Initialize printer_mutex for protecting the sync_pc semaphore
  pthread_mutex_init(&printer_mutex, NULL);
  // Print a message to indicate the readiness of the printer
  printf("Printer is ready.\n");
}

void printer_main() {
  int termination_flag = 0;
  int i;
  for (i = 0; i < mqs; i++) {
    // Sleep for 1 second before checking for new messages
    sleep(1);

    pthread_mutex_lock(&printer_mutex);
    // Find non-empty message queues
    int non_empty_queues[mqs];
    int num_non_empty_queues = 0;
    for (i = 0; i < mqs; i++) {
      pthread_mutex_lock(&message_queues[i].mutex);
      if (message_queues[i].front != message_queues[i].rear) {
        non_empty_queues[num_non_empty_queues++] = i;
      }
      pthread_mutex_unlock(&message_queues[i].mutex);
    }
    // Read printer commands from non-empty message queues
    for (i = 0; i < num_non_empty_queues; i++) {
      int queue_index = non_empty_queues[i];
      pthread_mutex_lock(&message_queues[queue_index].mutex);
      while (message_queues[queue_index].front != message_queues[queue_index].rear) {
        fprintf(fp, "%s\n", message_queues[queue_index].messages[message_queues[queue_index].front]);
        message_queues[queue_index].front = (message_queues[queue_index].front + 1) % mqs;
      }
      pthread_mutex_unlock(&message_queues[queue_index].mutex);
    }
    pthread_mutex_unlock(&printer_mutex);

    // Flush the file stream after writing to it
    fflush(fp);
  }
  // Clean up
  pthread_mutex_destroy(&printer_mutex);
  fclose(fp);
}


void printer_manager_init() {
  int socket_descriptor;
  int client_socket;
  int length_of_address;
  int option_value = 1;
  struct sockaddr_in server_address;
  struct sockaddr_in connection_address;
  char *message = "This is a message from the server";

  socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_descriptor < 0) {
    perror("Socket creation failed\n");
    exit(EXIT_FAILURE);
  }

  int status = setsockopt(socket_descriptor, SOL_SOCKET, SO_REUSEADDR,
                          &option_value, sizeof(option_value));
  if (status < 0) {
    perror("Couldn't set options\n");
    exit(EXIT_FAILURE);
  }

  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(CONNECTION_PORT);
  server_address.sin_addr.s_addr = INADDR_ANY;
  bzero(&(server_address.sin_zero), 8);

  status = bind(socket_descriptor, (struct sockaddr *)&server_address,
                sizeof(struct sockaddr));
  if (status < 0) {
    perror("Couldn't bind socket");
    exit(EXIT_FAILURE);
  }

  status = listen(socket_descriptor, 4);
  if (status < 0) {
    perror("Couldn't listen for connections");
    exit(EXIT_FAILURE);
  }
  sem_init(&queue_semaphore, 0, 1);

  int num_threads = 4;
  pthread_t printer_threads[num_threads];
  //for (int i = 0; i < num_threads; i++) {
    //pthread_create(&printer_threads[i], NULL, client_handler, NULL);
  //}
  int i;
for (i = 0; i < num_threads; i++) {
  int *index = malloc(sizeof(int));
  *index = i;
  pthread_create(&printer_threads[i], NULL, client_handler, (void *)index);
}


  printf("Printer Manager initialized. Listening for connections on port %d...\n",CONNECTION_PORT);

  while (1) {
    length_of_address = sizeof(connection_address);
    client_socket =
        accept(socket_descriptor, (struct sockaddr *)&connection_address,
               &length_of_address);
    if (client_socket < 0) {
      perror("Couldn't accept connection");
      exit(EXIT_FAILURE);
    } else {
      printf("Accepted new connection from %s:%d\n",
             inet_ntoa(connection_address.sin_addr),
             ntohs(connection_address.sin_port));
      send(client_socket, message, strlen(message), 0);
      add_connection_to_queue(client_socket);
    }
  }

  sem_destroy(&queue_semaphore);
}

void printer_manager() { printer_manager_init(); }


int main() {
    printer_init();
    init_message_queues();

    // Create separate threads for the printer_manager and printer_main functions
    pthread_t printer_manager_thread, printer_main_thread;

    if (pthread_create(&printer_manager_thread, NULL, (void *)printer_manager, NULL) != 0) {
        perror("Failed to create printer_manager thread");
        exit(EXIT_FAILURE);
    }

    if (pthread_create(&printer_main_thread, NULL, (void *)printer_main, NULL) != 0) {
        perror("Failed to create printer_main thread");
        exit(EXIT_FAILURE);
    }

    // Wait for both threads to finish (although they will run indefinitely in this case)
    pthread_join(printer_manager_thread, NULL);
    pthread_join(printer_main_thread, NULL);
}


