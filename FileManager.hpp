#ifndef FILEMANAGER_HPP

#define FILEMANAGER_HPP

#include <fcntl.h>

#include <sys/stat.h>

#include <dirent.h>

#include <cstring>

#include <unistd.h>

#include <regex>

using namespace std;

// Definición de códigos de escape ANSI para colores

#define RESET "\033[0m"

#define RED "\033[31m"

#define GREEN "\033[32m"

#define YELLOW "\033[33m"

#define BLUE "\033[34m"

class FileManager

{

private:
	string baseDirectory;

	int createCloudSpace();

	void deleteDirectory(string dirname);

public:
	FileManager();

	FileManager(string baseDirectory);

	void deleteCloudSpace();

	bool directoryExistsInCloud(string dirname); // Checks if a directory exists in the Cloud space

	bool fileExistsInCloud(string filename); // Checks if a file exists in the Cloud space

	bool directoryExists(string dirname); // Checks if a directory exists in the specified path

	bool fileExists(string filename); // Checks if a file exists in the specified path

	int createClientDirectory(string clientSpace);

	int deleteClientDirectory(string clientSpace);

	int createDirectory(string clientSpace, string dirname); // Creates a directory inside a client space

	int deleteDirectory(string clientSpace, string dirname); // Deletes a directory inside a client space

	int createFile(string clientSpace, string filename); // Creates a file inside a client space

	int deleteFile(string clientSpace, string insidePath, string filename); // Deletes a file inside a client space

	void showFiles(string clientSpace, string *result); // Shows all files in the Cloud space of a specific client

	void showFiles(string clientSpace, string dir, string *result); // Shows only files in the specified directory

	void importFile(string clientSpace, string sourcePath, string destinationPath, const string filename);

	void exportFile(string clientSpace, string sourcePath, string destinationPath, const string file);

	// Deletes a file inside a client space

	void writeReadFile(string clientSpace, string filePath); // Opens nano for each client and lets the user write in the specified file
};

FileManager::FileManager()

{

	string username = getenv("USER");

	this->baseDirectory = "/home/" + username + "/Cloud";

	createCloudSpace();
}

FileManager::FileManager(string baseDirectory)

{

	string username = getenv("USER");

	this->baseDirectory = "/home/" + username + "/Cloud/" + baseDirectory;
}

int FileManager::createCloudSpace()

{

	return mkdir(this->baseDirectory.c_str(), 0700);
}

void FileManager::deleteCloudSpace()

{

	// Open the specified directory

	DIR *dir = opendir(this->baseDirectory.c_str());

	if (dir == NULL)

	{

		cout << "Cloud can not be open: " << endl;

		return;
	}

	// Read content of the directory

	dirent *entity = readdir(dir);

	while (entity != NULL)

	{

		// Delete all clients

		if (entity->d_type == DT_DIR && strcmp(entity->d_name, ".") != 0 && strcmp(entity->d_name, "..") != 0)

		{

			deleteClientDirectory(entity->d_name);
		}

		entity = readdir(dir);
	}

	closedir(dir);

	remove(this->baseDirectory.c_str());
}

bool FileManager::directoryExistsInCloud(string dirname)

{

	string escapedBaseDirectory = regex_replace(this->baseDirectory, regex("/"), "\\/");

	if (!regex_match(dirname, regex("^" + escapedBaseDirectory + ".*")))

	{

		string path = baseDirectory + "/" + dirname;

		DIR *dir = opendir(path.c_str());

		if (dir)

		{

			closedir(dir);

			return true;
		}

		else

		{

			closedir(dir);

			return false;
		}
	}

	else

	{

		DIR *dir = opendir(dirname.c_str());

		if (dir)

		{

			closedir(dir);

			return true;
		}

		else

		{

			closedir(dir);

			return false;
		}
	}
}

bool FileManager::fileExistsInCloud(string filename)

{

	string escapedBaseDirectory = regex_replace(this->baseDirectory, regex("/"), "\\/");

	if (!regex_match(filename, regex("^" + escapedBaseDirectory + ".*")))

	{

		string path = baseDirectory + "/" + filename;

		int fd = open(path.c_str(), O_RDONLY);

		if (fd != -1)

		{

			close(fd);

			return true;
		}

		else

		{

			close(fd);

			return false;
		}
	}

	else

	{

		int fd = open(filename.c_str(), O_RDONLY);

		if (fd != -1)

		{

			close(fd);

			return true;
		}

		else

		{

			close(fd);

			return false;
		}
	}
}

bool FileManager::directoryExists(string dirname)

{

	DIR *dir = opendir(dirname.c_str());

	if (dir)

	{

		closedir(dir);

		return true;
	}

	else

	{

		closedir(dir);

		return false;
	}
}

bool FileManager::fileExists(string filename)

{

	int fd = open(filename.c_str(), O_RDONLY);

	if (fd != -1)

	{

		close(fd);

		return true;
	}

	else

	{

		close(fd);

		return false;
	}
}

int FileManager::createClientDirectory(string clientSpace)

{

	if (!directoryExistsInCloud(clientSpace))

	{

		string path = baseDirectory + "/" + clientSpace;

		return mkdir(path.c_str(), 0700); // creates the directory in the Cloud space
	}

	else

	{

		cerr << "Client space was already created" << endl;

		return -1;
	}
}

int FileManager::deleteClientDirectory(string clientSpace)

{

	if (directoryExistsInCloud(clientSpace))

	{

		deleteDirectory(this->baseDirectory + "/" + clientSpace);

		return 0;
	}

	else

	{

		cerr << "Directory does not exist: " << clientSpace << endl;

		return -1;
	}
}

int FileManager::createDirectory(string clientSpace, string dirname)

{

	if (directoryExistsInCloud(clientSpace))

	{

		if (!directoryExistsInCloud(clientSpace + "/" + dirname))

		{

			string path = baseDirectory + "/" + clientSpace + "/" + dirname;

			return mkdir(path.c_str(), 0700); // creates the directory in the Cloud space
		}

		else

		{

			cerr << "Directory already exists" << endl;

			return -1;
		}
	}

	else

	{

		cerr << "Client space was not found" << endl;

		return -1;
	}
}

int FileManager::deleteDirectory(string clientSpace, string dirname)

{

	if (directoryExistsInCloud(clientSpace))

	{

		if (directoryExistsInCloud(clientSpace + "/" + dirname))

		{

			string path = baseDirectory + "/" + clientSpace + "/" + dirname;

			deleteDirectory(path);

			return 0;
		}

		else

		{

			cerr << "Directory does not exist: " << clientSpace + "/" + dirname << endl;

			return -1;
		}
	}

	else

	{

		cerr << "Client space was not found" << endl;

		return -1;
	}
}

void FileManager::deleteDirectory(string dirname)

{

	if (directoryExistsInCloud(dirname))

	{

		// Open the specified directory

		DIR *dir = opendir(dirname.c_str());

		if (dir == NULL)

		{

			cout << "Specified directory can not be open: " << dirname << endl;

			return;
		}

		// Read content of the directory

		dirent *entity = readdir(dir);

		while (entity != NULL)

		{

			// Delete recursively all contents inside a directory except "." and ".."

			if (entity->d_type == DT_DIR && strcmp(entity->d_name, ".") != 0 && strcmp(entity->d_name, "..") != 0)

			{

				string entityName = entity->d_name;

				string directory = dirname + "/" + entityName;

				deleteDirectory(directory);
			}

			if (entity->d_type != DT_DIR && strcmp(entity->d_name, ".") != 0 && strcmp(entity->d_name, "..") != 0)

			{

				if (remove((dirname + "/" + entity->d_name).c_str()) != -1)

					cout << "File deleted successfully: " << dirname + "/" + entity->d_name << endl;

				else

					cerr << "Error deleting file: " << dirname + "/" + entity->d_name << endl;
			}

			entity = readdir(dir);
		}

		closedir(dir);

		if (remove(dirname.c_str()) != -1)

			cout << "Directory deleted successfully: " << dirname << endl;

		else

			cerr << "Error deleting directory: " << dirname << endl;
	}

	else

	{

		cerr << "Directory does not exist: " << dirname << endl;
	}
}

int FileManager::createFile(string clientSpace, string filename)

{

	if (directoryExistsInCloud(clientSpace))

	{

		string path = baseDirectory + "/" + clientSpace + "/" + filename;

		mode_t mode = S_IRUSR | S_IWUSR; // permit read and writing only to the owner

		return open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, mode);
	}

	else

	{

		cerr << "Client space was not found" << endl;

		return -1;
	}
}

int FileManager::deleteFile(string clientSpace, string insidePath, string filename)

{

	if (directoryExistsInCloud(clientSpace))

	{

		string path = baseDirectory + "/" + clientSpace + "/" + insidePath + filename;

		return remove(path.c_str());
	}

	else

	{

		cerr << "Client space was not found" << endl;

		return -1;
	}
}

// Show only files in the specified directory

// dirname must be an absolute route

void FileManager::showFiles(string clientSpace, string dirname, string *result)

{

	if (directoryExistsInCloud(clientSpace))

	{

		// Open the specified directory

		DIR *dir = opendir(dirname.c_str());

		if (dir == NULL)

			cout << "Err: Specified directory can not be open: " + dirname + "\n";

		// Read content of the directory

		dirent *entity = readdir(dir);

		while (entity != NULL)

		{

			string path = dirname + "/" + entity->d_name;

			if (strcmp(entity->d_name, ".") != 0 && strcmp(entity->d_name, "..") != 0)

				*result = *result + path + ",";

			// Print recursively all contents inside a directory except "." and ".."

			if (entity->d_type == DT_DIR && strcmp(entity->d_name, ".") != 0 && strcmp(entity->d_name, "..") != 0)

			{

				showFiles(clientSpace, path, result);
			}

			entity = readdir(dir);
		}

		closedir(dir);
	}

	else

	{

		cout << "Err: Client space was not found\n";
	}
}

// Show all files in the Cloud space of a specific client

void FileManager::showFiles(string clientSpace, string *result)

{

	if (directoryExistsInCloud(clientSpace))

	{

		showFiles(clientSpace, this->baseDirectory + "/" + clientSpace, result);
	}

	else

		cout << "Err: Client space was not found\n";
}

void FileManager::importFile(string clientSpace, string sourcePath, string destinationPath, const string filename)

{

	if (directoryExistsInCloud(clientSpace))

	{

		if (fileExistsInCloud(clientSpace + "/" + destinationPath + filename))

		{

			cerr << "File already exists" << endl;

			return;
		}

		string destination = baseDirectory + "/" + clientSpace + "/" + destinationPath;

		string cmd = "mv " + sourcePath + filename + " " + destination;

		system(cmd.c_str());

		// for moving the whole dir, just specify source path, but not a filename
	}

	else

		perror("Client space was not found");
}

void FileManager::exportFile(string clientSpace, string sourcePath, string destinationPath, const string file)

{

	if (directoryExistsInCloud(clientSpace))

	{

		if (fileExistsInCloud(clientSpace + "/" + sourcePath + file))

		{

			smatch filename;
			string path = sourcePath + file;
			if (regex_search(path, filename, regex("[^/]+$")))

			{

				string destination = destinationPath + "/" + filename.str();

				if (fileExists(destination))

				{

					cerr << "File already exists" << endl;

					return;

				}

				string source = baseDirectory + "/" + clientSpace + "/" + sourcePath + file;

				string cmd = "mv " + source + " " + destinationPath;

				system(cmd.c_str());

			}

		}
	}

	else

		perror("Client space was not found");
}

void FileManager::writeReadFile(string clientSpace, string filePath)

{

	if (directoryExistsInCloud(clientSpace))

	{

		string path = baseDirectory + "/" + clientSpace + "/" + filePath;

		int fd = open(path.c_str(), O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);

		if (fd != -1)

		{

			string cmd = "/bin/nano " + path;

			system(cmd.c_str());

			close(fd);
		}

		else

		{

			cerr << "Error opening the file" << endl;
		}
	}

	else

	{

		cerr << "Client space was not found" << endl;
	}
}

#endif