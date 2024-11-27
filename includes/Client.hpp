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

		std::string			response;

		bool				_isChunked;
		bool				_done;
		size_t				_contentLength;
		size_t				_bytesReceived;
		std::string			_buffer;

		Server	server;
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

		int		readRequest();
		bool	requestComplete();
		void	clearRequest();
		void	getContentLength();

		std::string getBuffer();
		void clearBuffer();
		void setChunked(bool isChunked);
		bool isChunked();
		void setServer(Server server);
		Server getServer();
		void setResponse(std::string response);
		std::string getResponse();
		bool	isDone();
};

#endif
