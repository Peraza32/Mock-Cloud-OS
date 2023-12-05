#ifndef THREADINFO_HPP
#define THREADINFO_HPP

#include <iostream>
#include <cstring>
#include <mqueue.h>   // message queue header
#include <sys/stat.h> // mode constants
#include <fcntl.h>    // O_* constants
#include <unistd.h>   // fork
#include <signal.h>   // signal
#include <pthread.h>  // threads
#include <vector>
#include "Login.hpp"
#include "FileManager.hpp"
#include "Utility.hpp"
#include "Message.hpp"

using namespace std;

struct ThreadInfo
{

    mqd_t queue;
    string user;
    Message msg;
    FileManager fileManager;
    Login *login;
};

#endif