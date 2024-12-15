#include "Client.hpp"
#include "RequestRouter.hpp"


Client::Client() {}

Client::Client(int clientFd, Serverhandler handler, struct sockaddr_in client_addr) : clientFd(clientFd), lastActivity(time(NULL)), handler(handler)
{
	_responseSentBytes = 0;
	_responselength = 0;
	_isChunked = false;
	_currentChunkSize = 0;
	_contentLength = 0;
	_bytesReceived = 0;
	_cgi = false;
	_cgi_finished = false;

	char ipBuffer[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(client_addr.sin_addr), ipBuffer, INET_ADDRSTRLEN);
	_client_ip = ipBuffer;
	_client_port = ntohs(client_addr.sin_port);
}

Client::Client(const Client &src)
{
	*this = src;
}

Client	&Client::operator=(const Client &src)
{
	if (this == &src)
		return *this;
	lastActivity = src.lastActivity;
	clientFd = src.clientFd;
	handler = src.handler;
	_buffer = src._buffer;
	_isChunked = src._isChunked;
	_currentChunkSize = src._currentChunkSize;
	_chunkFullString = src._chunkFullString;
	server = src.server;
	response = src.response;
	_contentLength = src._contentLength;
	_bytesReceived = src._bytesReceived;
	_responseSentBytes = src._responseSentBytes;
	_responselength = src._responselength;
	req = src.req;
	route = src.route;
	_client_port = src._client_port;
	_client_ip = src._client_ip;
	_cgi = src._cgi;
	_cgi_finished = src._cgi_finished;
	return *this;
}

Client::~Client(){}

//------------------------------------READ REQUEST------------------------------------//

int	Client::readRequest(int fd)
{
	char buffer[BUFFER_SIZE] = {0};

	ssize_t bytesRead = read(fd, buffer, BUFFER_SIZE);
	if (bytesRead < 0)
		return READ_ERROR;
	else if (bytesRead == 0)
		return CLIENT_DISCONNECTED;

	_bytesReceived += bytesRead;
	_buffer.append(buffer, bytesRead);

	if (requestComplete())
		return READ_COMPLETE;
	if (_isChunked)
		return processChunkedData();
	return READ_NOT_COMPLETE;
}

bool	Client::requestComplete()
{
	if (_isChunked)
		return false;
	size_t headerEnd = _buffer.find("\r\n\r\n");
	if (headerEnd != std::string::npos)
	{
		if (_buffer.find("Transfer-Encoding: chunked") != std::string::npos)
		{
			_isChunked = true;
			_buffer.erase(0, headerEnd + 4);
			return false;
		}

		getContentLength();
		size_t	bodyStart = headerEnd + 4;
		size_t	bodyLength = _bytesReceived - bodyStart;

		if (bodyLength >= _contentLength)
			return true;
	}
	return false;
}


void	Client::getContentLength()
{
	size_t content_length_pos = _buffer.find("Content-Length: ");
	if (content_length_pos != std::string::npos)
	{
		size_t start = content_length_pos + 16;
		size_t end = _buffer.find("\r\n", start);
		std::string content_length_str = _buffer.substr(start, end - start);
		std::istringstream(content_length_str) >> _contentLength;
	}
}

void	Client::clearRequest()
{
	_isChunked = false;
	_currentChunkSize = 0;
	_chunkFullString.clear();
	_contentLength = 0;
	_bytesReceived = 0;
	_buffer.clear();
}

//------------------------------------PROCESS CHUNKED DATA------------------------------------//

int	Client::processChunkedData()
{
	while (1)
	{
		if (_currentChunkSize == 0)
		{
			size_t pos = _buffer.find("\r\n");
			if (pos == std::string::npos)
				return READ_NOT_COMPLETE;

			std::string chunk_size_str = _buffer.substr(0, pos);
			std::istringstream(chunk_size_str) >> std::hex >> _currentChunkSize;

			_buffer.erase(0, pos + 2);
			if (_currentChunkSize == 0)
			{
				if (_buffer.find("\r\n") != std::string::npos)
				{
					_buffer = _chunkFullString;
					return READ_COMPLETE;
				}
				else
					return READ_ERROR;
			}
		}

		if (_buffer.length() >= _currentChunkSize + 2)
		{
			_chunkFullString.append(_buffer, 0, _currentChunkSize);
			_buffer.erase(0, _currentChunkSize + 2);
			_currentChunkSize = 0;
		}
		else
			return READ_NOT_COMPLETE;
	}
	return 0;
}

//------------------------------------SEND RESPONSE------------------------------------//

int	Client::sendResponse()
{
	size_t to_send = _responselength - _responseSentBytes;
	if (to_send > BUFFER_SIZE)
		to_send = BUFFER_SIZE;
	ssize_t bytesSent = send(clientFd, response.c_str(), to_send, 0); // 0 = no flags
	if (bytesSent <= 0)
		return SEND_ERROR;

	_responseSentBytes += bytesSent;
	response.erase(0, bytesSent);

	if (_responseSentBytes >= _responselength)
		return SEND_COMPLETE;

	return SEND_NOT_COMPLETE;
}

void	Client::clearResponse()
{
	_responseSentBytes = 0;
	_responselength = 0;
	_cgi = false;
	_cgi_finished = false;
	response.clear();
}

//------------------------------------GETTERS & SETTERS------------------------------------//

time_t	Client::getLastActivity()
{
	return lastActivity;
}

Serverhandler Client::getServerhandler()
{
	return handler;
}

int	Client::getFd()
{
	return clientFd;
}

void	Client::updateLastActivity()
{
	lastActivity = time(NULL);
}

std::string	Client::getRequestStr()
{
	return _buffer;
}

void	Client::appendToRequestStr(std::string str, int bytes)
{
	_buffer.append(str, bytes);
}

void Client::clearBuffer()
{
	_buffer.clear();
}

void Client::setChunked(bool isChunked)
{
	_isChunked = isChunked;
}

bool Client::isChunked()
{
	return _isChunked;
}

void Client::setServer(std::vector<Server> servers)
{
	if (servers.size() == 1)
		this->server = servers[0];
	else
	{
		this->server = servers[0];
		for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); ++it)
		{
			std::vector<std::string> server_names = it->get_server_name();
			for (std::vector<std::string>::iterator it2 = server_names.begin(); it2 != server_names.end(); ++it2)
			{
				std::string host = req.getHeader("Host");
				if (!host.empty())
				{
					host = host.substr(0, host.find(":"));
					if (host == *it2)
						this->server = *it;
				}
			}
		}
	}
}

Server Client::getServer()
{
	return server;
}

void Client::generateResponse()
{
	this->response = RequestRouter::route(req, server);
	//std::cout << "Response " << this->response << std::endl;
	_responselength = response.length();
}

void	Client::setResponse(std::string set_response)
{
	response = set_response;
	_responselength = set_response.length();
}

std::string Client::getResponse()
{
	return response;
}

void	Client::setRequest()
{
	req = Request(_buffer);
	clearBuffer();
}

Request	Client::getRequest()
{
	return req;
}

int	Client::getPort()
{
	return _client_port;
}

std::string	Client::getIp()
{
	return _client_ip;
}

void Client::setRoute(const Server &server)
{
	route = RequestRouter::_getRoute(server, req);
}

Route Client::getRoute() const
{
    return route;
}

bool	Client::getCGIfinished()
{
	return _cgi_finished;
}

void	Client::setCGIfinished(bool status)
{
	this->_cgi_finished = status;
}

bool	Client::getCGI()
{
	return _cgi;
}

void	Client::setCGI(bool status)
{
	this->_cgi = status;
}

std::string	Client::getPrintName()
{
	struct sockaddr_in server_addr;
	socklen_t addr_len = sizeof(server_addr);

	if (getsockname(clientFd, (struct sockaddr *)&server_addr, &addr_len) < 0)
	{
		perror("getsockname failed");
		close(clientFd);
	}
	char server_ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(server_addr.sin_addr), server_ip, INET_ADDRSTRLEN);
	int server_port = ntohs(server_addr.sin_port);

	std::ostringstream ss;
	ss << "	" << server_ip << ":" << server_port;
	return ss.str();
}
