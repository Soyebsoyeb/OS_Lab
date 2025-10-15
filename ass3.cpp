/*
Parallel Matrix Multiplication using C++ threads


Program Description:
- Multiplies two large square matrices (dimension specified by user).
- Parallelized using std::thread.
- Measures elapsed time for multiplication only (matrix initialization excluded).

Command Line Arguments:
1. Matrix dimension (N)
2. Number of parallel threads
3. Mod value for random initialization
4. Print switch (1 = print matrices, 0 = do not print)

Example usage:
./matrix_mult 3000 4 100 0


 
Summation formula:->

   C[i][j] = (sum from k=0 to n-1)(A[i][k] * B[k][j]);


*/



#include<iostream>
#include<vector>
#include<thread>       // Multithreading Support
#include<random>  
#include<chrono>      
#include<cstdlib>     // for atoi()

using namespace std;

void multiply_chunk(const vector<unsigned int>&A , const vector<unsigned int>&B ,
    vector<unsigned long long>& C , int N , int start_row , int end_row){


        // Using flat 1D-arrays for Cache Efficiency

        for(int i = start_row ; i<end_row ; i++){
            for(int j=0; j<N ; j++){
                unsigned long long sum = 0;

                for(int k=0; k<N ; k++){
                    sum += A[i*N+k]*B[k*N+j];
                }
                C[i*N + j] = sum;
            }
        }
    }



    // Argument count , Argument Vector
    int main(int argc , char* argv[]){
        // atoi() -> ASCII to integer

        if(argc != 5){
            cerr << "Usage: " << argv[0] << "<dimension> <threads> <mod> <print_switch>" << endl;
            return 1;
        }

        int N = atoi(argv[1]);
        int num_threads = atoi(argv[2]);
        int mod_value = atoi(argv[3]);
        int print_switch = atoi(argv[4]);


        cout << "Matrix Dimension: " << N << "x" << N << endl;
        cout << "Number of Threads: " << num_threads << endl;
        cout << "Mod Value: " << mod_value << endl;
        cout << "Print Switch: " << print_switch << endl;

        // Initialize matrices with random numbers

        vector<unsigned int> A(N*N) , B(N*N);
        vector<unsigned long long> C(N*N , 0);

        random_device(rd);
        mt19937 gen(rd());

        uniform_int_distribution<unsigned int> dis(0,mod_value-1);

        for(int i=0; i<N*N ; i++){
            A[i] = dis(gen);
            B[i] = dis(gen);
        }

        if(print_switch == 1){
            cout << "Matrix A" << endl;

            for(int i=0; i<N; i++){
                for(int j=0;j<N;j++){
                    cout << A[i*N + j] << " ";
                }
                cout << endl;
            }

            cout << "Matrix B" << endl;

            for(int i=0; i<N; i++){
                for(int j=0;j<N;j++){
                    cout << B[i*N + j] << " ";
                }
                cout << endl;
            }
        }

        // Start Timer
        auto start = chrono::high_resolution_clock::now();


        // Launch Threads
        // Thread is used to run function

        vector<thread> threads;
        int rows_per_thread = N/num_threads;

        for(int t=0; t<num_threads ; t++){
            int start_row = t*rows_per_thread;
            int end_row = (t == num_threads-1) ? N:(t+1)*rows_per_thread;

            threads.emplace_back(multiply_chunk , cref(A) , cref(B) , ref(C) , N , start_row , end_row);
        }

        for(auto &th : threads){
            th.join();
        }

        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double> elapsed = end-start;

        cout <<"Time for Parallel Matrix Multiplication: " << elapsed.count() <<" seconds " << endl;

        if(print_switch == 1){
            cout << "Resullt Matrix C = A * B " << endl;

            for(int i=0; i<N ; i++){
                for(int j=0 ; j<N ; j++){
                    cout << C[i*N + j] << endl;
                    cout << endl;
                }
            }
        }

        return 0;

    }