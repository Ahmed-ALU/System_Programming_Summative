#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <signal.h>

#define MAX_CLIENTS 100 // max number of client
#define BUFFER_SZ 2048

int client_count = 0; // number of client connecting
static int uid = 10;

/* Client structure */
typedef struct
{
  struct sockaddr_in address;
  int sockfd;
  int uid;
  char name[32];
} client_t;

// functions initilization
void print_client_addr(struct sockaddr_in addr);
void str_trim_lf(char *arr, int length);
void queue_add(client_t *cl);
void queue_remove(int uid);
void send_message(char *s, int uid);
void *handle_client(void *arg);


client_t *clients[MAX_CLIENTS]; // collection of all clients
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    printf("Usage: %s <port>\n", argv[0]);
    return EXIT_FAILURE;
  }

  char *ip = "127.0.0.1";   // local address of server
  int port = atoi(argv[1]); // port of the server
  int option = 1;
  int listen_file_discriptor = 0, connfd = 0;
  struct sockaddr_in serv_address;
  struct sockaddr_in cli_address;
  pthread_t tid;

  // Server socket settings
  // create tcp server socket
  listen_file_discriptor = socket(AF_INET, SOCK_STREAM, 0);
  serv_address.sin_family = AF_INET;
  serv_address.sin_addr.s_addr = inet_addr(ip);
  serv_address.sin_port = htons(port);

  // bind the address *ip and port number
  if (bind(listen_file_discriptor, (struct sockaddr *)&serv_address, sizeof(serv_address)) < 0)
  {
    perror("ERROR: Socket binding failed");
    return EXIT_FAILURE;
  }

  /* Listen to the server */
  if (listen(listen_file_discriptor, 10) < 0)
  {
    perror("ERROR: Socket listening failed");
    return EXIT_FAILURE;
  }

  printf("!!!! BIENVENUE A NOTRE CHAMBRE DE TCHAT!!!!\n");

  while (1) // while loop to handle exchange of message btn servers
  {
    socklen_t clilen = sizeof(cli_address);
    // accept connection from the client server
    connfd = accept(listen_file_discriptor, (struct sockaddr *)&cli_address, &clilen);

    /* Check if max clients is reached */
    if ((client_count + 1) == MAX_CLIENTS)
    {
      printf("Max clients reached. Rejected: ");
      print_client_addr(cli_address);
      printf(":%d\n", cli_address.sin_port);
      close(connfd);
      continue;
    }

    /* Client settings */
    client_t *cli = (client_t *)malloc(sizeof(client_t));
    cli->address = cli_address;
    cli->sockfd = connfd;
    cli->uid = uid++;

    // Add client to the queue
    queue_add(cli);
    // create thread to handle the client connecting to server
    pthread_create(&tid, NULL, &handle_client, (void *)cli);

    // reduce cpu usage
    sleep(1);
  }

  return EXIT_SUCCESS;
}

// helper function to trim user input and remove next line char
void str_trim_lf(char *arr, int length)
{
  int i;
  for (i = 0; i < length; i++)
  { // trim \n
    if (arr[i] == '\n')
    {
      arr[i] = '\0';
      break;
    }
  }
}

// helper function to print client ip address
void print_client_addr(struct sockaddr_in addr)
{
  printf("%d.%d.%d.%d",
         addr.sin_addr.s_addr & 0xff,
         (addr.sin_addr.s_addr & 0xff00) >> 8,
         (addr.sin_addr.s_addr & 0xff0000) >> 16,
         (addr.sin_addr.s_addr & 0xff000000) >> 24);
}

/* Add clients to array (queue) */
void queue_add(client_t *cl)
{
  pthread_mutex_lock(&clients_mutex);

  for (int i = 0; i < MAX_CLIENTS; ++i)
  {
    if (!clients[i])
    {
      clients[i] = cl;
      break;
    }
  }

  pthread_mutex_unlock(&clients_mutex);
}

/* Remove clients to array (queue) */
void queue_remove(int uid)
{
  pthread_mutex_lock(&clients_mutex);

  for (int i = 0; i < MAX_CLIENTS; ++i)
  {
    if (clients[i])
    {
      if (clients[i]->uid == uid)
      {
        clients[i] = NULL;
        break;
      }
    }
  }

  pthread_mutex_unlock(&clients_mutex);
}

/* helper function to send message to all clients except sender */
void send_message(char *s, int uid)
{
  pthread_mutex_lock(&clients_mutex); // lock the thread

  for (int i = 0; i < MAX_CLIENTS; ++i)
  {
    if (clients[i])
    {
      if (clients[i]->uid != uid) // check if uid is not equal to id of sender
      {
        if (write(clients[i]->sockfd, s, strlen(s)) < 0) // catch if no message was sent
        {
          perror("ERROR: write to descriptor failed");
          break;
        }
      }
    }
  }

  pthread_mutex_unlock(&clients_mutex); // unlock thread
}

/* helper function to handle all communication with the client */
void *handle_client(void *arg)
{
  char buff_out[BUFFER_SZ];
  char name[32];      // name of client
  int leave_flag = 0; // flag to identify if client is connected or not

  client_count++; // increment number of client on each connection
  client_t *cli = (client_t *)arg;

  // get client name
  if (recv(cli->sockfd, name, 32, 0) <= 0 || strlen(name) < 2 || strlen(name) >= 32 - 1)
  {
    printf("Didn't enter the name.\n");
    leave_flag = 1;
  }
  else
  {
    strcpy(cli->name, name);
    sprintf(buff_out, "%s has joined\n", cli->name);
    printf("%s", buff_out);
    send_message(buff_out, cli->uid); // send message to other client
  }

  bzero(buff_out, BUFFER_SZ);

  while (1) // while loop to handle message connection between client server
  {
    if (leave_flag) // catch server flag when it is not running
    {
      break;
    }
    // handling receive messages from the client servers
    int receive = recv(cli->sockfd, buff_out, BUFFER_SZ, 0);
    if (receive > 0)
    {
      if (strlen(buff_out) > 0)
      {
        send_message(buff_out, cli->uid); // send message to other clients
      }
    }
    else if (receive == 0 || strcmp(buff_out, "exit") == 0) // catch response when user leaves the chat room
    {
      sprintf(buff_out, "%s has left\n", cli->name);
      printf("%s", buff_out);
      send_message(buff_out, cli->uid); // send message to other about user who left
      leave_flag = 1;
    }
    else
    { // catch when there is an error on the server
      printf("ERROR: -1\n");
      leave_flag = 1;
    }

    bzero(buff_out, BUFFER_SZ);
  }

  // Delete client from array (queue) and close thread
  close(cli->sockfd);
  // remove client from array
  queue_remove(cli->uid);
  // free malloc memory allocated to the client
  free(cli);
  // decrement number of client remaining of chat room
  client_count--;
  // close the thread
  pthread_detach(pthread_self());

  return NULL;
}

