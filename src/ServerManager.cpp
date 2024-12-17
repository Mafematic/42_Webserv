#include "ServerManager.hpp"

void ServerManager::setup(std::string config_path)
{
	int	duplicate_flag;

	duplicate_flag = false;
	std::vector<Server> _server_config;
	Config_Parser parser(config_path);
	_server_config = parser.parse_config();

	Logger::log(INFO, "Configuration file parsed", "", "", -1);
	for (std::vector<Server>::iterator it = _server_config.begin(); it != _server_config.end(); ++it)
	{
		for (std::vector<Serverhandler>::iterator tmp_it = serverhandler.begin(); tmp_it != serverhandler.end(); ++tmp_it)
		{
			if (it->get_port() == tmp_it->getPort()
				&& it->get_ip() == tmp_it->getIp())
			{
				tmp_it->addServer(*it);
				duplicate_flag = true;
				Logger::log(WARNING, "[Serverduplicat detected]	added to existing socket: " + util::to_string(tmp_it->getSocket()), "", "", -1);
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

//-------------------------------------------------HANDLE CLIENT REQUEST, CGI & RESPONSE-------------------------------------------------//

void	ServerManager::handleClientRequest(Client &client, std::vector<Server> servers)
{
	int		status;
	Route	currentRoute;

	status = client.readRequest(_eventFd);
	if (status == READ_ERROR)
		return closeConnection(client, "[READ ERROR]");
	else if (status == CLIENT_DISCONNECTED)
		return closeConnection(client, "[DISCONNECT]");
	else if (status == READ_NOT_COMPLETE)
		return ;

	client.setRequest();
	client.setServer(servers);
	client.setRoute(client.getServer());
	currentRoute = client.getRoute();
	Logger::log(TRACE, "Request read", client.getRequest().getMethod() + " " + client.getRequest().getPath(), client.getPrintName(), client.getFd());

	client.generateResponse();
	if (currentRoute.get_location() == "/cgi-bin/" && RequestRouter::valid)
	{
		acceptNewCGIConnection(client.getFd());
		client.updateLastActivity();
		client.setCGI(true);
		client.clearRequest();
		Logger::log(DEBUG, "CGI Request : CGI initialized and running", "", client.getPrintName(), client.getFd());
		return ;
	}
	client.updateLastActivity();
	Logger::log(DEBUG, "Request proccesed - ready to send response", "", client.getPrintName(), client.getFd());
}

void ServerManager::handleClientResponse(Client &client)
{
	int	close;

	if (client.getCGI())
	{
		if (!client.getCGIfinished())
			return ;
		Logger::log(DEBUG, "CGI process finished and response ready", "", client.getPrintName(), client.getFd());
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
	Logger::log(TRACE, "Response send", "", client.getPrintName(), client.getFd());
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
		Logger::log(ERROR, "[CGI] Reading from cgi-pipe : clean up ", "", client.getPrintName(), client.getFd());
		client.setCGIfinished(false);
		close(_eventFd);
		epoll_ctl(_epollFd, EPOLL_CTL_DEL, _eventFd, NULL);
		cgi_controllers.erase(_eventFd);
		client.setResponse(getCustomError(client, 500));
	}
	else if (status == READ_NOT_COMPLETE)
		return ;
	else
	{
		client.setResponse(client.getRequestStr());
		Logger::log(DEBUG, "[CGI] read from pipe : waiting for child to finish", "", client.getPrintName(), client.getFd());
	}
}

void ServerManager::cleanUpCGI(Client &client, int fd)
{
	Logger::log(DEBUG, "Cleaning up CGI for client", "", client.getPrintName(), fd);

	client.setCGIfinished(false);
	client.setCGI(false);

	if (epoll_ctl(_epollFd, EPOLL_CTL_DEL, fd, NULL) == -1) {
		Logger::log(ERROR, "Failed to remove CGI FD from epoll", "", client.getPrintName(), fd);
	}
	close(fd);

	if (cgi_controllers.find(fd) != cgi_controllers.end())
	{
		cgi_controllers[fd].kill_child();
		cgi_controllers.erase(fd);
	}
	client.clearRequest();
	client.clearResponse();
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
	Logger::log(INFO, "Servermanager is running", "", "", -1);
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
		int clientFd = it->second.corresponding_client.getFd();

		if (_clients.find(clientFd) == _clients.end())
		{
			Logger::log(WARNING, "Client disconnected befor CGI completion : cleaning up", "", "", clientFd);
			epoll_ctl(_epollFd, EPOLL_CTL_DEL, it->first, NULL);
			close(it->first);
			it->second.kill_child();
			it->second.remove_cgi_tmp_infile();
			cgi_controllers.erase(it->first);
			//PROBLEM : IF CLIENT DISCONNECTS FIRST << SAME REQUEST OVER AND OVER ---------------------------<<<<<<<<<
			return;
		}

		Client &client = _clients[clientFd];
		int status = it->second.check_cgi();

		if (status == CGI_EXITED_NORMAL)
		{
			Logger::log(DEBUG, "[CGI] Child finished : ready to send response", "", client.getPrintName(), client.getFd());
			client.setCGIfinished(true);
			close(it->first);
			cgi_controllers.erase(it->first);
			return ;
		}
		else if (status == CGI_KILLED_TIMEOUT)
		{
			Logger::log(WARNING, "[CGI TIMEOUT] CGI process killed due to timeout", "", client.getPrintName(), client.getFd());
			cleanUpCGI(client, it->first);
			client.setResponse(getCustomError(client, 504));
			return;
		}
		else if (status == CGI_EXITED_ERROR)
		{
			Logger::log(ERROR, "[CGI ERROR] CGI process exited with an error", "", client.getPrintName(), client.getFd());
			cleanUpCGI(client, it->first);
			client.setResponse(getCustomError(client, 500));
			return;
		}
		else
			++it;
	}
}

std::string ServerManager::getCustomError(Client client, int error)
{
	std::string customError;
	Route route = client.getRoute();
	Server server = client.getServer();
	Request req = client.getRequest();


	customError = RequestRouter::getCustomErrorPage(server.get_final_root(route), route, error, server);
	return RequestRouter::_serveFile(customError, error, req);
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
	Logger::log(DEBUG, reason + " Client disconnected", "", client.getPrintName(), client.getFd());
	client.clearRequest();
	client.clearResponse();
	epoll_ctl(_epollFd, EPOLL_CTL_DEL, client.getFd(), NULL);
	close(client.getFd());
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
		_clients[clientFD].setResponse(getCustomError(client, 500));
		return ;
	}

	int cgi_fd = cgi.pipe_receive_cgi_answer[0];

	setNonBlocking(cgi_fd);
	_ev.events = EPOLLIN;
	_ev.data.fd = cgi_fd;
	if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, cgi_fd, &_ev) == -1)
	{
		Logger::log(ERROR, "Failed to add cgi-pipe to socket", "", client.getPrintName(), client.getFd());
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
			Logger::log(ERROR, "Fialed to add client to socket", "", "", -1);
			close(clientSocket);
			return ;
		}
		Client new_client(clientSocket, handler, client_addr);
		_clients[clientSocket] = new_client;
		Logger::log(INFO, "Accpeted new client-connection", "", new_client.getPrintName(), new_client.getFd());
	}
	else
		Logger::log(ERROR, "Failed to accept new client-connection", "", "", -1);
}

void ServerManager::setNonBlocking(int socketFd)
{
	int	flags;

	// Get the current flags on the socket
	flags = fcntl(socketFd, F_GETFL, 0);
	if (flags == -1)
		return Logger::log(ERROR, "Failed to set socket to non-blocking mode.", "", "", -1);
	// Set the socket to non-blocking by adding O_NONBLOCK to its flags
	if (fcntl(socketFd, F_SETFL, flags | O_NONBLOCK) == -1)
		return Logger::log(ERROR, "Failed to set socket to non-blocking mode.", "", "", -1);
}

//-------------------------------------------------EPOLL-------------------------------------------------//

int ServerManager::createEpoll()
{
	_epollFd = epoll_create1(0);
	if (_epollFd == -1)
	{
		Logger::log(ERROR, "Failed to create epoll.", "", "", -1);
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
		Logger::log(INFO, "Socket added to epoll", "", "", -1);
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
