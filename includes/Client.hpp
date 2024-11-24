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
		std::basic_string<char> _buffer;
		// Server & server
		// Route & Route
		// Request & cur_request


		// check_header_received()


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

		std::basic_string<char> &getBuffer();
		void appendToBuffer(const std::basic_string<char> &data);
		void clearBuffer();

};

#endif
