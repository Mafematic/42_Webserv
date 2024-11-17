#ifndef SIMPLESOCKET_HPP
# define SIMPLESOCKET_HPP

#include <sys/socket.h>
#include <iostream>
#include <netinet/in.h>
#include <cstdlib>
#include <cstdio> 


class SimpleSocket
{
	private:
		struct sockaddr_in _address;
		int _sock;
		int _connection;

	public:
		// Constructor
		SimpleSocket(int domain, int service, int protocol, int port, u_long interface);
		// Pure virtual function to connect to a network
		virtual int connect_to_network(int sock, struct sockaddr_in address) = 0;
		// Function to test sockets and connections
		void test_connection(int);
		// Getter functions
		struct sockaddr_in get_address();
		int get_sock();
		int get_connection();
		// Setter functions
		void set_connection(int);
};

#endif 