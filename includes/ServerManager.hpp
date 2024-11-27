#ifndef SERVERMANAGER_HPP
#define SERVERMANAGER_HPP

#include "Client.hpp"
#include "Serverhandler.hpp"
#include "RequestRouter.hpp"
#include "Config_Parser.hpp"

extern bool g_running;

class ServerManager
{
	public:
		ServerManager();
		ServerManager(const ServerManager &src);
		~ServerManager();

		void setup(std::string path);
		void run();
		void handleClient(Client &client, std::vector<Server> servers);
		Server	getServer(std::vector<Server> servers, Request req);
		void 	sendClientResponse(Client &client, std::string response);
		void	closeConnection(Client &client, std::string reason);

	private:
		std::vector<Serverhandler>	serverhandler;
		std::map<int, Client>		_clients;
		struct epoll_event			_ev;
		int							_epollFd;
		int							_eventFd;

		int		createEpoll();
		int		epollAddSockets();
		void	acceptNewConnection(Serverhandler handler);
		void	setNonBlocking(int clientSocket);
		void	checkTimeout();
};

#endif
