#ifndef SERVERHANDLER_HPP
#define SERVERHANDLER_HPP

#include "webserv.hpp"
#include "Server.hpp"

class Serverhandler {
	public:
		Serverhandler();
		Serverhandler(int port, std::string ip);
		Serverhandler(const Serverhandler &src);
		~Serverhandler();
		int		getSocket() const;
		int		getPort() const;
		void	setup();
	private:
		std::vector<Server> _servers;
		struct sockaddr_in _address;
		int	_serverSocket;
		int	_port;
		std::string _ip;
};

#endif
