//  Avoiding Deadlock using Thread Programming

// <-: DeadLock Avoidance Stratergy :->

// (i) Always acquire mutexes in a "consistent global order"
//   mutex1(total_1) -> mutex(total_2) -> mutex(total_3)

// (ii) Any thread that needs to lock multiple mutexes will lock
//      them in this order , regardless of which totals it is modifying

// (iii) This prevents circular wait condition ,which is the main
//        main cause of deadlocks

/*
Constraints:

(i)Each total is initially 100,000.
(ii) Must use mutex locks to protect them.
(iii)Must prevent deadlock by locking mutexes in the correct order.
(iv) The program runs forever and repeatedly prints consistent totals.

*/

#include <iostream>
#include <thread> // Class for multithreading
#include <mutex>  // For Mutual Exclusion
#include <random> // For generating random number safely in threads
#include <chrono> // TO manage timing/sleep operations
#include <iomanip>

using namespace std;

long long Total_1 = 100000, Total_2 = 100000, Total_3 = 100000;

// mutexes protecting each total
// Only one thread can lock a mutex at a time → prevents race conditions.
mutex mtx1, mtx2, mtx3;

// Random number generator
random_device rd;
mt19937 gen(rd());

//  DeadLock Free Locking Functions
void lock_in_order(mutex &a, mutex &b)
{
    if (&a < &b)
    {
        a.lock();
        b.lock();
    }
    else
    {
        b.lock();
        a.lock();
    }
}

void unlock_in_order(mutex &a, mutex &b)
{
    a.unlock();
    b.unlock();
}

// Thread 1: Move 1–10 units from Total_1 to Total_2 or Total_3
void threadFunc1()
{
    uniform_int_distribution<> dist_amount(1, 10);
    uniform_int_distribution<> dist_choice(0, 1);

    while (true)
    {
        int amt = dist_amount(gen);
        int choice = dist_choice(gen);

        if (choice == 0)
        {
            // Total_1 -> TOtal_2

            lock_in_order(mtx1, mtx2);
            Total_1 -= amt;
            Total_2 += amt;
            unlock_in_order(mtx1, mtx2);
        }
        else
        {
            // Total_1 -> Total_3;
            lock_in_order(mtx1, mtx3);
            Total_1 -= amt;
            Total_3 += amt;
            unlock_in_order(mtx1, mtx3);
        }
        this_thread::sleep_for(chrono::milliseconds(10));
    }
}

// Thread 2: Move 1–20 units from Total_2 to Total_1 or Total_3
void threadFunc2()
{
    uniform_int_distribution<> dist_amount(1, 20);
    uniform_int_distribution<> dist_choice(0, 1);

    while (true)
    {
        int amt = dist_amount(gen);
        int choice = dist_choice(gen);

        if (choice == 0)
        {
            // Total_2 -> Total_1

            lock_in_order(mtx1, mtx2);
            Total_2 -= amt;
            Total_1 += amt;
            unlock_in_order(mtx1, mtx2);
        }
        else
        {
            // Total_3 -> Total_1

            lock_in_order(mtx2, mtx3);
            Total_2 -= amt;
            Total_3 += amt;

            unlock_in_order(mtx2, mtx3);
        }
        this_thread::sleep_for(chrono::milliseconds(10));
    }
}

// Thread 3: Move 1–30 units from Total_3 to Total_1 or Total_2
void threadFunc3()
{
    uniform_int_distribution<> dist_amount(1, 30);
    uniform_int_distribution<> dist_choice(0, 1);

    while (true)
    {
        int amt = dist_amount(gen);
        int choice = dist_choice(gen);

        if (choice == 0)
        { // Total_3 -> Total_1
            lock_in_order(mtx1, mtx3);
            Total_3 -= amt;
            Total_1 += amt;
            unlock_in_order(mtx1, mtx3);
        }
        else
        { // Total_3 -> Total_2
            lock_in_order(mtx2, mtx3);
            Total_3 -= amt;
            Total_2 += amt;
            unlock_in_order(mtx2, mtx3);
        }

        this_thread::sleep_for(chrono::milliseconds(10));
    }
}

// Thread 4: Display totals and grand total

void displayFunc()
{
    while (true)
    {
        {
            lock_guard<mutex> lock1(mtx1);
            lock_guard<mutex> lock2(mtx2);
            lock_guard<mutex> lock3(mtx3);

            long long grand = Total_1 + Total_2 + Total_3;

            cout << fixed << setprecision(0)
                 << "Total_1 = " << setw(8) << Total_1
                 << " | Total_2 = " << setw(8) << Total_2
                 << " | Total_3 = " << setw(8) << Total_3
                 << " | Grand Total = " << grand << endl;
        }

        this_thread::sleep_for(chrono::milliseconds(300));
    }
}

int main()
{
    cout << "Starting DeadLock-Free Thread Simulation" << endl;

    thread th1(threadFunc1);
    thread th2(threadFunc2);
    thread th3(threadFunc3);
    thread th4(displayFunc);

    th1.join();
    th2.join();
    th3.join();
    th4.join();

    return 0;
}

