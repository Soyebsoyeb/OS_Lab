#include <iostream>
#include <vector>
#include <thread>
#include <random>
#include <chrono>
using namespace std;

vector<unsigned char> A, B, C;
int N, num_threads;

void multiply(int tid) {
    int rows_per_thread = N / num_threads;
    int start = tid * rows_per_thread;
    int end = (tid == num_threads - 1) ? N : start + rows_per_thread;

    for (int i = start; i < end; ++i) {
        for (int j = 0; j < N; ++j) {
            int sum = 0;
            for (int k = 0; k < N; ++k)
                sum += A[i*N + k] * B[k*N + j];
            C[i*N + j] = static_cast<unsigned char>(sum % 256);
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 5) return 1;
    N = stoi(argv[1]);
    num_threads = stoi(argv[2]);
    int mod = stoi(argv[3]);
    bool print_switch = stoi(argv[4]) == 1;

    A.resize(N*N);
    B.resize(N*N);
    C.resize(N*N, 0);

    // Initialize matrices
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(0, mod-1);
    for (int i=0;i<N*N;i++) { A[i]=dist(gen); B[i]=dist(gen); }

    auto start_time = chrono::high_resolution_clock::now();

    vector<thread> threads;
    for (int i = 0; i < num_threads; ++i)
        threads.emplace_back(multiply, i);

    for (auto &t : threads) t.join();

    auto end_time = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> elapsed = end_time - start_time;
    cout << "Time (ms): " << elapsed.count() << "\n";

    return 0;
}

