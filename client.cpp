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



using namespace std;



#define NOTIFY_SIG SIGUSR1

static void handler(int);



string loggedMenu(bool &flag, const string user);

string menu(bool &logged, string &user);



int main()

{

    bool isChild = false;



    //-------------------------------------------------------------------------------------

    // Variable initailization and Constants declaration

    //-------------------------------------------------------------------------------------

    struct sigevent sev;           // Notification structure

    struct sigaction sa;           // Signal structure

    sigset_t blockMask, emptyMask; // Signal masks



    // Variables for the program

    string message;

    mq_attr attr; // Attributes of the queue

    bool flag = true;

    bool session = false;



    string user;



    // Parameters for the queue

    const char *queue_name = "/MessageQueue";

    const int queue_permissions = 0666;



    // Opening the message queue

    mqd_t mq = mq_open(queue_name, O_RDWR, queue_permissions, NULL);



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



            mq = mq_open(queue_name, O_WRONLY | O_NONBLOCK, queue_permissions, NULL);

        }

    }



    mq_getattr(mq, &attr);

    char buffer[attr.mq_msgsize];

    string receivedMessage;

    //-------------------------------------------------------------------------------------



    sigemptyset(&blockMask);                              // Block timer signal temporarily

    sigaddset(&blockMask, NOTIFY_SIG);                    // Add timer signal to block mask

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

    sev.sigev_signo = NOTIFY_SIG;    // Notify using this signal

    if (mq_notify(mq, &sev) == -1)   // Establish asynchronous notification

    {

        cout << "Error setting up signal handler" << endl;

        return -1;

    }



    // -------------------------------------------------------------------------------------



    char *response;

    // Reading messages from terminal, until exits with "exit"

    while (flag)

    {

        sigsuspend(&emptyMask);        // Wait for notification signal

        if (mq_notify(mq, &sev) == -1) // Reregister for notification

        {

            cout << "Error reestablishing notification" << endl;

            return -1;

        }

        if (flag)

            break;



        mq_getattr(mq, &attr);

        buffer[attr.mq_msgsize];



        if (session)

        {



            message = loggedMenu(flag, user);

            if (message == "6")

            {

                mq_send(mq, message.c_str(), message.size(), 0);

                break;

            }



            cout << message << endl;

            mq_send(mq, message.c_str(), message.size(), 0);

            // Response from server

            auto rom = mq_receive(mq, buffer, attr.mq_msgsize, nullptr); // Receiving the message

            receivedMessage = buffer;                                    // Converting the message to string

            cout << "Message received: " << buffer << endl;



            message.clear();

        }

        else

        {

            message = menu(flag, user);

            if (message.substr(0, 1) == "L")

            {

                mq_send(mq, message.c_str(), message.size(), 0);

                // Response from server

                auto rom = mq_receive(mq, buffer, attr.mq_msgsize, nullptr); // Receiving the message

                receivedMessage = buffer;                                    // Converting the message to string

                cout << "Message received: " << buffer << endl;

                if (receivedMessage.substr(0, 1) == "1")

                {

                    cout << "Login successful" << endl;

                    session = true;

                }

                else

                {

                    cout << "Login failed" << endl;

                    user = "";

                }

            }



            if (message.substr(0, 1) == "R")

            {

                mq_send(mq, message.c_str(), message.size(), 0);

                // Response from server

                auto rom = mq_receive(mq, buffer, attr.mq_msgsize, nullptr); // Receiving the message

                receivedMessage = buffer;                                    // Converting the message to string

                cout << "Message received: " << buffer << endl;

                if (receivedMessage.substr(0, 1) == "1")

                {

                    cout << "Registration successful" << endl;

                    session = true;

                }

                else

                {

                    cout << "Registration failed" << endl;

                    user = "";

                }

            }



            message.clear();

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

    string location;

    string filename;

    char option;

    cout << "Select an option: ";

    cout << "\n1. Import file to server";

    cout << "\n2. Export file from server";

    cout << "\n3. Create File";

    cout << "\n4. Delete File from server";

    cout << "\n5. List files in server";

    cout << "\n6. Exit" << endl;

    cin >> option;



    switch (option)

    {

    case '1':



        cout << endl;

        cout << "Enter the location of the file/s to import" << endl;

        cin >> location;

        cout << "Enter the name of the file to import:" << endl;

        cin >> filename;



        // TODO: Check for file existence



        cout << "Importing file" << endl;

        message = "1|" + user + " |" + location + " |" + filename;



        // clearing variables

        location.clear();

        filename.clear();



        break;

    case '2':

        cout << "Enter the name of the file to retrieve from the server:" << endl;

        cin >> filename;

        cout << "Enter the location where the file should be exported. \n Remeber, you must have access to this location" << endl;

        cin >> location;

        message = "2|" + user + " |" + location + " |" + filename;



        // TODO: Check for file existence



        cout << "Exporting file" << endl;



        // clearing variables

        location.clear();

        filename.clear();

        break;

    case '3':

        cout << "Enter the name of the file to create:" << endl;

        cin >> filename;



        cout << "Creating file" << endl;



        // clearing variables

        location.clear();

        filename.clear();

        message = "3 " + filename;

        break;

    case '4':

        cout << "Enter the name of the file to delete from the server: \n Remeber this action CAN NOT BE UNDONE" << endl;

        cin >> filename;

        cout << "Are you sure you want to delete the file? (Y/N)" << endl;

        cin >> option;

        if (option == 'Y' || option == 'y')

        {

            message = "4 " + filename;

        }

        else

        {

            cout << "File not deleted" << endl;

            break;

        }



        // TODO: Check for file existence



        cout << "Deleting file" << endl;



        // clearing variables

        filename.clear();

        break;

    case '5':

        cout << "Listing files" << endl;

        message = "5";

        break;

    case '6':

        cout << "Exiting" << endl;

        message = "6";

        flag = false;

        break;

    default:

        cout << "Invalid option" << endl;

        break;

    }

    return message;

}



string menu(bool &logged, string &user)

{



    char option;

    string message;

    string username, password;

    Login login = Login("./");

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



        message = "L " + username + "|" + password;

        user = username;

        break;

    case '2':

        cout << "Register" << endl;

        cout << "Enter your username: ";

        cin >> username;

        cout << "Enter your password: ";

        cin >> password;

        message = "R " + username + "|" + password;

        user = username;

        break;

    case '3':

        cout << "Exiting" << endl;



        break;

    default:

        cout << "Invalid option" << endl;

        break;

    }

    return message;

}