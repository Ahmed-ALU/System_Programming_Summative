#include<stdio.h>
#include<sys/wait.h>
#include<unistd.h>
#include<stdlib.h>

int main(int argc , char *argv[])
{   
    int id;
    printf("Tree of children processes\n");
    // printf('*************************\n');
    int counter = *argv[1] - '0';
    printf("The program will print %s children \n", argv[1]);
    
    for(int i=0;i<counter;i++) // loop will run argv[1] times
    {   
        int id = fork(); // Calling the fork()

        if (id == -1) { // if an error
        printf("An error happened while creating a fork/child\n");
        exit(0);
        }

        if(id == 0){ // a child
            printf("Hello, I'm a child, my pid is [%d] and my parent is [%d]\n",getpid(),getppid());
            exit(0); 
        }
	}
    int wpid;
    while ((wpid = wait(NULL)) > 0); ; // After finishing the loop, and all the children exit, the parent will print 
    printf("Hello, I'm the parent, my pid is [%d]\n",getpid());
    
    // printf('*************************\n');
}
