#include "ServerManager.hpp"

void ServerManager::setup(std::string config_path)
{
	int	duplicate_flag;

	duplicate_flag = false;
	std::vector<Server> _server_config;
	Config_Parser parser(config_path);
	_server_config = parser.parse_config();

	Logger::log(INFO, "Configuration file parsed", "", "");
	for (std::vector<Server>::iterator it = _server_config.begin(); it != _server_config.end(); ++it)
	{
		for (std::vector<Serverhandler>::iterator tmp_it = serverhandler.begin(); tmp_it != serverhandler.end(); ++tmp_it)
		{
			if (it->get_port() == tmp_it->getPort()
				&& it->get_ip() == tmp_it->getIp())
			{
				tmp_it->addServer(*it);
				duplicate_flag = true;
				Logger::log(WARNING, "[Serverduplicat detected]	added to existing socket: " + util::to_string(tmp_it->getSocket()), "", "");
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

void	ServerManager::handleClientRequest(Client &client, std::vector<Server> servers)
{
	int		status;
	Route	currentRoute;

	status = client.readRequest(_eventFd);
	if (status == READ_ERROR)
		return closeConnection(client, "[Failed to read request]");
	else if (status == CLIENT_DISCONNECTED)
		return closeConnection(client, "[DISCONNECT]");
	else if (status == READ_NOT_COMPLETE)
		return ;

	client.setRequest();
	client.setServer(servers);
	client.setRoute(client.getServer());
	currentRoute = client.getRoute();
	Logger::log(TRACE, "Request read", client.getRequest().getMethod(), client.getPrintName());

	if (currentRoute.get_location() == "/cgi-bin/")
	{
		std::cout << RED << "CGI Request" << RESET << std::endl;
		acceptNewCGIConnection(client.getFd());
		client.updateLastActivity();
		client.clearRequest();
		client.setCGI(true);
		std::cout << RED << "CGI initialized and running..." << RESET << std::endl;
		return ;
	}

	client.generateResponse();
	client.updateLastActivity();
	Logger::log(DEBUG, "Request proccesed - ready to send response", "", client.getPrintName());
}

void ServerManager::handleClientResponse(Client &client)
{
	int	close;

	if (client.getCGI())
	{
		if (!client.getCGIfinished())
			return ;
		Logger::log(DEBUG, "CGI process finished and response ready", "", client.getPrintName());
	}

	close = 0;
	if (client.getResponse().empty())
		return ;
	if (client.getResponse().find("Connection: close") != std::string::npos)
		close = 1;
	int status = client.sendResponse();
	if (status == SEND_ERROR)
		return (closeConnection(client, "[Failed to send response]"));
	else if (status == SEND_NOT_COMPLETE)
		return ;
	Logger::log(TRACE, "Response send", "", client.getPrintName());
	if (close == 1)
		return (closeConnection(client, "[Header -> Connection: close]"));
	client.clearRequest();
	client.clearResponse();
}

void	ServerManager::handleCGI()
{
	Client &client = _clients[cgi_controllers[_eventFd].corresponding_client.getFd()];

	int status = client.readRequest(_eventFd);
	if (status == READ_ERROR)
	{
		Logger::log(ERROR, "Reading from cgi-pipe : clean up ", "", client.getPrintName());
		client.setCGIfinished(false);
		close(_eventFd);
		epoll_ctl(_epollFd, EPOLL_CTL_DEL, _eventFd, NULL);
		cgi_controllers.erase(_eventFd);
	}
	else if (status == READ_NOT_COMPLETE)
		return ;
	else
	{
		client.setResponse(client.getRequestStr());
		Logger::log(DEBUG, "CGI read from pipe : ready to send response!", "", client.getPrintName());
	}
}

void	ServerManager::cleanUpCGI(Client &client, const std::string &logMessage)
{
	Logger::log(DEBUG, logMessage, "", client.getPrintName());
	client.setCGIfinished(false);
	client.setCGI(false);
	epoll_ctl(_epollFd, EPOLL_CTL_DEL, _eventFd, NULL);
	close(_eventFd);
	cgi_controllers.erase(_eventFd);
	client.clearRequest();
}



//-------------------------------------------------SERVER LOOP-------------------------------------------------//

void ServerManager::run()
{
	struct epoll_event	events[MAX_EVENTS];
	int					eventCount;

	if (createEpoll() == -1 || epollAddSockets() == -1)
	{
		return ;
	}
	Logger::log(INFO, "Servermanager is running", "", "");
	while (g_running)
	{
		eventCount = epoll_wait(_epollFd, events, MAX_EVENTS, EPOLL_TIMEOUT);
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
						break ;
					}
				}
				if (_clients.find(_eventFd) != _clients.end())
					handleClientRequest(_clients[_eventFd], _clients[_eventFd].getServerhandler().getServers());
				else if (cgi_controllers.find(_eventFd) != cgi_controllers.end())
					handleCGI();
			}
			else if ((events[i].events & EPOLLOUT) && _clients.find(_eventFd) != _clients.end())
				handleClientResponse(_clients[_eventFd]);
		}
		checkTimeout();
		checkForCGI();
	}
	std::cout << RED << "Server shutting down..." << RESET << std::endl;
}

void	ServerManager::checkForCGI()
{
	for (std::map<int, Cgi_Controller>::iterator it = cgi_controllers.begin(); it != cgi_controllers.end(); )
	{
		int status = it->second.check_cgi();
		Client &client = _clients[it->second.corresponding_client.getFd()];


		if (_clients.find(client.getFd()) == _clients.end())
		{
			std::cout << YELLOW << "Client disconnected before CGI completion, cleaning up" << RESET << std::endl;
			close(it->first);
			epoll_ctl(_epollFd, EPOLL_CTL_DEL, it->first, NULL);
			cgi_controllers.erase(it++);
			continue;
		}

		if (status == CGI_EXITED_NORMAL)
		{
			std::cout << RED << "CGI exited normally for client " << client.getFd() << RESET << std::endl;
			client.setCGIfinished(true);
			cgi_controllers.erase(it->first);
			return ;
		}
		else if (status == CGI_KILLED_TIMEOUT)
		{
			Logger::log(WARNING, "CGI process killed due to timeout", "", client.getPrintName());
			client.setResponse("HTTP/1.1 504 Gateway Timeout\r\nContent-Length: 0\r\n\r\n");
			cleanUpCGI(client, "[CGI Timeout]");
			return;
		}
		else if (status == CGI_EXITED_ERROR)
		{
			Logger::log(ERROR, "CGI process exited with an error", "", client.getPrintName());
			client.setResponse("HTTP/1.1 500 Internal Server Error\r\nContent-Length: 0\r\n\r\n");
			cleanUpCGI(client, "[CGI Error]");
			return;
		}

		++it;
	}
}


void ServerManager::checkTimeout()
{
	Client	tmp;

	for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end();)
	{
		if (time(NULL) - it->second.getLastActivity() > CLIENT_TIMEOUT)
		{
			tmp = it->second;
			it++;
			closeConnection(tmp, "[Client timeout]");
		}
		else
			++it;
	}
}

void ServerManager::closeConnection(Client &client, std::string reason)
{
	//std::cout << YELLOW << reason << " Client disconnected : ClientFd " << client.getFd() << RESET << std::endl;
	Logger::log(DEBUG, reason + " Client disconnected", "", client.getPrintName());
	client.clearRequest();
	client.clearResponse();
	client.setCGIfinished(false);
	close(client.getFd());
	epoll_ctl(_epollFd, EPOLL_CTL_DEL, client.getFd(), NULL);
	_clients.erase(client.getFd());
}

void	ServerManager::acceptNewCGIConnection(int clientFD)
{
	Client client = _clients[clientFD];
	Cgi_Controller	cgi(client);
	try
	{
		cgi.start_cgi();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		//_clients[clientFD].setResponse("");
		//Errorcode <---------------
	}

	int cgi_fd = cgi.pipe_receive_cgi_answer[0];

	setNonBlocking(cgi_fd);
	_ev.events = EPOLLIN;
	_ev.data.fd = cgi_fd;
	if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, cgi_fd, &_ev) == -1)
	{
		std::cerr << "Error: epoll_ctl failed for client socket" << std::endl;
		close(cgi_fd);
		return ;
	}
	cgi_controllers[cgi_fd] = cgi;

}

void ServerManager::acceptNewConnection(Serverhandler handler)
{
	struct sockaddr_in	client_addr;
	socklen_t			client_len;
	int					clientSocket;

	client_len = sizeof(client_addr);
	clientSocket = accept(_eventFd, (struct sockaddr *)&client_addr, &client_len);
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
		Client new_client(clientSocket, handler, client_addr);
		_clients[clientSocket] = new_client;
	}
	else
		std::cerr << "Accept failed." << std::endl;
}

void ServerManager::setNonBlocking(int socketFd)
{
	int	flags;

	// Get the current flags on the socket
	flags = fcntl(socketFd, F_GETFL, 0);
	if (flags == -1)
	{
		std::cerr << "Error: Failed to get flags for socket. Error: " << strerror(errno) << std::endl;
		return ;
	}
	// Set the socket to non-blocking by adding O_NONBLOCK to its flags
	if (fcntl(socketFd, F_SETFL, flags | O_NONBLOCK) == -1)
	{
		std::cerr << "Error: Failed to set socket to non-blocking mode. Error: " << strerror(errno) << std::endl;
		return ;
	}
}

//-------------------------------------------------EPOLL-------------------------------------------------//

int ServerManager::createEpoll()
{
	_epollFd = epoll_create1(0);
	if (_epollFd == -1)
	{
		std::cerr << "Error: epoll_create1 failed" << std::endl;
		return (-1);
	}
	return (1);
}

int ServerManager::epollAddSockets()
{
	int	serverSocket;

	for (std::vector<Serverhandler>::iterator it = serverhandler.begin(); it != serverhandler.end(); ++it)
	{
		serverSocket = it->getSocket();
		if (serverSocket < 0)
		{
			std::cerr << "Error: Invalid server socket (fd: " << serverSocket << ")" << std::endl;
			return (-1);
		}
		_ev.events = EPOLLIN;
		_ev.data.fd = serverSocket;
		if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, serverSocket, &_ev) == -1)
		{
			std::cerr << "Error: epoll_ctl failed for server socket (fd: " << serverSocket << "). Error: " << strerror(errno) << std::endl;
			return (-1);
		}
		Logger::log(INFO, "Socket added to epoll", "", "");
	}
	// std::cout << GREEN << "[Info]	All server sockets added to epoll" << RESET << std::endl;
	return (1);
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
