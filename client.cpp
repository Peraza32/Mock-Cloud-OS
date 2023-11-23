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

    // Parameters for the queue
    const char *queue_name = "/ServerQueue";
    const int queue_permissions = 0666;

    // Opening the message queue
    mqd_t msg_queue = mq_open(queue_name, O_RDWR | O_NONBLOCK, queue_permissions, NULL);

    // checking if the queue exists and was open
    if (msg_queue == -1)
    {
        cout << "Message queue not open. Waking up server" << std::endl;
        // if queue was not open, then calling the server to create it
        int pid = fork();
        if (pid == 0)
            execl("server", "server", NULL);
        else
        {
            msg_queue = mq_open(queue_name, O_WRONLY | O_NONBLOCK, queue_permissions, NULL);
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
    while (true)
    {
        cout << "Select an option: ";
        cout << "\n1. Import file to server";
        cout << "\n2. Export file from server";
        cout << "\n3. Delete File from server";
        cout << "\n4. List files in server";
        cout << "\n5. Exit" << endl;
        cin >> message;

        if (message == "exit")
        {
            mq_send(msg_queue, message.c_str(), message.size(), 0);
            break;
        }

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
        //clearing response
        memset(&response, 0, sizeof(response));
    }

    // closing the queue
    mq_close(msg_queue);
    mq_unlink(queue_name);
    return 0;
}

static void handler(int sig)
{
    // sig = 1;
}