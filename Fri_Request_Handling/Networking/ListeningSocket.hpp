#ifndef LISTENINGSOCKET_HPP
# define LISTENINGSOCKET_HPP

#include "BindingSocket.hpp"


class ListeningSocket: public BindingSocket
{
	private:
		int _backlog;
		int _listening;

	public:
		// Constructor
		ListeningSocket(int domain, int service, int protocol, int port,
			u_long interface, int bcklg);
		void start_listening();
		virtual ~ListeningSocket() {}

};

#endif 