#include "Client.hpp"

Client::Client(){}

Client::Client(int clientFd, Serverhandler handler) : clientFd(clientFd), lastActivity(time(NULL)), handler(handler){}

Client::Client(const Client &src)
{
	*this = src;
}

Client	&Client::operator=(const Client &src)
{
	if (this == &src)
		return *this;
	lastActivity = src.lastActivity;
	clientFd = src.clientFd;
	handler = src.handler;
	return *this;
}

Client::~Client(){}

time_t	Client::getLastActivity()
{
	return lastActivity;
}

Serverhandler Client::getServerhandler()
{
	return handler;
}

int	Client::getFd()
{
	return clientFd;
}

void	Client::updateLastActivity()
{
	lastActivity = time(NULL);
}

std::basic_string<char> &Client::getBuffer()
{
    return _buffer;
}

void Client::appendToBuffer(const std::basic_string<char> &data)
{
    _buffer += data;
}

void Client::clearBuffer()
{
    _buffer.clear();
}