/*
(1) Creating two named pipes(FIFOs):
    (a) fifo_parent_to_child -> For parent -> child communication
    (b) fifo_child_to_parent -> For Child -> Parent Communication

(2) Forks a child process using fork()

(3) Parent Process:
    (a) Reads a 1GB source file.
    (b) Sends it to the child process through FIFO1
    (c) Receives the file back from the child through FIFO2
    (d) Measures the total round-trip transfer time

(4) Child Process:
    (a) Receives data from the parent via FIFO1
    (b) Writes a copy to a temporary file 
    (c) Sends the same data back to the parent via FIFO2
*/


#include<iostream>
#include<sys/types.h>      // System Types (pid_t , ssize_t)
#include<sys/stat.h>       // File Attributes and permissions( "mkfifo() permissions" )
#include<fcntl.h>          // File Open Flags

#include<unistd.h>         // POSIX System Calls (fork(), read(), write(), close(), sleep())
#include<cstring>          // C-Style string functions (strerror() , perror())
#include<chrono>
#include<cstdlib>          // exit() , system() 

using namespace std;

#define FIFO1 "fifo_parent_to_child"
#define FIFO2 "fifo_child_to_parent"
#define BUFFER_SIZE 4096

int main(){
    // Create FIFOs

    // mkfifo() -> Creates a named pipe with read/write operations.
    // 0666 -> Allows all users to read/write.
    // errno != EXIST -> ignores "already exists" error.

    if(mkfifo(FIFO1 , 0666) == -1 && errno != EEXIST){
        perror("mkfifo FIFO1");
    }
    if(mkfifo(FIFO2 , 0666) == -1 && errno!= EEXIST){
        perror("mkfifo FIFO2");
    }


    // Fork a Child Process
    pid_t pid = fork();

    if(pid < 0){
        perror("fork");
        return 1;
    }

    auto start = chrono::high_resolution_clock::now();

    if(pid >0){
        // Parent Process
        cout << "Parent Process Started ....." << endl;

        int fd_write = open(FIFO1 , O_WRONLY);
        int fd_read = open(FIFO2 , O_RDONLY);

        if(fd_write < 0 || fd_read < 0){
            perror("Open FIFO");
            return 1;
        }

        // Open the 1GB Source File

        int src = open("source_1GB.bin" , O_RDONLY);
        if(src < 0){
            perror("Open Source File");
            return 1;
        }

        // Send file to child 
        char buffer[BUFFER_SIZE];
        ssize_t bytes;

        while((bytes = read(src,buffer,BUFFER_SIZE)) > 0){
            write(fd_write , buffer , bytes);
        }
        close(src);
        close(fd_write);

        cout << "Parent -> Child Transfer Complete" << endl;

        // Receive file back from Child 
        int dest = open("returned_1GB.bin" , O_WRONLY | O_CREAT | O_TRUNC , 0666);
        while((bytes = read(fd_read , buffer , BUFFER_SIZE)) > 0){
            write(dest , buffer , bytes);
        }
        close(dest);
        close(fd_read);


        
        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double> elapsed = end - start;
        cout << "Child -> Parent transfer complete.\n";
        cout << "Total round-trip time: " << elapsed.count() << " seconds.\n";

        // Compare files
        cout << "\nComparing files using diff...\n";
        system("diff source_1GB.bin returned_1GB.bin > compare_result.txt");
        cout << "Comparison complete. Check compare_result.txt\n";
    }

    else{
        //  CHILD PROCESS 
        cout << "Child process started...\n";

        int fd_read  = open(FIFO1, O_RDONLY);
        int fd_write = open(FIFO2, O_WRONLY);
        if (fd_read < 0 || fd_write < 0) {
            perror("open FIFO");
            exit(1);
        }

        // Temporary file to save copy
        int temp = open("temp_child_copy.bin", O_WRONLY | O_CREAT | O_TRUNC, 0666);
        char buffer[BUFFER_SIZE];
        ssize_t bytes;

        // Read from parent, write to temp and FIFO2
        while ((bytes = read(fd_read, buffer, BUFFER_SIZE)) > 0) {
            write(temp, buffer, bytes);
            write(fd_write, buffer, bytes);
        }

        close(fd_read);
        close(fd_write);
        close(temp);
        cout << "Child process done transferring file.\n";
        exit(0);
    }

    return 0;
    }


