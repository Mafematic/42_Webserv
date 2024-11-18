#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <ctime>

#include "Serverhandler.hpp"

class Client
{
	private:
		int		clientFd;
		time_t	lastActivity;
		Serverhandler handler;
	public:
		Client();
		Client(int clientFd, Serverhandler handler);
		Client(const Client &src);
		~Client();

		Client	&operator=(const Client &src);

		time_t	getLastActivity();
		Serverhandler getServerhandler();
		int		getFd();
		void	updateLastActivity();

};

#endif
