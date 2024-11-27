#include "ServerManager.hpp"

void ServerManager::setup(std::string config_path)
{
	int	duplicate_flag = false;
	std::vector<Server> _server_config;
	Config_Parser parser(config_path);

	_server_config = parser.parse_config();
	for (std::vector<Server>::iterator it = _server_config.begin(); it != _server_config.end(); ++it)
	{
		for(std::vector<Serverhandler>::iterator tmp_it = serverhandler.begin(); tmp_it != serverhandler.end(); ++tmp_it)
		{
			if (it->get_port() == tmp_it->getPort() && it->get_ip() == tmp_it->getIp())
			{
				tmp_it->addServer(*it);
				duplicate_flag = true;
				std::cout << YELLOW << "[Serverduplicat detected] added to existing socket: " << tmp_it->getSocket() << RESET << std::endl;
				break ;
			}
		}
		if (duplicate_flag == false)
		{
			Serverhandler server(it->get_port(), it->get_ip());
			server.addServer(*it);
			serverhandler.push_back(server);
		}
	}
}

void ServerManager::sendClientResponse(Client &client, std::string response)
{
	ssize_t bytesSent = send(client.getFd(), response.c_str(), response.length(), 0); // 0 = no flags
	if (bytesSent < 0)
	{
		perror("Failed to send response");
	}
	else
	{
		client.clearRequest();
		_ev.events = EPOLLIN | EPOLLET;
		_ev.data.fd = client.getFd();
		if (epoll_ctl(_epollFd, EPOLL_CTL_MOD, client.getFd(), &_ev) == -1)
			perror("Failed to modify epoll event");
	}
}

Server ServerManager::getServer(std::vector<Server> servers, Request req)
{
	Server server;
	if (servers.size() == 1)
		server = servers[0];
	else
	{
		server = servers[0];
		for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); ++it)
		{
			std::vector<std::string> server_names = it->get_server_name();
			for (std::vector<std::string>::iterator it2 = server_names.begin(); it2 != server_names.end(); ++it2)
			{
				std::string host = req.getHeader("Host");
				host = host.substr(0, host.find(":"));
				if (host == *it2)
					return *it;
			}
		}
	}
	return server;
}

void ServerManager::handleClient(Client &client, std::vector<Server> servers)
{
	int status = client.readRequest();
	if (status == READ_ERROR)
	{
		closeConnection(client, "[Failed to read request]");
		return;
	}
	else if (status == CLIENT_DISCONNECTED)
	{
		closeConnection(client, "[Client disconnected]");
		return;
	}
	else if (status == READ_NOT_COMPLETE)
	{
		struct epoll_event ev;
		ev.events = EPOLLIN | EPOLLET;
		ev.data.fd = client.getFd();
		if (epoll_ctl(_epollFd, EPOLL_CTL_MOD, client.getFd(), &ev) == -1)
		{
			perror("Failed to modify epoll event");
			return;
		}
		return;
	}

	//std::cout << TURKIZ << "++++ Buffer: " << client.getBuffer() << RESET << std::endl;

	Request req(client.getBuffer());
	client.setServer(getServer(servers, req));
	std::string response = RequestRouter::route(req, client.getServer());
	client.setResponse(response);
	client.clearBuffer();

	//std::cout << "++++ Response" << response << std::endl;

	struct epoll_event ev;
	ev.events = EPOLLOUT | EPOLLET;
	ev.data.fd = client.getFd();
	std::cout << BLUE << "[Client ready for processing] : ClientFd " << client.getFd() << RESET << std::endl;
	if (epoll_ctl(_epollFd, EPOLL_CTL_MOD, client.getFd(), &ev) == -1)
	{
		perror("Failed to modify epoll event");
		return;
	}
}

void ServerManager::run()
{
	struct epoll_event events[MAX_EVENTS];

	if (createEpoll() == -1 || epollAddSockets() == -1) {
		return;
	}

	std::cout << GREEN << "Server Manager started, waiting for connections..." << RESET << std::endl;

	Serverhandler tmp;
	while (g_running)
	{
		int eventCount = epoll_wait(_epollFd, events, MAX_EVENTS, 1000);
		for (int i = 0; i < eventCount; i++)
		{
			_eventFd = events[i].data.fd;
			if (events[i].events & EPOLLIN)
			{
				// Check if event is on a listening server socket
				bool isServerSocket = false;
				for (std::vector<Serverhandler>::iterator it = serverhandler.begin(); it != serverhandler.end(); ++it)
				{
					if (_eventFd == it->getSocket())
					{
						isServerSocket = true;
						tmp = *it;
						break;
					}
				}

				if (isServerSocket == true)
					acceptNewConnection(tmp);
				else
				{
					if (_clients.find(_eventFd) != _clients.end())
					{
						_clients[_eventFd].updateLastActivity();
						if (_clients[_eventFd].isDone())
							std::cout << GREEN << "[New Request] : ClientFd " << _clients.find(_eventFd)->second.getFd() << RESET << std::endl;
						handleClient(_clients[_eventFd], _clients[_eventFd].getServerhandler().getServers());
					}
				}
			}
			else if (events[i].events & EPOLLOUT)
			{
				// Handle write events
				if (_clients.find(_eventFd) != _clients.end())
				{
					_clients[_eventFd].updateLastActivity();
					std::cout << GREEN << "[Sending Response] : ClientFd " << _clients[_eventFd].getFd() << RESET << std::endl;
					sendClientResponse(_clients[_eventFd], _clients[_eventFd].getResponse());
				}
			}
		}
		checkTimeout();
	}
	std::cout << RED << "Server shutting down..." << RESET << std::endl;
}

void	ServerManager::checkTimeout()
{
	for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); )
	{
		if (time(NULL) - it->second.getLastActivity() > CLIENT_TIMEOUT)
		{
			Client tmp = it->second;
			it++;
			closeConnection(tmp, "[Client timeout]");
		}
		else
			++it;
	}
}

void	ServerManager::closeConnection(Client &client, std::string reason)
{
	std::cout << YELLOW << reason << " Client disconnected : ClientFd " << client.getFd() << RESET << std::endl;
	epoll_ctl(_epollFd, EPOLL_CTL_DEL, client.getFd(), NULL);
	close(client.getFd());
	client.clearBuffer();
	_clients.erase(client.getFd());
}

void	ServerManager::acceptNewConnection(Serverhandler handler)
{
	int clientSocket = accept(_eventFd, NULL, NULL);
	if (clientSocket >= 0)
	{
		setNonBlocking(clientSocket);

		_ev.events = EPOLLIN | EPOLLET;
		_ev.data.fd = clientSocket;
		if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, clientSocket, &_ev) == -1)
		{
			std::cerr << "Error: epoll_ctl failed for client socket" << std::endl;
			close(clientSocket);
			return ;
		}
		std::cout << GREEN << "Accepted new client connection : ClientFd " << clientSocket << RESET << std::endl;
		Client new_client(clientSocket, handler);
		_clients[clientSocket] = new_client;
	}
	else
		std::cerr << "Accept failed." << std::endl;
}

void	ServerManager::setNonBlocking(int socketFd)
{
	// Get the current flags on the socket
	int flags = fcntl(socketFd, F_GETFL, 0);
	if (flags == -1)
	{
		std::cerr << "Error: Failed to get flags for socket. Error: " << strerror(errno) << std::endl;
		return;
	}

	// Set the socket to non-blocking by adding O_NONBLOCK to its flags
	if (fcntl(socketFd, F_SETFL, flags | O_NONBLOCK) == -1)
	{
		std::cerr << "Error: Failed to set socket to non-blocking mode. Error: " << strerror(errno) << std::endl;
		return ;
	}
}

int	ServerManager::createEpoll()
{
	_epollFd = epoll_create1(0);
	if (_epollFd == -1)
	{
		std::cerr << "Error: epoll_create1 failed" << std::endl;
		return -1;
	}
	return 1;
}

int	ServerManager::epollAddSockets()
{
	for (std::vector<Serverhandler>::iterator it = serverhandler.begin(); it != serverhandler.end(); ++it)
	{
		int serverSocket = it->getSocket();
		if (serverSocket < 0)
		{
			std::cerr << "Error: Invalid server socket (fd: " << serverSocket << ")" << std::endl;
			return -1;
		}

		_ev.events = EPOLLIN;
		_ev.data.fd = serverSocket;
		if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, serverSocket, &_ev) == -1)
		{
			std::cerr << "Error: epoll_ctl failed for server socket (fd: " << serverSocket << "). Error: " << strerror(errno) << std::endl;
			return -1;
		}
		std::cout << "[Serversocket added] - fd: " << it->getSocket() << ", port: " << it->getPort() << std::endl;
	}
	return 1;
}

ServerManager::ServerManager()
{
	_epollFd = -1;
	_eventFd = -1;
}

ServerManager::ServerManager(const ServerManager &src)
{
	*this = src;
}

ServerManager::~ServerManager()
{
	// close(_epollFd);
	// close(_eventFd);
}
