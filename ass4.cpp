// mmap()
// It is a system call in LINUX used to map a file or device in a process virtual memory space
// Once mapped, the file can be accessed like a normal array without explicit read() or write() system calls.
//  The OS handles loading pages from disk into RAM as needed.


#include <iostream>
#include <fcntl.h>      // File Control Options
#include <unistd.h>     // close() , ftruncate() , usleep() 
#include <sys/mman.h>   // Memory mapping functions : mmap() , munmap()
#include <cstdlib>      // rand() , srand() , exit()
#include <ctime>        // time() for seeding random numbeers
#include <cstdint>      // Fixed-width integer types: uint8_t
#include <cerrno>       // Error Handling


using namespace std;

#define FILE_NAME  "bigfile.bin"
#define FILE_SIZE  (8ULL * 1024 * 1024 * 1024)  // 8 GB


int main(){
    srand(time(nullptr));
    // rand() using current time

    // Open or Create file
    int fd = open(FILE_NAME , O_RDWR | O_CREAT , 0666);

    if(fd < 0){
        perror("open");
        return 1;
    }

    // If file was smaller , it extends it(extra space filled wiht zeroes)
    // If file wasd bigger , it truncates

    if(ftruncate(fd , FILE_SIZE) == -1){
        perror("ftruncate");
        return 1;
    }



    // Memory Map to File

    // nmap maps the file into memory as a sequence of bytes
    // Each byte is 8 bits
    // Using  , ( uint8_t* ) , let's you treat the mapped memory as an array of bytes


    uint8_t* mapped = (uint8_t*) mmap(nullptr , FILE_SIZE , PROT_READ | PROT_WRITE , MAP_SHARED , fd , 0 );
    
    // returns void

    // nullptr -> Let OS choose the first address
    // PROT_READ | PROT_WRITE  
    // asking for memory permissions: (i) Read form it  , (ii) Write from it
    
    // MAP_SHARED -> Updates we make in memory is written back to file
    // fd -> file descriptor of hte file you want to map (created / opened)

    //  offset from where mapping should be started

    if(mapped == "MAP_FAILED"){
        perror("mmap");
        return 1;
    }

    cout << "File mapped successfully. Accessing memory now ... " << endl;


    while(true){
        off_t F = rand() % FILE_SIZE;     // Offset: 0 to 8GB
        uint8_t  X = rand() % 256;        // Value: 0 to 255

        mapped[X] = F;
        // Writes the byte X to offset F in the memory
        
        uint8_t X_read = mapped[F]; 
        // Ensures Write Worker perfectly


        if(X != X_read){
            cerr << "Verfication failed at offset 0x" << hex << F << " value ( " <<(int)X << " , read = " << (int) X_read << ")" << endl;
            
            munmap(mapped , FILE_SIZE);
            // un map file from memory

            close(fd);
            return 1;
        }
    }

    cout << "Offset: 0x" << hex << F << " Value: " << dec << (int)X << " Verified" << endl;
    usleep(10000);        // Sleep 10ms to slow output
}
