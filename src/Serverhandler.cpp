#include "Serverhandler.hpp"

Serverhandler::Serverhandler() {}

Serverhandler::Serverhandler(int port, std::string ip) : _port(port), _ip(ip)
{
	setup();
}

Serverhandler::Serverhandler(const Serverhandler &src)
{
	*this = src;
}

Serverhandler &Serverhandler::operator=(const Serverhandler &src)
{
	if (this == &src)
		return *this;
	_port = src._port;
	_ip = src._ip;
	_serverSocket = src._serverSocket;
	_address = src._address;
	_servers = src._servers;
	return *this;
}

Serverhandler::~Serverhandler() {}

int Serverhandler::getSocket() const
{
	return _serverSocket;
}

int Serverhandler::getPort() const
{
	return _port;
}

std::string Serverhandler::getIp() const
{
	return _ip;
}

void Serverhandler::setup()
{
	_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	std::cout << GREEN << "[Socket created] : " <<  _serverSocket << RESET << std::endl;
	if (_serverSocket < 0)
	{
		std::cerr << "Error: socket creation failed" << std::endl;
		return;
	}

	int opt = 1;
	if (setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		std::cerr << "Error: setsockopt failed" << std::endl;
		close(_serverSocket);
		return;
	}

	memset(&_address, 0, sizeof(_address));
	_address.sin_family = AF_INET;
	_address.sin_port = htons(_port);

	if (inet_pton(AF_INET, _ip.c_str(), &_address.sin_addr) <= 0)
	{
		std::cerr << "Error: invalid IP address format or inet_pton failed" << std::endl;
		close(_serverSocket);
		return;
	}

	if (bind(_serverSocket, (struct sockaddr *)&_address, sizeof(_address)) < 0)
	{
		std::cerr << "Error: binding failed" << std::endl;
		close(_serverSocket);
		return;
	}
	if (listen(_serverSocket, MAX_EVENTS) < 0)
	{
		std::cerr << "Error: listening failed" << std::endl;
		close(_serverSocket);
		return;
	}
}

std::vector<Server> Serverhandler::getServers() const
{
	return _servers;
}

void	Serverhandler::addServer(Server server)
{
	_servers.push_back(server);
}
