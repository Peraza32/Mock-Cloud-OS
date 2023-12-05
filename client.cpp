#include <iostream>

#include <fcntl.h>

#include <unistd.h>

#include <sys/stat.h>

#include <mqueue.h>

#include <string>

#include <cstring>

#include <fstream>

#include <signal.h>

#include <sys/wait.h>

#include "Login.hpp"

#include "FileManager.hpp"

#include "Utility.hpp"

#include "Message.hpp"

using namespace std;

#define NOTIFY_SIG SIGUSR1

static void handler(int);

string loggedMenu(bool &flag, const string user);

string menu(bool &flag, string &user);

int main()

{

    bool isChild = false;

    //-------------------------------------------------------------------------------------

    // Variable initailization and Constants declaration

    //-------------------------------------------------------------------------------------

    // Variables for the program

    string message;

    mq_attr attr, resAttr; // Attributes of the queue

    bool flag = false;

    bool session = false;

    string user;

    Message msg = Message();
    Message response = Message();

    string queue_response;
    mqd_t mq_response;

    // Parameters for the queue

    const char *queue_name = "/MessageQueue";

    const int queue_permissions = 0666;

    // Opening the message queue

    mqd_t mq = mq_open(queue_name, O_RDWR | O_NONBLOCK, queue_permissions, NULL);

    // checking if the queue exists and was open

    if (mq == -1)

    {

        cout << "Message queue not open. Waking up server" << std::endl;

        // if queue was not open, then calling the server to create it

        int pid = fork();

        if (pid == 0)

        {

            if (execl("./server", "server", "", "", (char *)0) == -1)

            {

                perror("execl");

                exit(1); // Exit the child process on error
            }
        }

        else

        {

            cout << "\n-----------------------------------------";

            cout << "\nServer has been initialized\n";

            int status;

            waitpid(pid, &status, 0); // Waiting for the childs execution to terminate to continue

            mq = mq_open(queue_name, O_CREAT | O_RDWR | O_NONBLOCK, queue_permissions, NULL);
        }
    }

    mq_getattr(mq, &attr);

    char buffer[attr.mq_msgsize];

    string receivedMessage;

    //-------------------------------------------------------------------------------------

    /*sigemptyset(&blockMask); // Block timer signal temporarily

    sigaddset(&blockMask, NOTIFY_SIG); // Add timer signal to block mask

    if (sigprocmask(SIG_SETMASK, &blockMask, NULL) == -1) //

    {

        cout << "Error setting up signal handler" << endl;

        return -1;
    }

    sigemptyset(&sa.sa_mask); // Unblock timer signal

    sa.sa_flags = 0;

    sa.sa_handler = handler;

    if (sigaction(NOTIFY_SIG, &sa, NULL) == -1) // Establish handler for notification signal

    {

        cout << "Error setting up signal handler" << endl;

        return -1;
    }

    sev.sigev_notify = SIGEV_SIGNAL; // Notify via signal

    sev.sigev_signo = NOTIFY_SIG; // Notify using this signal

    if (mq_notify(mq, &sev) == -1) // Establish asynchronous notification

    {

        cout << "Error setting up signal handler" << endl;

        return -1;
    }*/

    // -------------------------------------------------------------------------------------

    // Reading messages from terminal, until exits with "exit"

    while (!flag)

    {

        // sigsuspend(&emptyMask); // Wait for notification signal

        /*if (mq_notify(mq, &sev) == -1) // Reregister for notification

        {

            cout << "Error reestablishing notification" << endl;

            return -1;
        }*/

        if (flag)

            break;

        if (session)

        {
            response.clear();
            receivedMessage.clear();
            message.clear();
            memset(buffer, 0, sizeof(buffer));
            mq_getattr(mq, &attr);

            buffer[attr.mq_msgsize];
            message = loggedMenu(flag, user);

            // cout << message << endl;
            msg.createFromString(message);
            mq_send(mq, msg.toString().c_str(), msg.toString().size(), 0);

            // mq_send(mq, message.c_str(), message.size(), 0);
            if (msg.getOption() == "7")
            {
                mq_close(mq_response);
                mq_unlink(queue_response.c_str());
                flag = true;
                break;
            }

            if (msg.getOption() == "0")
            {
                mq_send(mq, msg.toString().c_str(), msg.toString().size(), 0);
                mq_close(mq);
                mq_close(mq_response);
                mq_unlink(queue_response.c_str());
                flag = true;
                break;
            }

            // Response from server

            auto rom = mq_receive(mq_response, buffer, resAttr.mq_msgsize, nullptr); // Receiving the message

            receivedMessage = buffer; // Converting the message to string

            response.createFromString(receivedMessage);

            if (response.getOption() == "1")
            {
                if (response.getOneMessage(0) == "1")
                    cout << response.getOneMessage(1) << endl;
                else
                    cout << response.getOneMessage(1) << endl;
            }

            if (response.getOption() == "2")
            {
                if (response.getOneMessage(0) == "1")
                    cout << response.getOneMessage(1) << endl;
                else
                    cout << response.getOneMessage(1) << endl;
            }

            if (response.getOption() == "3")
            {
                if (response.getOneMessage(0) == "1")
                    cout << response.getOneMessage(1) << endl;
                else
                    cout << response.getOneMessage(1) << endl;
            }

            if (response.getOption() == "4")
            {
                if (response.getOneMessage(0) == "1")
                    cout << response.getOneMessage(1) << endl;
                else
                    cout << response.getOneMessage(1) << endl;
            }

            if (response.getOption() == "5")
            {
                cout << "Files in server: " << endl;
                response.showSeparatedMessages(',');
            }

            if (response.getOption() == "6")
            {
                int fd = open(response.getOneMessage(0).c_str(), O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
                if (fd != -1)
                {
                    string cmd = "/bin/nano " + response.getOneMessage(0);
                    system(cmd.c_str());
                    close(fd);
                }
                else
                {
                    cerr << "Error opening the file" << endl;
                }
            }
        }

        else

        {

            message = menu(flag, user);

            msg.createFromString(message);
            if (msg.getOption() == "3")
            {
                flag = true;
                break;
            }

            mq_send(mq, msg.toString().c_str(), msg.toString().length(), 0);

            // setting response queue
            queue_response = "/" + user + "ResponseQueue";
            mq_response = mq_open(queue_response.c_str(), O_CREAT | O_RDWR, 0666, NULL);
            mq_getattr(mq_response, &resAttr);
            buffer[resAttr.mq_msgsize];

            // Response from server
            auto rom = mq_receive(mq_response, buffer, resAttr.mq_msgsize, nullptr); // Receiving the message
            receivedMessage = buffer;                                                // Converting the message to string
            response.createFromString(receivedMessage);

            if (response.getOption() == "L")
            {
                if (response.getOneMessage(0) == "1")
                {
                    cout << response.getOneMessage(1) << endl;
                    session = true;
                }

                else
                    cout << response.getOneMessage(1) << endl;
            }

            if (response.getOption() == "R")

            {

                if (response.getOneMessage(0) == "1")
                {
                    cout << response.getOneMessage(1) << endl;
                    session = true;
                }

                else
                    cout << response.getOneMessage(1) << endl;
            }

            response.clear();
            receivedMessage.clear();
            message.clear();
            // clear buffer
            memset(buffer, 0, sizeof(buffer));
        }
    }

    // closing the queue

    mq_close(mq);

    mq_unlink(queue_name);

    return 0;
}

static void handler(int sig)

{

    // sig = 1;
}

string loggedMenu(bool &flag, const string user)

{

    string message;

    string originLocation;
    string destinationLocation;

    string filename;

    char option;

    cout << "Select an option: ";

    cout << "\n1. Import file to server";

    cout << "\n2. Export file from server";

    cout << "\n3. Create File";

    cout << "\n4. Delete File from server";

    cout << "\n5. List files in server";

    cout << "\n6.  Write to txt file";

    cout << "\n7. Exit" << endl;

    cin >> option;

    if (option == '0')
    {
        message = "|0|" + user + "|";
        flag = true;
        return message;
    }

    switch (option)

    {

    case '1':

        cout << endl;

        cout << "Enter the location of the file to import" << endl;

        cin >> originLocation;

        cout << "Enter the name of the file to import:" << endl;

        cin >> filename;

        cout << "Enter the name of directory where the file should be imported\nif you want to use the root directory, input: . (dot)" << endl;

        cin >> destinationLocation;

        destinationLocation = (destinationLocation == ".") ? "" : destinationLocation;

        // TODO: Check for file existence

        cout << "Importing file" << endl;

        message = "|1|" + user + "|" + originLocation + "|" + destinationLocation + "|" + filename + "|";

        // clearing variables

        originLocation.clear();

        destinationLocation.clear();

        filename.clear();

        break;

    case '2':

        cout << endl;

        cout << "Enter the location of the file to export\n if it's in the root directory, input: . (dot)" << endl;

        cin >> originLocation;

        cout << "Enter the name of the file to export:" << endl;

        cin >> filename;

        cout << "Enter the name of directory where the file should be exported" << endl;

        cin >> destinationLocation;

        originLocation = (originLocation == ".") ? "" : originLocation;
        // TODO: Check for file existence

        cout << "Importing file" << endl;

        message = "|2|" + user + "|" + originLocation + "|" + destinationLocation + "|" + filename + "|";

        // clearing variables

        originLocation.clear();

        destinationLocation.clear();

        filename.clear();

        break;

    case '3':

        cout << "Enter the name of the file to create:" << endl;

        cin >> filename;

        cout << "Creating file" << endl;

        message = "|3|" + user + "|" + filename + "|";

        // clearing variables

        filename.clear();

        break;

    case '4':

        cout << "Enter the name of the file to delete from the server: \nRemeber this action CAN NOT BE UNDONE" << endl;

        cin >> filename;

        cout << "Enter the name of directory where the file is located\nif you want to use the root directory, input: . (dot)" << endl;

        cin >> destinationLocation;

        destinationLocation = (destinationLocation == ".") ? "" : destinationLocation;

        cout << "Are you sure you want to delete the file? (Y/N)" << endl;

        cin >> option;

        if (option == 'Y' || option == 'y')

        {

            message = "|4|" + user + "|" + destinationLocation + "|" + filename + "|";
            cout << "Deleting file" << endl;
        }

        else

        {

            cout << "File not deleted" << endl;
        }

        // TODO: Check for file existence

        // clearing variables

        filename.clear();

        destinationLocation.clear();

        break;

    case '5':

        cout << "Listing files" << endl;

        message = "|5|" + user + "|";

        break;

    case '6':

        cout << "Enter the location of the file to edit \nif you want to use the root directory, input: . (dot)" << endl;
        cin >> originLocation;
        originLocation = (originLocation == ".") ? "" : originLocation;

        cout << "Enter the name of the .text file to edit" << endl;
        cin >> filename;

        filename = filename + ".txt";

        message = "|6|" + user + "|" + originLocation + "|" + filename + "|";

        // clearing variables

        originLocation.clear();

        filename.clear();

        break;

    case '7':

        cout << "Exiting" << endl;

        message = "|7|";

        flag = false;

        break;

    default:

        cout << "Invalid option" << endl;

        break;
    }

    return message;
}

string menu(bool &flag, string &user)

{

    char option;

    string message;

    string username, password;

    cout << "Select an option: ";

    cout << "\n1. Login";

    cout << "\n2. Register";

    cout << "\n3. Exit" << endl;

    cin >> option;

    switch (option)

    {

    case '1':

        cout << "Login" << endl;

        cout << "Enter your username: ";

        cin >> username;

        cout << "Enter your password: ";

        cin >> password;

        message = "|L|" + username + "|" + password + "|";

        user = username;

        break;

    case '2':

        cout << "Register" << endl;

        cout << "Enter your username: ";

        cin >> username;

        cout << "Enter your password: ";

        cin >> password;

        message = "|R|" + username + "|" + password + "|";

        user = username;

        break;

    case '3':

        cout << "Exiting" << endl;
        message = "|3|";
        flag = true;

        break;

    default:

        cout << "Invalid option" << endl;

        break;
    }

    return message;
}