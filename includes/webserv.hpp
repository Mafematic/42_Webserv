#pragma once

#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define LIGHT BLUE "\033[94m"
#define PURPLE "\033[35m"
#define RESET "\033[0m"

#define PORT1 8080;
#define PORT2 8081;

#define MAX_EVENTS 1000
#define EPOLL_TIMEOUT 1000
#define BUFFER_SIZE 100000
#define CLIENT_TIMEOUT 12
//#define MAX_UPLOAD_SIZE 1000000
#define CGI_TIMEOUT_SEC 10

enum	e_cgi_status
{
	CGI_RUNNING,
	CGI_EXITED_NORMAL,
	CGI_EXITED_ERROR,
	CGI_KILLED_TIMEOUT
};

enum	LogLevel
{
	DEBUG,
	INFO,
	TRACE,
	WARNING,
	ERROR
};

enum	State
{
	READ_ERROR = -1,
	CLIENT_DISCONNECTED = 0,
	READ_NOT_COMPLETE = 1,
	READ_COMPLETE = 2,
	SEND_ERROR = -1,
	SEND_NOT_COMPLETE = 1,
	SEND_COMPLETE = 2
};


#include <iostream>
#include <string>
#include <fstream>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> //?
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <cstdio>
#include <sstream>
#include <vector>
#include <map>
#include <ctime>
#include <csignal>
#include <iomanip>
#include <cctype>
#include <limits>
#include <climits>
#include <cfloat>
#include <stdint.h>
#include <algorithm>
#include <deque>
#include <list>
#include <sys/types.h>
#include <sys/stat.h>
#include <numeric>
#include <cstdlib>
#include <sys/wait.h>
#include <dirent.h>
