#include "ServerManager.hpp"

void ServerManager::setup(std::string config_path)
{
	int	duplicate_flag = false;
	std::vector<Server> _server_config;
	Config_Parser parser(config_path);

	_server_config = parser.parse_config();
	std::cout << GREEN << "[Info]	Configuration file parsed" << RESET << std::endl;
	for (std::vector<Server>::iterator it = _server_config.begin(); it != _server_config.end(); ++it)
	{
		for(std::vector<Serverhandler>::iterator tmp_it = serverhandler.begin(); tmp_it != serverhandler.end(); ++tmp_it)
		{
			if (it->get_port() == tmp_it->getPort() && it->get_ip() == tmp_it->getIp())
			{
				tmp_it->addServer(*it);
				duplicate_flag = true;
				std::cout << YELLOW << "[Info][Serverduplicat detected]	added to existing socket: " << tmp_it->getSocket() << RESET << std::endl;
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

//-------------------------------------------------HANDLE CLIENT REQUEST & RESPONSE-------------------------------------------------//

void ServerManager::handleClientRequest(Client &client, std::vector<Server> servers)
{
	int status = client.readRequest();
	if (status == READ_ERROR)
	{
		closeConnection(client, "[Failed to read request]");
		return;
	}
	else if (status == CLIENT_DISCONNECTED)
	{
		closeConnection(client, "[DISCONNECT]");
		return;
	}
	else if (status == READ_NOT_COMPLETE)
		return;
	std::cout << LIGTH BLUE << "++++ [Request read] : ClientFd " << client.getFd() << RESET << std::endl;
	std::cout << PURPLE << client.getCompleteRequest() << RESET << std::endl;

	client.setRequest();
	client.setServer(servers);
	client.setResponse();

	client.updateLastActivity();
	std::cout << LIGTH BLUE << "+++++ [Request processed] : ClientFd " << client.getFd() << RESET << std::endl;
}

void	ServerManager::handleClientResponse(Client &client)
{
	int close = 0;
	if (client.getResponse().empty())
		return;
	if (client.getResponse().find("Connection: close") != std::string::npos)
		close = 1;

	std::cout << PURPLE << client.getResponse() << std::endl;
	int	status = client.sendResponse();
	if (status == SEND_ERROR)
		return	closeConnection(client, "[Failed to send response]");
	else if (status == SEND_NOT_COMPLETE)
		return;

	std::cout << LIGTH BLUE << "++++ [Response sent] : ClientFd " << client.getFd() << RESET << std::endl;

	if (close == 1)
		return closeConnection(client, "[Header -> Connection: close]");
	client.clearRequest();
	client.clearResponse();
}

//-------------------------------------------------SERVER LOOP-------------------------------------------------//

void ServerManager::run()
{
	struct epoll_event events[MAX_EVENTS];

	if (createEpoll() == -1 || epollAddSockets() == -1) {
		return;
	}

	std::cout << GREEN << "[Info]	Servermanager is running" << RESET << std::endl;

	while (g_running)
	{
		int eventCount = epoll_wait(_epollFd, events, MAX_EVENTS, EPOLL_TIMEOUT);
		for (int i = 0; i < eventCount; i++)
		{
			_eventFd = events[i].data.fd;
			if (events[i].events & EPOLLIN)
			{
				for (std::vector<Serverhandler>::iterator it = serverhandler.begin(); it != serverhandler.end(); ++it)
				{
					if (_eventFd == it->getSocket())
					{
						acceptNewConnection(*it);
						break;
					}
				}
				if (_clients.find(_eventFd) != _clients.end())
					handleClientRequest(_clients[_eventFd], _clients[_eventFd].getServerhandler().getServers());
			}
			else if (events[i].events & EPOLLOUT)
			{
				if (_clients.find(_eventFd) != _clients.end())
					handleClientResponse(_clients[_eventFd]);
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
	client.clearRequest();
	client.clearResponse();
	close(client.getFd());
	epoll_ctl(_epollFd, EPOLL_CTL_DEL, client.getFd(), NULL);
	_clients.erase(client.getFd());
}

void	ServerManager::acceptNewConnection(Serverhandler handler)
{
	int clientSocket = accept(_eventFd, NULL, NULL);
	if (clientSocket >= 0)
	{
		setNonBlocking(clientSocket);

		_ev.events = EPOLLIN | EPOLLOUT;
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

//-------------------------------------------------EPOLL-------------------------------------------------//

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
		std::cout << GREEN << "[Info]	server " << RESET << it->getSocket() << GREEN << " is ready " << std::endl;
	}
	//std::cout << GREEN << "[Info]	All server sockets added to epoll" << RESET << std::endl;
	return 1;
}

//-------------------------------------------------CONSTRUCTORS & DESTRUCTORS-------------------------------------------------//

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
