#include <iostream>
#include "FileManager.hpp"

int main (void) {
	string filename = "file";
	string username = "victor";

	FileManager manager = FileManager(username);

	if(manager.createDirectory() != -1) 
		cout << "\nDirectorio creado!" << endl;
	else cout << "\nDirectorio no creado" << endl;

	if(manager.openFile(filename) != -1) 
		cout << "\nArchivo creado!" << endl;
	else cout << "\nArchivo no creado" << endl;

	return 0; 
}