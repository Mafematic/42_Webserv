#pragma once

#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define TURKIZ "\033[36m"
#define RESET "\033[0m"

#define PORT1 8080;
#define PORT2 8081;

#define MAX_EVENTS 1000
#define BUFFER_SIZE 10
#define CLIENT_TIMEOUT 12
#define MAX_UPLOAD_SIZE 1000000

enum	State
{
	READ_ERROR = -1,
	CLIENT_DISCONNECTED = 0,
	READ_NOT_COMPLETE = 1,
	READ_COMPLETE = 2
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
