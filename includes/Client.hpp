#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <ctime>

class Client
{
	private:
		int		clientFd;
		time_t	lastActivity;
	public:
		Client();
		Client(int clientFd);
		Client(const Client &src);
		~Client();

		Client	&operator=(const Client &src);

		time_t getLastActivity();
		int		getFd();
		void	updateLastActivity();

};

#endif
