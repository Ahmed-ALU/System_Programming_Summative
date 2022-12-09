#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
int func(int n);

int main(int argc , char *argv[])
{
    printf("Chain of children processes\n");
    // printf('*************************\n');
    int counter = *argv[1] - '0';
    func(counter); 
    return 0;
    // printf('*************************\n');
}

int func(int n) 
{
    if (n == 0) // End of the loop
    { 
        return 0; // Close
    }
    
    int id = fork(); // forked from here  
    
    if (id == -1) { // if an error
        printf("An error happened while creating a fork/child");
        exit(0);
    }
    if (id==0) { // if a child
        printf("I am a child,  My pid is [%d]  my parent pid is [%d]\n", getpid(), getppid() );
        n = n-1;
        func(n);
        exit(0);
    }
    else {
       wait(NULL); // This will make  all the parents waits for their children
    } 
    return 0;   
    // Note that: all the parents will exit only after all the children has been created.
    // Nite that: The children are also considered parents except last one
}
