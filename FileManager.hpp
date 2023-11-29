#include <fcntl.h>
#include <sys/stat.h> 
#include <dirent.h>
#include <cstring>
#include <unistd.h>

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

public:
	FileManager();
	FileManager(string baseDirectory);

	int deleteCloudSpace();

	bool directoryExists(string dirname);
	int createClientDirectory(string clientSpace);
	int deleteClientDirectory(string clientSpace);

	int createDirectory(string clientSpace, string dirname);
	int deleteDirectory(string clientSpace, string dirname);

	int createFile(string clientSpace, string filename);
	int deleteFile(string clientSpace, string filename);
	int retrieveFile(string clientSpace, string filename);

	void showFiles(string clientSpace);
	void showFiles(string clientSpace, string dir);

	void importFile(string filePath, string newPath);
	void exportFile();
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

int FileManager::deleteCloudSpace() {
	return remove(this->baseDirectory.c_str());
}

bool FileManager::directoryExists(string dirname) {
	string path = baseDirectory + "/" + dirname;
	DIR* dir = opendir(path.c_str());

	if(dir)
		return true;
	else return false;
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

int FileManager::deleteClientDirectory(string dirname) {
	if(directoryExists(dirname)) {
		string path = baseDirectory + "/" + dirname;
		return remove(path.c_str());
	} 
	else {
		cerr << "Directory does not exist" << endl;
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
			return remove(path.c_str());
		}
		else {
			cerr << "Directory does not exist" << endl;
			return -1;
		}
	} 
	else {
		cerr << "Client space was not found" << endl;
		return -1;
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

void FileManager::importFile(string filePath, string newPath) {
	if(directoryExists(newPath)) {
		string path = baseDirectory + "/" + newPath;
		
		if(rename(filePath.c_str(), newPath.c_str()) == 0)
			cout << "Archivo movido exitosamente " << endl;
		else perror("Error al mover el archivo");
	} 
	else perror("El directorio especificado no existe");	
}