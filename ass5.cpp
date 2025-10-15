//  Shared Memory
//  Shared memory allows multiple processes (or threads in some cases)
//  to access the same memory space

#include <iostream>
#include <thread> // for multithreading
#include <mutex>  // for thread synchronization
#include <condition_variable>
#include <cstdlib>
#include <sys/ipc.h> // for shared memory location
#include <sys/shm.h> // same as above
#include <unistd.h>  // header file in C/C++ for POSIX (Unix-like) systems.
#include <iomanip>

#define MAX_PAIRS 100

struct SharedData
{
    int X[MAX_PAIRS];
    int Y[MAX_PAIRS];
    int A[MAX_PAIRS];
    int B[MAX_PAIRS];

    double C[MAX_PAIRS];

    bool computedA[MAX_PAIRS];
    bool computedB[MAX_PAIRS];
    bool computedC[MAX_PAIRS];

    // computedA/B/C are boolean flags to indicate if a particular thread has finished computing a value.

    int n; // Number of (X,Y) pairs.
};

SharedData *shm_ptr;              // pointer to shared memory
mutex mtx;                        // Avoids of Race Condition
condition_variable cvA, cvB, cvC; // For respective threads to get notified

// A = X*Y
void computeA()
{
    for (int i = 0; i < shm_ptr->n; i++)
    {
        uniqe_lock<mutex> lock(mtx);
        shm_ptr->A[i] = shm_ptr->X[i] * shm_ptr->Y[i];
        shm_ptr->computedA[i] = true;

        cvA.notify_all(); // notify C
        lock.unlock();
        usleep(10000);

        // Simulates computation delay to better demonstrate parallel execution
    }
}

//  B = 2*X + 2*Y + 1
void computeB()
{
    for (int i = 0; i < shm_ptr->n; i++)
    {
        unique_lock<mutex> lock(mtx);
        shm_ptr->B[i] = 2 * shm_ptr->X[i] + 2 * shm_ptr->Y[i] + 1;
        shm_ptr->computedB[i] = true;

        cvB.notify_all();
        lock.unlock();

        usleep(10000);
    }
}

//  C = B / A

void computeC()
{
    for (int i = 0; i < shm_ptr->n; i++)
    {
        unique_lock<mutex> lock(mtx);

        // Wait for A[i] to be computed
        while (!shm_ptr->computedA[i])
        {
            cvA.wait(lock); // Unlcok mutex and waits
        }

        // Wait for B[i] to be computed
        while (!shm_ptr->computedB)
        {
            cvB.wait(lock);
        }

        // Compute C[i]

        if (shm_ptr->A[i] != 0)
        {
            shm_ptr->C[i] = shm_ptr->B[i] / shm_ptr->A[i];
        }
        else
        {
            shm_ptr->C[i] = 0;
        }

        shm_ptr->computedC[i] = true;
        ;
        cvC.notify_all();

        lock.unlock();
        usleep(10000);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cout << "Usage : " << argv[0] << " < number of random pairs>" << endl;
        return 1;
    }

    int n = stoi(argv[1]);

    if (n <= 0 || n > MAX_PAIRS)
    {
        cout << " Number of pairs must be between 1 and " << MAX_PAIRS << endl;
        return 1;
    }

    // File to Key
    key_t key = ftok("shmfile", 65);

    // Shared Memory Segment
    // 0666 -> file permissions
    // IPC_CREAT -> Flag to create the segment if it doesn’t already exist

    int shmid = shmget(key, sizeof(SharedData), 0666 | IPC_CREAT);

    //  shmat() → Attach shared memory segment to your process’s address space.
    shm_ptr = (SharedData *)shmat(shmid, NULL, 0); // (id , OS to choose where to attach , flag)

    if (shm_ptr == (void *)-1)
    {
        perror("shmat");
        return 1;
    }

    // Initialize shared memory with random values
    shm_ptr->n = n;
    for (int i = 0; i < n; i++)
    {
        shm_ptr->X[i] = rand() % 10;
        shm_ptr->Y[i] = rand() % 10;

        shm_ptr->computedA[i] = false;
        shm_ptr->computedB[i] = false;
        shm_ptr->computedC[i] = false;
    }

    // Create Threads
    thread Th1(computeA);
    thread Th2(computeB);
    thread Th3(computeC);

    // Wait for threads to join
    Th1.join();
    Th2.join();
    Th3.join();

    // Print results
    cout << "Pairs(X,Y) | A | B | C\n";
    cout << "-------------------------------\n";
    for (int i = 0; i < n; i++)
    {
        cout << "(" << shm_ptr->X[i] << "," << shm_ptr->Y[i] << ") | "
             << shm_ptr->A[i] << " | " << shm_ptr->B[i] << " | "
             << fixed << setprecision(2) << shm_ptr->C[i] << "\n";
    }

    //  Detach shared memory from this process
    shmdt(shm_ptr);

    // SHared Memory ConTroL
    shmct(shmid, IPC_RMID, NULL);
    // IPC_RMID: “Remove (delete) this shared memory segment”
    return 0;
}
