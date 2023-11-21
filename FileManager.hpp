#include <fcntl.h>
#include <sys/stat.h> 

using namespace std;

class FileManager {
private: 
	string directoryName; // personal space for the client, the directory name is intended to match the username
public:
	FileManager(string directoryName);
	int createDirectory();
	int openFile(string filename);
	int deleteFile();
	int retrieveFile();
	void showFiles();
};

FileManager::FileManager(string directoryName) {
	this->directoryName = directoryName; 
}

int FileManager::createDirectory() {
	return mkdir(this->directoryName.c_str(), 0700); // creates the directory in the current working directory
}

int FileManager::openFile (string filename) {
	string path = directoryName + "/"+ filename;
	mode_t mode = S_IRUSR | S_IWUSR; // permit read and writing only to the owner
	return open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, mode);
}