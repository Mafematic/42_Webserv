#include "ServerManager.hpp"
#include "RequestRouter.hpp"
#include "Config_Parser.hpp"

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
			if (it->get_listen().port == tmp_it->getPort() && it->get_listen().ip == tmp_it->getIp())
			{
				tmp_it->_servers.push_back(*it);
				duplicate_flag = true;
				std::cout << YELLOW << "[Serverduplicat detected] added to existing socket: " << tmp_it->getSocket() << RESET << std::endl;
				break ;
			}
		}
		if (duplicate_flag == false)
		{
			Serverhandler server(it->get_listen().port, it->get_listen().ip);
			server._servers.push_back(*it);
			serverhandler.push_back(server);
		}
	}
}

void ServerManager::sendClientResponse(int clientSocket, const std::string &response, bool keepAlive)
{
	ssize_t bytesSent = send(clientSocket, response.c_str(), response.length(), 0); // 0 = no flags
	if (bytesSent < 0)
	{
		perror("Failed to send response");
	}
	if (!keepAlive)
	{
		close(clientSocket);
	}
}

std::string ServerManager::readRequest(int clientSocket)
{
	char buffer[BUFFER_SIZE];
	int bytes_read = read(clientSocket, buffer, sizeof(buffer) - 1);
	if (bytes_read <= 0)
	{
		perror("Failed to read request");
		close(clientSocket);
		return "";
	}
	buffer[bytes_read] = '\0';
	return std::string(buffer);
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

std::string ServerManager::readRequestBody(int clientSocket, std::string &buffer, int contentLength)
{
	size_t headerEnd = buffer.find("\r\n\r\n") + 4; // End of headers
	int bodyReadSoFar = buffer.size() - headerEnd;  // Already read part of the body

	while (bodyReadSoFar < contentLength)
	{
		char temp_buffer[BUFFER_SIZE];
		int bytesRead = read(clientSocket, temp_buffer, sizeof(temp_buffer) - 1);

		if (bytesRead > 0)
		{
			temp_buffer[bytesRead] = '\0';
			buffer += temp_buffer;
			bodyReadSoFar += bytesRead;
		}
		else
		{
			break; // Connection closed or error
		}
	}

	return buffer;
}


void ServerManager::handleClient(int clientSocket)
{
	std::string buffer = readRequest(clientSocket);
	if (buffer.empty())
	{
		return;
	}
	int contentLength = getContentLength(buffer);
	if (contentLength > 0)
	{
		buffer = readRequestBody(clientSocket, buffer, contentLength);
	}

	std::cout << "===== RECEIVED REQUEST =====" << std::endl;
	std::cout << buffer << std::endl;
	Request req(buffer); // Parse the raw request

	std::string response = RequestRouter::route(req);

	bool keepAlive = req.getHeader("Connection") == "keep-alive";
	sendClientResponse(clientSocket, response, keepAlive);
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
						acceptNewConnection(*it);
						isServerSocket = true;
						break;
					}
				}

				if (isServerSocket == false)
				{
					if (_clients.find(_eventFd) != _clients.end())
					{
						_clients[_eventFd].updateLastActivity();
						handleClient(_eventFd);
					}
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
