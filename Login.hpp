#ifndef LOGIN_HPP
#define LOGIN_HPP
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class Login
{

private:
    string location;
    string filename;

    // Methods, mostrly helper ones for the public methods
    void createLoginFile(string location);
    bool checkLocationFile(string location);
    string formatInput(string username, string pass);
    bool validatePassword(string password);

public:
    // Constructor
    Login(string location);

    // Methods
    bool registerUser(string username, string password);
    bool login(string username, string password);
    // void updatePassword(string username, string password);
    // void updateUsername(string username, string newUsername);
};

Login::Login(string location)
{
    this->location = location;
    this->filename = "login.txt";
    if (!checkLocationFile(location))
        createLoginFile(location);
}

void Login::createLoginFile(string location)
{
    const string filename = "login.txt";
    const string path = location + filename;
    ofstream file;
    file.open(path, ios::out | ios::app);
    if (file.is_open())
        file << "|  username    |   password    |" << endl;
    file.close();
}

bool Login::checkLocationFile(string location)
{
    const string filename = "login.txt";
    const string path = location + filename;
    ifstream file;
    file.open(path);
    if (file.is_open())
    {
        file.close();
        return true;
    }
    else
        return false;
}

string Login::formatInput(string username, string pass)
{
    string formattedInput = "|" + username + "|" + pass + "|";
    return formattedInput;
}

bool Login::registerUser(string username, string password)
{
    fstream file;
    string line;
    string formattedInput = formatInput(username, password);
    file.open(location + filename, ios::in | ios::out | ios::app);
    if (file.is_open())
    {
        while (getline(file, line))
        {
            if (line.compare(formattedInput) == 0)
            {

                return false;
            }
        }
        file << formattedInput << endl;
        return true;
    }
    else
    {
        cout << "Error opening file" << endl;
        return false;
    }
}

bool Login::login(string username, string password)
{
    fstream file;
    string line;
    string formattedInput = formatInput(username, password);
    file.open(location + filename, ios::in | ios::out | ios::app);
    if (file.is_open())
    {
        while (getline(file, line))
        {
            if (line.compare(formattedInput) == 0)
            {
                cout << "Login successful" << endl;
                return true;
            }
        }
        cout << "Login failed" << endl;
        return false;
    }
    else
    {
        cout << "Error opening file" << endl;
        return false;
    }
}

#endif