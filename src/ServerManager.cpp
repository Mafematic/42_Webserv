#include "ServerManager.hpp"

void ServerManager::setup(std::string config_path)
{
	int	duplicate_flag = false;
	std::vector<Server> _server_config;
	Config_Parser parser(config_path);

	_server_config = parser.parse_config();
	for (std::vector<Server>::iterator it = _server_config.begin(); it != _server_config.end(); ++it)
	{
		//print server_name
		// std::vector<std::string> name = it->get_server_name();
		// for (std::vector<std::string>::iterator it2 = name.begin(); it2 != name.end(); ++it2)
		// 	std::cout << GREEN << "Servername: " << *it2 << RESET << std::endl;

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

void ServerManager::sendClientResponse(int clientSocket, std::string &response)
{
	ssize_t bytesSent = send(clientSocket, response.c_str(), response.length(), 0); // 0 = no flags
	if (bytesSent < 0)
	{
		perror("Failed to send response");
	}
}

void ServerManager::readRequest(Client &client)
{
    std::vector<char> tempBuffer(BUFFER_SIZE);

    while (true)
	{
        int bytesRead = read(client.getFd(), tempBuffer.data(), tempBuffer.size());
        if (bytesRead <= 0)
		{
            if (bytesRead == 0) 
				break; 
            perror("Failed to read request");
            close(client.getFd());
            client.clearBuffer();
            return;
        }

        client.appendToBuffer(std::string(tempBuffer.data(), bytesRead));

        if (client.getBuffer().find("\r\n\r\n") != std::string::npos)
		{
            break;
        }
    }
}

int ServerManager::getContentLength(const std::string& request)
{
	size_t content_length_pos = request.find("Content-Length: ");
	if (content_length_pos != std::string::npos)
	{
		size_t start = content_length_pos + 16; // "Content-Length: " length
		size_t end = request.find("\r\n", start);
		std::string content_length_str = request.substr(start, end - start);
		int content_length;
		std::istringstream(content_length_str) >> content_length;
		return content_length;
	}
	return 0;
}


Server ServerManager::getServer(std::vector<Server> servers, Request req)
{
	Server server;
	if (servers.size() == 1)
	{
		std::cout << RED << "[DEBUG] Only one server for client" << RESET << std::endl;
		server = servers[0];
	}
	else
	{
		std::cout << RED << "[DEBUG] Multiple servers for client" << RESET << std::endl;
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
	readRequest(client);
    if (client.getBuffer().empty())
	{
        return;
    }
    std::cout << "++++ Buffer: " << client.getBuffer() << std::endl;

    Request req(client.getBuffer());

	//gets the server that the client is connected to, if there are multiple servers,
	//it will get the server based on the host header
	Server server = getServer(servers, req);

	std::string response = RequestRouter::route(req, server);
	//std::cout << "++++ Response" << response << std::endl;
	sendClientResponse(client.getFd(), response);
	client.clearBuffer();
}

void ServerManager::run()
{
	struct epoll_event events[MAX_EVENTS];

	if (createEpoll() == -1 || epollAddSockets() == -1) {
		return;
	}

	//print all serversockets with servers connected to them
	// for (std::vector<Serverhandler>::iterator it = serverhandler.begin(); it != serverhandler.end(); ++it)
	// {
	// 	std::cout << YELLOW << "Servers on ServerFd: "<< it->getSocket() << RESET << std::endl;
	// 	for (std::vector<Server>::iterator it2 = it->_servers.begin(); it2 != it->_servers.end(); ++it2)
	// 		std::cout << *it2 << std::endl;
	// }

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
					if (_eventFd == it->getSocket()) {
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
						std::cout << GREEN << "[New Request] : ClientFd " << _clients.find(_eventFd)->second.getFd() << RESET << std::endl;
						handleClient(_clients[_eventFd], _clients[_eventFd].getServerhandler().getServers());
					}
				}
			}
		}
	// process_request();
		checkTimeout();
	}
	std::cout << RED << "Server shutting down..." << RESET << std::endl;
}

	// process_request();
	// check header received
	// parse header
	// assign server
	// do routing
	// 

void	ServerManager::checkTimeout()
{
	for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); )
	{
		if (time(NULL) - it->second.getLastActivity() > CLIENT_TIMEOUT)
		{
			std::cout << YELLOW << "[Timeout] Client disconnected : ClientFd " << it->first << RESET << std::endl;
			epoll_ctl(_epollFd, EPOLL_CTL_DEL, it->first, NULL);
			close(it->first);
			_clients.erase(it++);
		}
		else
			++it;
	}
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
