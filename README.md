# C_Process_Creation_and_Termination
********************************
***This repository contains The summative projetcs for System programming course, Year 3 at ALU. There are two projects, the frist one is abut processes creation and termination in Unix using Fork() and Wait() Syscalls, and the second one is about...***
******************************


##  Project 01
*********************************

- You have been tasked to develop a program tar process creation and termination on a Linux platform. 

- You are required to create multiple child processes to work under one parent process. 

- In principle, child processes can do their own work independently or cooperatively to accomplish a task. 

- In this assignment, these child processes simply print out a "hello" message together with their PIDs (process IDs) and exit. 

- You should use fork() and wait() system calls to actualize this program.

*************************
**Requirements** 
        
        1- Take the number of child processes as an argument when the parent process creates child processes. This argument should be passed through a command line argument. 
        2- The parent process creates child processes and should print out an error message if creation fails. 
        3- The parent process should also wait far all child processes to finish and then exit.
        4- Each child process should print out a hello message together with its PID and then exit. 
        5- Test with 2, 4, and 8 child processes. 

**********************************
### Next,
instead of creating multiple child processes of a parent process, you are asked to create a **chain of processes**. That is to say, the parent process will create one child and watt for it to finish, while the child creates its own child and wait for it as well, and so on. 

The last child created should print out the message and exit immediately so that its ancestors can finish too. Test your program with 2, 4, and 8 child processes again.

## Required Files
***********************
- Develop a **Make-file**, to automate the compilation process. 


- Develop a **shell script** to automate the test process, i.e. to test Wth 2, 4, and 8 children processes for both versions of the program automatically with this script. 
