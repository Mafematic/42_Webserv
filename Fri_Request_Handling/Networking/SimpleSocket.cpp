#include "SimpleSocket.hpp"

// Default Constructor

SimpleSocket::SimpleSocket(int domain, int service, int protocol, int port, u_long interface)
{
	// Define address structure
	_address.sin_family = domain;
	_address.sin_port = htons(port);
	_address.sin_addr.s_addr = htonl(interface);

	// Establish Socket
	_sock = socket(domain, service, protocol);
	test_connection(_sock);

	// Establish network connection
	//_connection = connect_to_network(_sock, _address);
	//test_connection(_connection);
}

// Test connection function

void SimpleSocket::test_connection(int item_to_test)
{
	// Confirms that the socket or connection has been properly established
	if (item_to_test < 0)
	{
		perror("Failed to connect... ");
		exit(EXIT_FAILURE);
	}
}

// Getter functions

struct sockaddr_in SimpleSocket::get_address()
{
	return _address;
}

int SimpleSocket::get_sock()
{
	return _sock;
}

int SimpleSocket::get_connection()
{
	return _connection;
}

// Setter functions

void SimpleSocket::set_connection(int con)
{
	_connection = con;
}