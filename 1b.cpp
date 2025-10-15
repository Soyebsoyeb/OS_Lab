#include<iostream>
#include<cstdlib>   // for getenv() , setenv()
#include<string>

using namespace std;

int main() {
	cout << "-------- Displaying Existing Environment Variables --------" << endl;

// List of standard environment variables to display
	const char* vars[] = {"USER" , "HOME" , "HOST" , "ARCH" , "DISPLAY","PRINTER","PATH"};

	for(auto& var:vars){
	const char* value = getenv(var);

	if(value) cout << var << " = " << value << endl;
	else cout << var << " = " << "is not sent" << endl;
	}

	cout << endl << "-------Setting New Environment Variables -------"<<endl;

	// Using setenv() to create neww variables
	
	if(setenv("COURSE" , "Operating_Systems" , 1) != 0){
		perror("setenv course failed");
	}

	if(setenv("LAB" , "System_Programming_Lab" , 1) != 0){
		perror("setenv LAB failed!");
	}

	// Display newly added variables
	
	cout << "COURSE = " << getenv("COURSE") << endl;
	cout << "LAB = " << getenv("LAB") << endl;


	cout << "New Environment variables successfully set!" << endl;

	return 0;
}

