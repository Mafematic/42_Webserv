#include "Networking/ListeningSocket.hpp"
#include "Servers/Server.hpp"
#include "Request/Request.hpp"
#include <arpa/inet.h>

int main()
{
	int port = 8082;
	in_addr_t ip = INADDR_LOOPBACK;
	// std::cout << ip << std::endl;
	// // ip = inet_addr("127.0.0.1");
	// std::cout << ip << std::endl;
	// ip = htonl(0x7F000001); 
	// std::cout << ip << std::endl;

	int backlog = 10;
	std::string root = "root"; 

	Server t(ip, port, backlog, root);  // INADDR_ANY INADDR_LOOPBACK;
	return 0;
}
