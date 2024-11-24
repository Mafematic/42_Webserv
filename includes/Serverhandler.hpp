#ifndef SERVERHANDLER_HPP
#define SERVERHANDLER_HPP

#include "webserv.hpp"
#include "Server.hpp"

class Serverhandler
{
	public:
		Serverhandler();
		Serverhandler(int port, std::string ip);
		Serverhandler(const Serverhandler &src);
		Serverhandler &operator=(const Serverhandler &src);
		~Serverhandler();
		int			getSocket() const;
		int			getPort() const;
		std::string	getIp() const;
		void		setup();

		std::vector<Server> _servers;
	private:
		struct sockaddr_in _address;
		int	_serverSocket;
		int	_port;
		std::string _ip;
};

#endif
