#ifndef BINDINGSOCKET_HPP
# define BINDINGSOCKET_HPP

#include "SimpleSocket.hpp"

class BindingSocket: public SimpleSocket
{
	private:
		

	public:
		// Constructor
		BindingSocket(int domain, int service, int protocol, int port,
			u_long interface);

		// Virtual function from parent
		int connect_to_network(int sock, struct sockaddr_in address);

};

#endif 