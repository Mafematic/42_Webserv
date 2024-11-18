#include "ServerManager.hpp"
#include "RequestRouter.hpp"
#include "Config_Parser.hpp"

void ServerManager::setup(std::string config_path)
{
	std::vector<Server> _server_config;
	Config_Parser parser(config_path);

	_server_config = parser.parse_config();
	for (std::vector<Server>::iterator it = _server_config.begin(); it != _server_config.end(); ++it)
	{
		Serverhandler server(it->get_listen().port, it->get_listen().ip);
		serverhandler.push_back(server);
	}
}

void ServerManager::handleClient(int clientSocket)
{
	char buffer[BUFFER_SIZE];

	int bytes_read = read(clientSocket, buffer, sizeof(buffer) - 1);
	if (bytes_read <= 0)
	{
		perror("Failed to read request");
		close(clientSocket);
		return;
	}
	buffer[bytes_read] = '\0';

	// Extract Content-Length from headers to read full body
	std::string request(buffer);
	size_t content_length_pos = request.find("Content-Length: ");
	if (content_length_pos != std::string::npos)
	{
		size_t start = content_length_pos + 16; // "Content-Length: " length
		size_t end = request.find("\r\n", start);
		std::string content_length_str = request.substr(start, end - start);
		int content_length;
		std::istringstream(content_length_str) >> content_length;

		// Ensure full body is read if Content-Length > bytes_read
		while (request.size() < content_length + request.find("\r\n\r\n") + 4)
		{
			char temp_buffer[BUFFER_SIZE];
			int extra_bytes = read(clientSocket, temp_buffer, sizeof(temp_buffer) - 1);
			if (extra_bytes > 0)
			{
				temp_buffer[extra_bytes] = '\0';
				request += temp_buffer;
			}
			else
			{
				break;
			}
		}
	}

	// Copy the full request back to buffer
	strncpy(buffer, request.c_str(), sizeof(buffer) - 1);
	buffer[sizeof(buffer) - 1] = '\0';

	std::cout << "===== RECEIVED REQUEST =====" << std::endl;
	std::cout << buffer << std::endl;

	Request req(buffer); // Parse the raw request

	std::string response = RequestRouter::route(req);

	bool keepAlive = req.getHeader("Connection") == "keep-alive";

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

void ServerManager::run()
{
	struct epoll_event events[MAX_EVENTS];

	if (createEpoll() == -1 || epollAddSockets() == -1) {
		return;
	}

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
						isServerSocket = true;
						break;
					}
				}

				if (isServerSocket)
					acceptNewConnection();
				else
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

void	ServerManager::acceptNewConnection()
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
		Client new_client(clientSocket);
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
