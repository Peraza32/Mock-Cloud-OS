#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <string>
#include <cstring>
#include <fstream>
#include <signal.h>

using namespace std;

#define NOTIFY_SIG SIGUSR1
static void handler(int);

string Menu(bool &flag);

int main()
{

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
            execl("./", "Server", NULL);
        else
        {
            mq = mq_open(queue_name, O_WRONLY | O_NONBLOCK, queue_permissions, NULL);
        }
    }

    sigemptyset(&blockMask); // Initializing the signal mask
    sigaddset(&blockMask, NOTIFY_SIG);
    sigemptyset(&sa.sa_mask); // Initializing the signal mask
    sa.sa_flags = 0;          // Setting the flags
    sa.sa_handler = handler;
    sev.sigev_notify = SIGEV_SIGNAL; // Setting the notification signal
    sev.sigev_signo = NOTIFY_SIG;

    char *response;
    // Reading messages from terminal, until exits with "exit"
    while (flag)
    {
        message = Menu(flag);

        if (message == "6")
        {
            mq_send(mq, message.c_str(), message.size(), 0);
            break;
        }

        cout << message << endl;
        mq_send(mq, message.c_str(), message.size(), 0);

        /*
        // Sending the message to the server
        mq_send(msg_queue, message.c_str(), message.size(), 0);
        // Waiting for server to resond
        // Getting message from server
        mq_notify(msg_queue, &sev);
        sigemptyset(&emptyMask);
        sigsuspend(&emptyMask);
        memset(&message, 0, sizeof(message));
        mq_receive(msg_queue, response, 1024, NULL);

        cout << "Response from server: " << response << endl;
        // clearing response
        memset(&response, 0, sizeof(response));*/
        message.clear();
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

string Menu(bool &flag)
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
        message = "1 " + location + filename;

        // clearing variables
        location.clear();
        filename.clear();

        break;
    case '2':
        cout << "Enter the name of the file to retrieve from the server:" << endl;
        cin >> filename;
        cout << "Enter the location where the file should be exported. \n Remeber, you must have access to this location" << endl;
        cin >> location;
        message = "2 " + filename + location;

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