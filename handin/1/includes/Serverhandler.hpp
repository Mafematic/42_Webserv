#ifndef SERVERHANDLER_HPP
#define SERVERHANDLER_HPP

#include "webserv.hpp"
#include "Logger.hpp"
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
		std::vector<Server>	getServers() const;
		void	addServer(Server server);
		std::string	getIp() const;
		void		setup();

	private:
		struct sockaddr_in _address;
		std::vector<Server> _servers;
		int	_serverSocket;
		int	_port;
		std::string _ip;
};

#endif
