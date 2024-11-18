#ifndef SERVERMANAGER_HPP
#define SERVERMANAGER_HPP

#include "Client.hpp"
#include "Serverhandler.hpp"

extern bool g_running;

class ServerManager
{
	public:
		ServerManager();
		ServerManager(const ServerManager &src);
		~ServerManager();

		void setup(std::string path);
		void run();
		void handleClient(int clientSocket, std::vector<Server> server);
		std::string readRequest(int clientSocket);
		int getContentLength(const std::string& request);
		void sendClientResponse(int clientSocket, const std::string &response, bool keepAlive);
		std::string readRequestBody(int clientSocket, std::string &buffer, int contentLength);

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
