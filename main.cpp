#include <iostream>
#include "FileManager.hpp"

using namespace std;

int main(void) {
	string clients[2] = {"client1", "client2"};
	string username = getenv("USER");
	bool deleteAll = true;

	FileManager managerDefault = FileManager(); // Creates Cloud in /home/<user>/Cloud
	//FileManager managerNoDefault = FileManager("/home/" + username + "/Desktop/Cloud");

	for(int i = 0; i < 2; ++i) {
		// Create clients' spaces inside the cloud
		managerDefault.createClientDirectory(clients[i]);
		//managerNoDefault.createClientDirectory(clients[i]);

		// Create "home" directory inside each client space
		managerDefault.createDirectory(clients[i], "home");
		//managerNoDefault.createDirectory(clients[i], "home");

		// Create file.txt inside the home directory of each client
		//managerDefault.createFile(clients[i], "home/file.txt");
		//managerNoDefault.createFile(clients[i], "home/file.txt");

		// Opens nano for each client and lets the user write in the specified file
		// If the file is not already created, this function creates it
		managerDefault.writeReadFile(clients[i], "home/file.txt");

		// Show files inside of each client space
		cout << "For deafult cloud, client " << i << ":" << endl;
		string str = "";
		managerDefault.showFiles(clients[i], &str);
		cout << str << endl;
		//cout << "For no deafult cloud, client " << i << ":" << endl;
		//managerNoDefault.showFiles(clients[i]);
	}

	for(int i = 0; i < 2; ++i) {
		cout << "The following is the recently created file from client " << i+1 << endl;
		sleep(2);
		managerDefault.writeReadFile(clients[i], "home/file.txt");
	}	

	/*string sourcePath;
	cout << "----------------------------------------------------------" << endl;
	cout << "Now it is time to test the import function" << endl;
	cout << "Write an absolute path to import a file from your computer to client1's home directory: " << endl;
	cin >> sourcePath;
	managerDefault.importFile("client1", sourcePath, "home");

	string destinationPath;
	cout << "----------------------------------------------------------" << endl;
	cout << "Now it is time to test the export function" << endl;
	cout << "Write a path where to export file.txt from client1's home directory to your computer: " << endl;
	cin >> destinationPath;
	managerDefault.exportFile("client1", "home/file.txt", destinationPath);*/
	
	cout << "Want to delete all files and directories? (1/0) "; 
	cin >> deleteAll;

	if(deleteAll) {
		managerDefault.deleteCloudSpace();
		//managerNoDefault.deleteCloudSpace();
	}

	return 0;
}