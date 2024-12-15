#ifndef SERVERMANAGER_HPP
#define SERVERMANAGER_HPP

#include "Client.hpp"
#include "Logger.hpp"
#include "Serverhandler.hpp"
#include "RequestRouter.hpp"
#include "Config_Parser.hpp"
#include "Cgi_Controller.hpp"

extern bool g_running;

class ServerManager
{
	public:
		ServerManager();
		ServerManager(const ServerManager &src);
		~ServerManager();

		void setup(std::string path);
		void run();
		void	handleClientRequest(Client &client, std::vector<Server> servers);
		void	handleClientResponse(Client &client);
		void	handleCGI();
		void	cleanUpCGI(Client &client, const std::string &logMessage);
		Server	getServer(std::vector<Server> servers, Request req);
		void	closeConnection(Client &client, std::string reason);

		void testCgi(Client &client, Route &route);

	private:
		std::vector<Serverhandler>	serverhandler;
		std::map<int, Client>		_clients;
		struct epoll_event			_ev;
		int							_epollFd;
		int							_eventFd;

		std::map<int, Cgi_Controller>	cgi_controllers;

		int		createEpoll();
		int		epollAddSockets();
		void	acceptNewConnection(Serverhandler handler);
		void	acceptNewCGIConnection(int clientFD);
		void	setNonBlocking(int clientSocket);
		void	checkTimeout();
		void	checkForCGI();
};

#endif
