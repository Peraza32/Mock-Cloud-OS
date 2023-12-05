#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <iostream>
#include <string>
#include <vector>
#include "Utility.hpp"

using namespace std;

// example of message:
//  |option|sender|message1|message2|message3|...|messageN|

class Message
{

private:
    string sender;
    vector<std::string> messages;
    string separator;
    string option;

public:
    Message();
    Message(string sender);
    Message(string sender, vector<std::string> messages, string separator);

    void setSender(string sender);
    void addMessage(string message);
    void setSeparator(string separator);
    void setOption(string option);
    void setMessages(vector<std::string> messages);
    // set Messages overload, receives a pointer to string, a separator
    void setMessages(string messages, string separator);

    string getSender();
    vector<std::string> getMessages();
    string toString();
    void createFromString(string message);
    void showSeparatedMessages(char separator);
    string getOption();
    string getOneMessage(int index);

    void clear();
};

Message::Message()
{
    this->sender = "";
    this->messages = vector<std::string>();
    this->separator = "|";
}

Message::Message(string sender)
{
    this->sender = sender;
    this->messages = vector<std::string>();
    this->separator = "|";
}

Message::Message(string sender, vector<std::string> messages, string separator)
{
    this->sender = sender;
    this->messages = messages;
    this->separator = separator;
}

void Message::setSender(string sender)
{
    this->sender = sender;
}

void Message::addMessage(string message)
{
    this->messages.push_back(message);
}

void Message::setSeparator(string separator)
{
    this->separator = separator;
}

void Message::setOption(string option)
{
    this->option = option;
}

void Message::setMessages(vector<std::string> messages)
{
    this->messages = messages;
}

void Message::setMessages(string messages, string separator)
{
    this->messages.clear();
    string temp = "";
    for (int i = 0; i < sizeof(messages); i++)
    {
        if (messages[i] == separator[0])
        {
            this->messages.push_back(temp);
            temp = "";
        }
        else
            temp += messages[i];
    }
}

string Message::getSender()
{
    return this->sender;
}

vector<std::string> Message::getMessages()
{
    return this->messages;
}

string Message::toString()
{
    string result =
        this->separator + this->option +
        this->separator + this->sender + this->separator;
    for (int i = 0; i < this->messages.size(); i++)
    {
        result += this->messages[i] + this->separator;
    }
    return result;
}

// input string for createFromString should be in the format of:
// |opc|sender|message1|message2|message3|...|messageN|
void Message::createFromString(string message)
{
    this->messages.clear();
    int start = 0;
    int end = 0;
    int count = 0;
    for (int i = 0; i < message.length(); i++)
    {
        if (message[i] == this->separator[0])
        {
            if (count == 0)
            {
                start = i + 1;
                count++;
            }
            else if (count == 1)
            {
                end = i;
                this->option = message.substr(start, end - start);
                start = i + 1;
                count++;
            }
            else if (count == 2)
            {
                end = i;
                this->sender = message.substr(start, end - start);
                start = i + 1;
                count++;
            }
            else
            {
                end = i;
                this->messages.push_back(message.substr(start, end - start));
                start = i + 1;
            }
        }
    }
}

void Message::showSeparatedMessages(char separator = '|')
{
    if (this->option == "5")
    {
        unformatInput(this->messages[0], this->messages, separator);
        this->messages.erase(this->messages.begin());
    }
    for (int i = 0; i < this->messages.size(); i++)
        cout << this->messages[i] << endl;
}

string Message::getOption()
{
    return this->option;
}

string Message::getOneMessage(int index)
{
    return this->messages[index];
}

void Message::clear()
{
    this->messages.clear();
    this->separator = "|";
}

#endif