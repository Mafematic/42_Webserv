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
		Route route;

		bool	_cgi_finished;
		bool	_cgi;

		int					_client_port;
		std::string			_client_ip;

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

		int		readRequest(int fd);
		bool	requestComplete();
		void	clearRequest();
		void	getContentLength();

		int		processChunkedData();

		int		sendResponse();
		void	clearResponse();

		void clearBuffer();
		void setChunked(bool isChunked);
		bool isChunked();
		void setServer(std::vector<Server> servers);
		Server getServer();
		void		generateResponse();
		void		setResponse(std::string response);
		std::string	getResponse();

		void	setRequest();
		std::string	getRequestStr();
		void		appendToRequestStr(std::string str, int bytesread);
		Request	getRequest();

		int		getPort();
		std::string	getIp();
		void setRoute(const Server &server);
		Route getRoute() const;

		bool	getCGIfinished();
		void	setCGIfinished(bool status);
		bool	getCGI();
		void	setCGI(bool status);

		std::string	getPrintName();
};

#endif
