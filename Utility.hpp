#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <iostream>
#include <string>
#include <vector>

using namespace std;

// This function formats the input received from the client to use in the server operations
// string comes in the following format "operation|client|path" or "path1,path2,path3"
// Where operation can be a number from 1 to 6 or L for login or R for register
// Client is the name of the client
// Path is the path to the file or directory
void unformatInput(string input, vector<string> &output, char delimiter = '|')
{
    string temp = "";
    for (int i = 0; i < input.length(); ++i)
    {
        if (input[i] == delimiter)
        {
            output.push_back(temp);
            temp = "";
        }
        else
            temp += input[i];
    }
}

#endif