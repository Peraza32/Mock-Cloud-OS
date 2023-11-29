#include <iostream>
#include "FileManager.hpp"

using namespace std;

int main(void) {
	string clients[2] = {"client1", "client2"};
	string username = getenv("USER");
	bool deleteAll = true;

	FileManager managerDefault = FileManager(); // Creates Cloud in /home/<user>/Cloud
	FileManager managerNoDefault = FileManager("/home/" + username + "/Desktop/Cloud");

	for(int i = 0; i < 2; ++i) {
		// Create clients' spaces inside the cloud
		managerDefault.createClientDirectory(clients[i]);
		managerNoDefault.createClientDirectory(clients[i]);

		// Create "home" directory inside each client space
		managerDefault.createDirectory(clients[i], "home");
		managerNoDefault.createDirectory(clients[i], "home");

		// Create file.txt inside the home directory of each client
		managerDefault.createFile(clients[i], "home/file.txt");
		managerNoDefault.createFile(clients[i], "home/file.txt");

		// Show files inside of each client space
		cout << "For deafult cloud, client " << i << ":" << endl;
		managerDefault.showFiles(clients[i]);
		cout << "For no deafult cloud, client " << i << ":" << endl;
		managerNoDefault.showFiles(clients[i]);
	}

	cout << "Want to delete all files and directories? (1/0) "; 
	cin >> deleteAll;

	if(deleteAll) {
		managerDefault.deleteCloudSpace();
		managerNoDefault.deleteCloudSpace();
	}

	return 0;
}