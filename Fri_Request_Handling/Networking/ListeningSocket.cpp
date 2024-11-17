#include "ListeningSocket.hpp"

// Constructor
ListeningSocket::ListeningSocket(int domain, int service, int protocol,
	int port, u_long interface, int bcklg) : BindingSocket(domain, service, protocol,
	port, interface)
{
	_backlog = bcklg;
	start_listening();
	test_connection(_listening);
}

void ListeningSocket::start_listening()
{
	_listening = listen(get_sock(), _backlog);
}