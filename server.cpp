#include <iostream>
#include <cstring>
#include <mqueue.h>   // message queue header
#include <sys/stat.h> // mode constants
#include <fcntl.h>    // O_* constants
#include <unistd.h>   // fork
#include <signal.h>   // signal
#include "FileManager.hpp"

using namespace std;

#define NOTIFY_SIG SIGUSR1 // Notification signal from timer
static void handler(int);  // Signal handler

mqd_t mq;
const char *queue_name = "/MessageQueue";

int main()
{
    //-------------------------------------------------------------------------------------
    // Interruption signal
    signal(SIGINT, handler); // For handling the ^C signal, this way we close the message queue
    //-------------------------------------------------------------------------------------
    // Variable initialization and Constants declaration
    //-------------------------------------------------------------------------------------
    // Signal structures
    struct sigevent sev;
    struct sigaction sa;
    sigset_t blockMask, emptyMask;
    FileManager fileManager =  FileManager();

    // Queue variables and parameters
    string message, receivedMessage;
    mq_attr attr;
    ssize_t numRead; // Number of bytes read

    const int permission = 0666;

    bool flag = false;

    //-------------------------------------------------------------------------------------
    // Set up
    //-------------------------------------------------------------------------------------
    // creating message queue
    mq = mq_open(queue_name, O_CREAT | O_RDWR | O_NONBLOCK, permission, NULL);

    // Validating message queue creation, otherwise inform user and exit
    if (mq == -1)
    {
        cout << "Error creating message queue" << endl;
        return -1;
    }

    // Message queue attributes
    mq_getattr(mq, &attr);
    char buffer[attr.mq_msgsize]; // buffer size is the same as the message queue size

    //-------------------------------------------------------------------------------------
    // Setting up signal handler
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

    cout << "Message queue created successfully" << endl;

    //-------------------------------------------------------------------------------------

    while (flag != true)
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

        while ((numRead = mq_receive(mq, buffer, attr.mq_msgsize, NULL)) >= 0)
        {
            auto rom = mq_receive(mq, buffer, attr.mq_msgsize, nullptr); // Receiving the message
            receivedMessage = buffer;                                    // Converting the message to string
            cout << "Message received: " << buffer << endl;
            if (strcmp(buffer, "5") == 0)
            {
                flag = true;
                break;
            }

            // Options based on messages received from client
            if (receivedMessage.compare(0, 1, "1") == 0)
            {
                // Example for <clientSpace> = "client1"
                // Example for <destinationPath> = home/documents
                // Example for <sourcePath> = "/home/user/Desktop/file.txt"

                // fileManager.importFile(<clientSpace>, <sourcePath>, <destinationPath>);
                cout << "Importing file" << endl;
            }

            if (receivedMessage.compare(0, 1, "2") == 0)
            {
                // Example for <clientSpace> = "client1"
                // Example for <destinationPath> = "/home/user/Desktop/file.txt"
                // Example for <sourcePath> = "home/documents/"

                //fileManager.exportFile(<clientSpace>, <sourcePath>, <destinationPath>);
                cout << "Exporting file" << endl;
            }

            if (receivedMessage.compare(0, 1, "3") == 0)
            {
                // Example for <clientSpace> = "client1"
                // Example for <filename> = "file1.txt" || "home/file.txt"

                //fileManager.deleteFile(<clientSpace>, <filename>);
                cout << "Deleting file" << endl;
            }

            if (receivedMessage.compare(0, 1, "4") == 0)
            {
                // Example for <clientSpace> = "client1"

                //fileManager.showFiles(<clientSpace>);
                cout << "Listing files" << endl;
            }

            // clean message
            memset(buffer, 0, sizeof(buffer));
        }
    }
    mq_close(mq);
    mq_unlink(queue_name);
    return 0;
}

static void handler(int sig)
{
    cout << "\n----------------------------------------------"; 
    cout << "\nClosing the message queue";
    // Close and unlink the message queue
    mq_close(mq);
    mq_unlink(queue_name);
    exit(0); // Terminate the program
}