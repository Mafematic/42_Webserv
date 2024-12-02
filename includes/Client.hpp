#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <ctime>

#include "Serverhandler.hpp"
#include "Request.hpp"
#include "RequestRouter.hpp"

class Client
{
	private:
		int				clientFd;
		time_t			lastActivity;
		Serverhandler	handler;

		std::string			response;
		size_t				_responselength;
		size_t				_responseSentBytes;

		bool				_isChunked;
		std::string			_chunkFullString;
		size_t				_currentChunkSize;

		size_t				_contentLength;
		size_t				_bytesReceived;
		std::basic_string<char>			_buffer;

		Server	server;
		Request	req;
		// Route & Route
		// check_header_received()

		//struct sockaddr_in	client_addr;

		int					_client_port;
		char				_client_ip[INET_ADDRSTRLEN];

	public:
		Client();
		Client(int clientFd, Serverhandler handler, struct sockaddr_in client_addr);
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

		int		processChunkedData();

		int		sendResponse();
		void	clearResponse();

		std::string getRequestStr();
		void clearBuffer();
		void setChunked(bool isChunked);
		bool isChunked();
		void setServer(std::vector<Server> servers);
		Server getServer();
		void setResponse();
		std::string getResponse();

		void	setRequest();
		Request	getRequest();

		int		getPort();
		std::string	getIp();
};

#endif
