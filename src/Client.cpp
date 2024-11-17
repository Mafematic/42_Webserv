#include "Client.hpp"

Client::Client(){}

Client::Client(int clientFd) : clientFd(clientFd), lastActivity(time(NULL)){}

Client::Client(const Client &src)
{
	*this = src;
}

Client::~Client(){}

Client	&Client::operator=(const Client &src)
{
	if (this == &src)
		return *this;
	lastActivity = src.lastActivity;
	clientFd = src.clientFd;
	return *this;
}

time_t	Client::getLastActivity()
{
	return lastActivity;
}

int	Client::getFd()
{
	return clientFd;
}

void	Client::updateLastActivity()
{
	lastActivity = time(NULL);
}
