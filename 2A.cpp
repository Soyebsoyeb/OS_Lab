#include<iostream>
#include<csignal>         // for signal() , SIGINT
#include<cstdlib>         // for exit() , EXIT_SUCCESS , EXIT_FAILURE
#include<unistd.h>        // for getpid() , sleep()

using namespace std;



// Global counter for how many times Ctrl+C was pressed
int sigint_count = 0;


// Signal Handler Function
void sigint_handler(int sig){
	sigint_count++;
	
	cout << endl <<"[CTRL + C] Ha Ha , Not Stopping (count = " << sigint_count << ")" << endl;


// Safety Exit after 5 times
if(sigint_count >= 5){
	cout << " Alright , you win Exiting now ... " << endl;
	exit(0);
}

if(signal(SIGINT,sigint_handler) == SIG_ERR){
	perror("Signal");
	exit(1);
}
}	

int main(){
	// Register SIGINT handler
	if(signal(SIGINT , sigint_handler) == SIG_ERR){
		perror("Signal");
		exit(EXIT_FAILURE);
	}
	cout << "Program Started. Try pressing Ctrl+C multiple times ... "<< endl;

	// Infinite Loop to keep the program Alive
	
	while(true){
	cout << "Running ....(PID: " << getpid() << ")" << endl;
	sleep(1);
	}

	return 0;
}

