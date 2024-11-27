#include "Client.hpp"

Client::Client(){}

Client::Client(int clientFd, Serverhandler handler) : clientFd(clientFd), lastActivity(time(NULL)), handler(handler)
{
	_done = true;
	_isChunked = false;
	_contentLength = 0;
	_bytesReceived = 0;
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
	server = src.server;
	response = src.response;
	_contentLength = src._contentLength;
	_bytesReceived = src._bytesReceived;
	_done = src._done;
	return *this;
}

Client::~Client(){}

int	Client::readRequest()
{
	char buffer[BUFFER_SIZE];

	size_t bytesRead = read(clientFd, buffer, BUFFER_SIZE);
	if (bytesRead < 0)
		return READ_ERROR;
	else if (bytesRead == 0)
		return CLIENT_DISCONNECTED;

	_bytesReceived += bytesRead;
	//std::cout << RED << "BytesRead: " << bytesRead << " BytesReceived: " << _bytesReceived << RESET << std::endl;
	_buffer.append(buffer, bytesRead);

	//std::cout << TURKIZ << "Buffer: " << _buffer << RESET << std::endl;
	if (requestComplete())
	{
		//std::cout << RED << "BytesReceived: " << _bytesReceived << " ContentLength: " << _contentLength << RESET << std::endl;
		return READ_COMPLETE;
	}
	_done = false;
	return READ_NOT_COMPLETE;
}

bool	Client::requestComplete()
{
	if (_buffer.find("\r\n\r\n") != std::string::npos)
	{
		getContentLength();
		if (_bytesReceived >= _contentLength + _buffer.find("\r\n\r\n") + 4)
			return true;
	}
	//std::cout << RED << "Request not complete! BytesReceived: " << _bytesReceived << RESET << std::endl;
	return false;
}


void	Client::getContentLength()
{
	size_t content_length_pos = _buffer.find("Content-Length: ");
	if (content_length_pos != std::string::npos)
	{
		size_t start = content_length_pos + 16; // "Content-Length: " length
		size_t end = _buffer.find("\r\n", start);
		std::string content_length_str = _buffer.substr(start, end - start);
		std::istringstream(content_length_str) >> _contentLength;
	}
}

void	Client::clearRequest()
{
	//std::cout << RED << "Clearing request" << RESET << std::endl;
	_done = true;
	_isChunked = false;
	_contentLength = 0;
	_bytesReceived = 0;
	_buffer.clear();
}

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

std::string	Client::getBuffer()
{
    return _buffer;
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

void Client::setServer(Server server)
{
	this->server = server;
}

Server Client::getServer()
{
	return server;
}

void Client::setResponse(std::string response)
{
	this->response = response;
}

std::string Client::getResponse()
{
	return response;
}

bool	Client::isDone()
{
	return _done;
}
