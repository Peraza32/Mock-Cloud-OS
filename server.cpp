#include <iostream>

#include <cstring>

#include <mqueue.h> // message queue header

#include <sys/stat.h> // mode constants

#include <fcntl.h> // O_* constants

#include <unistd.h> // fork

#include <signal.h> // signal

#include <pthread.h> // threads

#include <vector>

#include <semaphore.h>

// hand made libraries

#include "Login.hpp"

#include "FileManager.hpp"

#include "Utility.hpp"

#include "Message.hpp"

#include "ThreadInfo.hpp"

using namespace std;

#define NOTIFY_SIG SIGUSR1 // Notification signal from timer

static void handler(int); // Signal handler

void *clientLoop(void *info);

mqd_t mq;

const char *queue_name = "/MessageQueue";

static int threadCount = 0;

static sem_t sem;

int main()

{

    //-------------------------------------------------------------------------------------

    // Interruption signal

    signal(SIGINT, handler); // For handling the ^C signal, this way we close the message queue

    //-------------------------------------------------------------------------------------

    // Variable initialization and Constants declaration

    //-------------------------------------------------------------------------------------

    Message msg = Message();

    Message response = Message();

    // Signal structures

    struct sigevent sev;

    struct sigaction sa;

    sigset_t blockMask, emptyMask;

    FileManager manager = FileManager();

    Login login = Login("files/");

    // Queue variables and parameters

    string message, receivedMessage;

    mq_attr attr;

    ssize_t numRead; // Number of bytes read

    const int permission = 0666;

    bool flag = false;

    vector<string> unformattedInput;

    // Temporal response queue

    string responseQueue;

    mq_attr resAttr;

    // threads

    vector<pthread_t> threads;

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

    sigemptyset(&blockMask); // Block timer signal temporarily

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
    }

    cout << "Message queue created successfully" << endl;

    //-------------------------------------------------------------------------------------

    // Semaphore

    sem_init(&sem, 0, 1);

    //-------------------------------------------------------------------------------------

    while (flag != true)

    {

        sigsuspend(&emptyMask); // Wait for notification signal

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

            receivedMessage = buffer; // Converting the message to string

            cout << "Message received: " << buffer << endl;

            msg.createFromString(receivedMessage);

            // creating response queue for the client

            // responseQueue = "/" + msg.getSender() + "ResponseQueue";

            // mqd_t responseMQ = mq_open(responseQueue.c_str(), O_RDWR | O_NONBLOCK, permission, NULL);

            if (msg.getOption() == "0")

            {

                // shutdown

                cout << "Shutting down..." << endl;

                flag = true;

                // join threads

                if (threads.size() > 0)

                {

                    for (int i = 0; i < threads.size(); i++)

                    {

                        pthread_join(threads[i], NULL);
                    }
                }

                break;
            }

            // creating thread info

            ThreadInfo *info = new ThreadInfo();

            info->msg = msg;

            info->user = msg.getSender();

            info->login = &login;

            // creating thread

            pthread_t thread;

            pthread_create(&thread, NULL, (void *(*)(void *))clientLoop, (void *)info);

            threads.push_back(thread);

            // clean message

            msg.clear();

            // response.clear();

            receivedMessage.clear();

            message.clear();

            memset(buffer, 0, sizeof(buffer));

            // join threads

            for (int i = 0; i < threads.size(); i++)

            {

                pthread_join(threads[i], NULL);
            }
        }
    }

    mq_close(mq);

    mq_unlink(queue_name);

    // destroying semaphore

    sem_destroy(&sem);

    return 0;
}

static void handler(int sig)

{

    /*cout << "\n----------------------------------------------";



    cout << "\nClosing the message queue";



    // Close and unlink the message queue



    mq_close(mq);



    mq_unlink(queue_name);



    exit(0); // Terminate the program*/
}

void *

clientLoop(void *info)

{

	cout << "Hola";

    // Variables

    ThreadInfo *threadInfo = (ThreadInfo *)info;

    Message msg = threadInfo->msg;

    FileManager fManager = FileManager();

    Message response = Message();

    Login *login = threadInfo->login;

    string responseQueue = "/" + msg.getSender() + "ResponseQueue";

    mqd_t responseMQ = mq_open(responseQueue.c_str(), O_RDWR | O_NONBLOCK, 0666, NULL);

    int lock = threadCount;

    response.setSender(msg.getSender());

    // Options based on messages received from client

    if (msg.getOption() == "1")

    {

        // message from queue must contain:

        //  option, username, path where the file will be saved, path of the file to be imported

        //  example: "1|username|sourcePath|destinationPath"

        //  here the destinationPath is inside the clientSpace

        cout << "Importing file..." << endl;

        // fileManager.importFile(<clientSpace>, <sourcePath>, <destinationPath>, filename)

        try

        {

            response.setSender(msg.getSender());

            fManager.importFile(msg.getSender(), msg.getOneMessage(0), msg.getOneMessage(1), msg.getOneMessage(2));

            response.setOption("1");

            response.addMessage("1");

            response.addMessage("File imported Successfully");
        }

        catch (const std::exception &e)

        {

            std::cerr << e.what() << '\n';

            response.setOption("1");

            response.addMessage("0");

            response.addMessage("File not imported");
        }

        mq_send(responseMQ, response.toString().c_str(), response.toString().length(), 0);
    }

    if (msg.getOption() == "2")

    {

        // message from queue must contain:

        //  option, username, path where the file is saved, path of the file to be exported

        //  example: "2|username|sourcePath|destinationPath"

        //  here the sourcePath is inside the clientSpace

        cout << "Exporting file..." << endl;

        try

        {

            response.setSender(msg.getSender());

            fManager.exportFile(msg.getSender(), msg.getOneMessage(0), msg.getOneMessage(1), msg.getOneMessage(2));

            response.setOption("2");

            response.addMessage("File exported Successfully");
        }

        catch (const std::exception &e)

        {

            std::cerr << e.what() << '\n';

            response.setOption("2");

            response.addMessage("0");

            response.addMessage("File not exported");
        }

        mq_send(responseMQ, response.toString().c_str(), response.toString().length(), 0);
    }

    if (msg.getOption() == "3")

    {

        cout << "Creating file" << endl;

        response.setOption("3");

        response.setSender(msg.getSender());

        if (fManager.createFile(msg.getSender(), msg.getMessages()[0]) != -1)

        {

            response.addMessage("1");

            response.addMessage("File created");

            mq_send(responseMQ, response.toString().c_str(), response.toString().length(), 0);
        }

        else

        {

            response.addMessage("0");

            response.addMessage("File not created");

            mq_send(responseMQ, response.toString().c_str(), response.toString().length(), 0);
        }
    }

    if (msg.getOption() == "4")

    {

        // message from queue must contain:

        // option, username, path where the file is saved, including the name of the file to delete

        // example: "4|username|path\filename"

        cout << "Deleting files" << endl;

        response.setOption("4");

        response.setSender(msg.getSender());

        if (fManager.deleteFile(msg.getSender(), msg.getOneMessage(0), msg.getOneMessage(1)) != -1)

        {

            response.addMessage("1");

            response.addMessage("File deleted");
        }

        else

        {

            response.addMessage("0");

            response.addMessage("File not deleted");
        }

        mq_send(responseMQ, response.toString().c_str(), response.toString().length(), 0);
    }

    if (msg.getOption() == "5")

    {

        string files;

        cout << "Listing files" << endl;

        // Setting up response

        response.setOption("5");

        response.setSender(msg.getSender());

        fManager.showFiles(msg.getSender(), &files);

        response.addMessage(files);

        // Sending response

        mq_send(responseMQ, response.toString().c_str(), response.toString().length(), 0);
    }

    if (msg.getOption() == "6")

    {

        cout << "Sending path file..." << endl;

        string path;

        // Setting up response

        response.setOption("6");

        response.setSender(msg.getSender());

        path = fManager.writeReadFile(msg.getSender(), msg.getOneMessage(0) + msg.getOneMessage(1));

        response.addMessage(path);

        // Sending response

        mq_send(responseMQ, response.toString().c_str(), response.toString().length(), 0);
    }

    if (msg.getOption() == "R")

    {

        // extracting username and password from the message

        sem_wait(&sem);

        cout << "Registering new user" << endl;

        if (login->registerUser(msg.getSender(), msg.getOneMessage(0)))

        {

            cout << "New user registered" << endl;

            fManager.createClientDirectory(msg.getSender());

            response.setSender(msg.getSender());

            response.setOption("R");

            response.addMessage("1");

            response.addMessage("User registered");

            mq_send(responseMQ, response.toString().c_str(), response.toString().length(), 0);
        }

        else

        {

            response.setSender(msg.getSender());

            response.setOption("R");

            response.addMessage("0");

            response.addMessage("User not registered");

            mq_send(responseMQ, response.toString().c_str(), response.toString().length(), 0);
        }

        //  clearing vector

        response.clear();

        sem_post(&sem);
    }

    if (msg.getOption() == "L")

    {

        sem_wait(&sem);

        cout << "Logging..." << endl;

        if (login->login(msg.getSender(), msg.getMessages()[0]))

        {

            cout << "User logged" << endl;

            response.setSender(msg.getSender());

            response.setOption("L");

            response.addMessage("1");

            response.addMessage("User logged");

            cout << response.toString() << endl;

            mq_send(responseMQ, response.toString().c_str(), response.toString().length(), 0);
        }

        else

        {

            cout << "User not logged" << endl;

            response.setSender(msg.getSender());

            response.setOption("L");

            response.addMessage("0");

            response.addMessage("User not logged");

            mq_send(responseMQ, response.toString().c_str(), response.toString().length(), 0);
        }

        sem_post(&sem);
    }

    mq_close(responseMQ);

    // mq_unlink(msg.getSender().c_str());

    return NULL;
}
