#include <fcntl.h>
#include <sys/stat.h> 
#include <dirent.h>
#include <cstring>
#include <unistd.h>
#include <regex>

using namespace std;

// Definición de códigos de escape ANSI para colores
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"

class FileManager {
private: 
	string baseDirectory;
	int createCloudSpace();
	void deleteDirectory(string dirname);

public:
	FileManager();
	FileManager(string baseDirectory);

	void deleteCloudSpace();

	bool directoryExists(string dirname); // Checks if a directory exists in the Cloud space
	int createClientDirectory(string clientSpace);
	int deleteClientDirectory(string clientSpace);

	int createDirectory(string clientSpace, string dirname); // Creates a directory inside a client space
	int deleteDirectory(string clientSpace, string dirname); // Deletes a directory inside a client space

	int createFile(string clientSpace, string filename); // Creates a file inside a client space
	int deleteFile(string clientSpace, string filename); // Deletes a file inside a client space

	void showFiles(string clientSpace); // Shows all files in the Cloud space of a specific client
	void showFiles(string clientSpace, string dir); // Shows only files in the specified directory

	void importFile(string clientSpace, string sourcePath, string destinationPath);
	void exportFile(string clientSpace, string sourcePath, string destinationPath);

	void writeReadFile(string clientSpace, string filePath);
};

FileManager::FileManager() {
	string username = getenv("USER");
	this->baseDirectory = "/home/" + username + "/Cloud";
	createCloudSpace();
}

FileManager::FileManager(string baseDirectory) {
	this->baseDirectory = baseDirectory; // This must be an absolute path, otherwise creates the directory in the current working directory
	createCloudSpace();
}

int FileManager::createCloudSpace() {
	return mkdir(this->baseDirectory.c_str(), 0700);
}

void FileManager::deleteCloudSpace() {
	// Open the specified directory
	DIR* dir = opendir(this->baseDirectory.c_str());

	if(dir == NULL) {
		cout << "Cloud can not be open: " << endl;
		return;
	}

	// Read content of the directory
	dirent* entity = readdir(dir);
	while(entity != NULL) {
		
		// Delete all clients
		if(entity->d_type == DT_DIR && strcmp(entity->d_name, ".") != 0 && strcmp(entity->d_name, "..") != 0) {
			deleteClientDirectory(entity->d_name); 
		}

		entity = readdir(dir);
	}

	closedir(dir);
	remove(this->baseDirectory.c_str());
}

bool FileManager::directoryExists(string dirname) {
	string escapedBaseDirectory = regex_replace(this->baseDirectory, regex("/"), "\\/");
	
	if(!regex_match(dirname, regex("^" + escapedBaseDirectory + ".*"))) {
		string path = baseDirectory + "/" + dirname;
		DIR* dir = opendir(path.c_str());

		if(dir) {
			closedir(dir);
			return true;
		}
		else {
			closedir(dir);
			return false;
		}
	}
	else {
		DIR* dir = opendir(dirname.c_str());

		if(dir) {
			closedir(dir);
			return true;
		}
		else {
			closedir(dir);
			return false;
		}
	}
			
}

int FileManager::createClientDirectory(string clientSpace) {
	if(!directoryExists(clientSpace)) {
		string path = baseDirectory + "/" + clientSpace;
		return mkdir(path.c_str(), 0700);  // creates the directory in the Cloud space
	} 
	else {
		cerr << "Client space was already created" << endl;
		return -1;
	}
}

int FileManager::deleteClientDirectory(string clientSpace) {
	if(directoryExists(clientSpace)) {
		deleteDirectory(this->baseDirectory + "/" + clientSpace);
		return 0;
	} 
	else {
		cerr << "Directory does not exist: " << clientSpace << endl;
		return -1;	
	}
}

int FileManager::createDirectory(string clientSpace, string dirname) {
	if(directoryExists(clientSpace)) {
		if(!directoryExists(clientSpace + "/" + dirname)) {
			string path = baseDirectory + "/" + clientSpace + "/" + dirname;
			return mkdir(path.c_str(), 0700);  // creates the directory in the Cloud space
		}
		else {
			cerr << "Directory already exists" << endl;
			return -1;
		}
	} 
	else {
		cerr << "Client space was not found" << endl;
		return -1;
	}
}

int FileManager::deleteDirectory(string clientSpace, string dirname) {
	if(directoryExists(clientSpace)) {
		if(directoryExists(clientSpace + "/" + dirname)) {
			string path = baseDirectory + "/" + clientSpace + "/" + dirname;
			deleteDirectory(path);
			return 0;
		}
		else {
			cerr << "Directory does not exist: " << clientSpace + "/" + dirname << endl;
			return -1;
		}
	} 
	else {
		cerr << "Client space was not found" << endl;
		return -1;
	}
}

void  FileManager::deleteDirectory(string dirname) {
	if(directoryExists(dirname)) {

		// Open the specified directory
		DIR* dir = opendir(dirname.c_str());
		if(dir == NULL) {
			cout << "Specified directory can not be open: " << dirname << endl;
			return;
		}

		// Read content of the directory
		dirent* entity = readdir(dir);
		while(entity != NULL) {
			
			// Delete recursively all contents inside a directory except "." and ".."
			if(entity->d_type == DT_DIR && strcmp(entity->d_name, ".") != 0 && strcmp(entity->d_name, "..") != 0) {
				string entityName = entity->d_name;
				string directory = dirname + "/" + entityName;
				deleteDirectory(directory);
			}

			if(entity->d_type != DT_DIR && strcmp(entity->d_name, ".") != 0 && strcmp(entity->d_name, "..") != 0) {
				if(remove((dirname + "/" + entity->d_name).c_str()) != -1)
					cout << "File deleted successfully: " << dirname + "/" + entity->d_name << endl;
				else 
					cerr << "Error deleting file: "<< dirname + "/" + entity->d_name << endl;
			}

			entity = readdir(dir);
		}

		closedir(dir);
		if(remove(dirname.c_str()) != -1)
			cout << "Directory deleted successfully: " << dirname << endl;
		else 
			cerr << "Error deleting directory: "<< dirname << endl;
	} 
	else {
		cerr << "Directory does not exist: " <<  dirname << endl;	
	}
}

int FileManager::createFile(string clientSpace, string filename) {
	if(directoryExists(clientSpace)) {
		string path = baseDirectory + "/" + clientSpace + "/" + filename;
		mode_t mode = S_IRUSR | S_IWUSR; // permit read and writing only to the owner
		return open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, mode);
	} 
	else {
		cerr << "Client space was not found" << endl;
		return -1;
	}
}

int FileManager::deleteFile(string clientSpace, string filename) {
	if(directoryExists(clientSpace)) {
		string path = baseDirectory + "/" + clientSpace + "/" + filename;
		return remove(path.c_str());
	} 
	else {
		cerr << "Client space was not found" << endl;
		return -1;
	}
}

// Show only files in the specified directory
// dirname must be an absolute route
void FileManager::showFiles(string clientSpace, string dirname) {
	if(directoryExists(clientSpace)) {

		// Open the specified directory
		DIR* dir = opendir(dirname.c_str());
		if(dir == NULL) {
			cout << "Specified directory can not be open: " << dirname << endl;
			return;
		}

		// Read content of the directory
		dirent* entity = readdir(dir);

		while(entity != NULL) {

			if(entity->d_type == DT_DIR) // print in blue all directories
				cout << BLUE << entity->d_name << RESET << endl;
			else 
				cout << entity->d_name << endl;
			
			// Print recursively all contents inside a directory except "." and ".."
			if(entity->d_type == DT_DIR && strcmp(entity->d_name, ".") != 0 && strcmp(entity->d_name, "..") != 0) {
				string path = dirname + "/" + entity->d_name;
				showFiles(clientSpace, path);
			}

			entity = readdir(dir);
		}

		closedir(dir);
	} 
	else {
		cerr << "Client space was not found" << endl;
	}
}

// Show all files in the Cloud space of a specific client
void FileManager::showFiles(string clientSpace) {
	if(directoryExists(clientSpace)) {
		showFiles(clientSpace ,this->baseDirectory + "/" + clientSpace);	
	} 
	else {
		cerr << "Client space was not found" << endl;
	}
}

void FileManager::importFile(string clientSpace, string sourcePath, string destinationPath) {
	if(directoryExists(clientSpace)) {
		string destination = baseDirectory + "/" + clientSpace + "/" + destinationPath;
		string cmd = "mv " + sourcePath + " " + destination;
		system(cmd.c_str());
	} 
	else perror("Client space was not found");	
}

void FileManager::exportFile(string clientSpace, string sourcePath, string destinationPath) {
	if(directoryExists(clientSpace)) {
		string source = baseDirectory + "/" + clientSpace + "/" + sourcePath;
		string cmd = "mv " + source + " " + destinationPath;
		system(cmd.c_str());
	} 
	else perror("Client space was not found");	
}

void FileManager::writeReadFile(string clientSpace, string filePath) {
	if(directoryExists(clientSpace)) {
		string path = baseDirectory + "/" + clientSpace + "/" + filePath;
		int fd = open(path.c_str(), O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
		if(fd != -1) {
			string cmd = "/bin/nano " + path;
			system(cmd.c_str());
			close(fd);
		}
		else {
			cerr << "Error opening the file" << endl;
		}
	} 
	else {
		cerr << "Client space was not found" << endl;
	}
}