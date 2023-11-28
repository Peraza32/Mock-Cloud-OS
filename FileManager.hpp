#include <fcntl.h>
#include <sys/stat.h> 
#include <dirent.h>
#include <cstring>

using namespace std;

// Definición de códigos de escape ANSI para colores
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"

class FileManager {
private: 
	string spaceName; // personal space for the client, the directory name is intended to match the username
	int createPersonalSpace();

public:
	FileManager(string spaceName);

	int deletePersonalSpace();

	bool directoryExists(string dirname);
	int createDirectory(string dirname);
	int deleteDirectory(string dirname);

	int createFile(string filename);
	int openFile(string filename);
	int deleteFile(string filename);
	int retrieveFile(string filename);

	void showFiles();
	void showFiles(string dir);
};

FileManager::FileManager(string spaceName) {
	this->spaceName = spaceName; 
	createPersonalSpace();
}

int FileManager::createPersonalSpace() {
	return mkdir(this->spaceName.c_str(), 0700); // creates the directory in the current working directory
}

int FileManager::deletePersonalSpace() {
	return remove(this->spaceName.c_str());
}

bool FileManager::directoryExists(string dirname) {
	string path = spaceName + "/" + dirname;
	DIR* dir = opendir(path.c_str());

	if(dir)
		return true;
	else return false;
}

int FileManager::createDirectory(string dirname) {
	if(!directoryExists(dirname)) {
		string path = spaceName + "/" + dirname;
		return mkdir(path.c_str(), 0700);  // creates the directory in the personal space
	} 
	else {
		cerr << "El directorio especificado ya existe" << endl;
		return -1;
	}
}

int FileManager::deleteDirectory(string dirname) {
	if(directoryExists(dirname)) {
		string path = spaceName + "/" + dirname;
		return remove(path.c_str());
	} 
	else {
		cerr << "El directorio especificado no existe" << endl;
		return -1;	
	}
}

int FileManager::createFile (string filename) {
	string path = spaceName + "/"+ filename;
	mode_t mode = S_IRUSR | S_IWUSR; // permit read and writing only to the owner
	return open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, mode);
}

int FileManager::deleteFile(string filename) {
	string path = spaceName + "/"+ filename;
	return remove(path.c_str());
}

// Show only files in the specified directory
void FileManager::showFiles(string dirname) {
	// Open the specified directory
	DIR* dir = opendir(dirname.c_str());
	if(dir == NULL) {
		cout << "No se puede abrir el directorio especificado: " << dirname << endl;
		return;
	}

	// Read content of the directory
	dirent* entity = readdir(dir);
	while(entity != NULL) {

		if(entity->d_type == DT_DIR)
			cout << BLUE << entity->d_name << RESET << endl;
		else 
			cout << entity->d_name << endl;
		
		if(entity->d_type == DT_DIR && strcmp(entity->d_name, ".") != 0 && strcmp(entity->d_name, "..") != 0) {
			string path = dirname + "/" + entity->d_name;
			showFiles(path);
		}
		entity = readdir(dir);
	}

	closedir(dir);
}

// Show all files in the personal space
void FileManager::showFiles() {
	showFiles(this->spaceName);
}