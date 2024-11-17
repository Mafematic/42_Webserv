#ifndef CONNECTINGSOCKET_HPP
# define CONNECTINGSOCKET_HPP

#include "SimpleSocket.hpp"


class ConnectingSocket: public SimpleSocket
{
	private:
		

	public:
		// Constructor
		ConnectingSocket(int domain, int service, int protocol, int port,
			u_long interface);

		// Override of virtual function from parent
		int connect_to_network(int sock, struct sockaddr_in address);

};

#endif 