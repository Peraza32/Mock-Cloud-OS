#include <iostream>
#include "FileManager.hpp"

int main (void) {
	string filename = "file";
	string username = "victor";
	string directory = "home";
	string filename2 = "home/file";

	FileManager manager = FileManager(username);

	int t = 3;

	manager.createDirectory(directory);

	while(t--) {
		manager.createFile(filename);

		manager.createFile(filename2);
	}

	manager.showFiles();

	// Test for when entering just the name directory
	// The correct way should be <spaceName>/<dirname>
	manager.showFiles("home");

	// Correct way
	manager.showFiles("victor/home");

	// Deleting 
	if(manager.deleteFile(filename) != -1)
		cout << "\nArchivo eliminado!" << endl;
	else cout << "\nArchivo no eliminado" << endl;

	if(manager.deleteDirectory(directory) != -1)
		cout << "\nDirectorio eliminado!" << endl;
	else cout << "\nDirectorio no eliminado" << endl;

	return 0; 
}