#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define LENGTH 2048

// Functions Initilization
void handle_send();
void handle_receive();
void exit_signal(int signal_n);
void trim (char* arr, int len);

// The global variables
int socket_file_descriptor = 0; // Signal File descriptor
int flag = 0; // For signal handling
char user_name[32]; //for user input 
struct sockaddr_in server_address;
pthread_t thread_send;
pthread_t thread_receive;


// The following is the main TCP, client part algorithms


int main(int argc, char **argv){
	// If the number of arguments is less than two, because we need to call the file and the 
	//port number as well to the argv
	if(argc != 2){
		printf("Please enter a full correct argument");
		printf("(./server or ./ckient + port_number)");
		return EXIT_FAILURE;
	}

	char *ip_address = "127.0.0.1";
	int port = atoi(argv[1]); // argv[1] is the port number, 
	//the input from the terminal is a string, so we are converting it to an int using atoi()

	signal(SIGINT, exit_signal);

	// Read The name of the user so that we can use it in the chat 
	printf("What is your name? ( more than 2 chars and less than 30 chars): ");
	fgets(user_name, 32, stdin);
	trim(user_name, strlen(user_name));
	
	// socket settings
	socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = inet_addr(ip_address); // should be the same as the server file
	server_address.sin_port = htons(port); // should be the same as the server file


  // Connecting to Server
	int err = connect(socket_file_descriptor, (struct sockaddr *)&server_address, sizeof(server_address));

	if (err == -1) {
		printf("ERROR: connect\n");
		return EXIT_FAILURE;
	}

	// Everything is ok? Send name
	send(socket_file_descriptor, user_name, 32, 0);
	printf("!!!!!! BIENVENUE A NOTRE CHAMBRE DE TCHAT !!!!!!\n");

	// Multithreading part

	// threading message sending by threading handle send func
	if(pthread_create(&thread_send, NULL, (void *) handle_send, NULL) != 0){
		printf("pthread error\n");
	return EXIT_FAILURE;
	}

	// threading message reciving from the server by threading handle receive func
	if(pthread_create(&thread_receive, NULL, (void *) handle_receive, NULL) != 0){
		printf("pthread error\n");
		return EXIT_FAILURE;
	}

	// in case an exit ctrl+c was called from the client side...
	while (1){
		if(flag){
			printf("Turn off the chat \n");
			break;
	}
	}

	// close the socket file after canceling/exitting
	close(socket_file_descriptor);
	return EXIT_SUCCESS;
	}



// The following are the helping functions

void exit_signal(int signal_n) {
    flag = 1;
}

// We found that we usually get \n at the end of sent text or 
// between, whcih we found it must be fixed by this function
void trim (char* arr, int len) {
  int i;
  for (i = 0; i < len; i++) { 
	if (arr[i] == '\n') {
    // change with end of line
	arr[i] = '\0';
    break;
    }
  }
}

void handle_receive() {
	char message[LENGTH] = {};

  while (1) {
	
	// Read the bytes from the buffer to the defined socket by 
	int rec = recv(socket_file_descriptor, message, LENGTH, 0);
    
	if (rec > 0) { // if sucess
      printf("%s", message); // to the terminal or stdout
      fflush(stdout); //clean the stdout
	
    } else if (rec == 0) { // If no message
			break;
    }
	memset(message, 0, sizeof(message)); // We had to use it as the variables used to have arbitrary value upon creation, causing a strange text to appear.
  }
}

void handle_send() { // this function is to handle getting the message from the client and sending it to the server
  char message[LENGTH] = {};
	char buffer[LENGTH + 64] = {}; 

  while(1) {
  	// Just in case
	fflush(stdout);
	// Read/wait for the message from the user in the terminal as long as it is open
    fgets(message, LENGTH, stdin);
	// trim the \n from the message text
    trim(message, LENGTH);
	// put the messag on the buffer
	sprintf(buffer, "%s: %s\n", user_name, message);
	// Send the message
	send(socket_file_descriptor, buffer, strlen(buffer), 0);
    // set the message and the buffer vars to the original sizes
	bzero(message, LENGTH);
    bzero(buffer, LENGTH + 64);
  }
  exit_signal(2); // to exit if ctrl+c pressed
}


