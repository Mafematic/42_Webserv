#include "Client.hpp"

Client::Client(){}

Client::Client(int clientFd, Serverhandler handler) : clientFd(clientFd), lastActivity(time(NULL)), handler(handler)
{
	_readDone = true;
	_isChunked = false;
	_currentChunkSize = 0;
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
	_currentChunkSize = src._currentChunkSize;
	server = src.server;
	response = src.response;
	_contentLength = src._contentLength;
	_bytesReceived = src._bytesReceived;
	_readDone = src._readDone;
	return *this;
}

Client::~Client(){}

int	Client::readRequest()
{
	char buffer[BUFFER_SIZE];

	ssize_t bytesRead = read(clientFd, buffer, BUFFER_SIZE);
	if (bytesRead < 0)
		return READ_ERROR;
	else if (bytesRead == 0)
		return CLIENT_DISCONNECTED;

	_bytesReceived += bytesRead;
	_buffer.append(buffer, bytesRead);

	if (requestComplete())
		return READ_COMPLETE;

	_readDone = false;
	return READ_NOT_COMPLETE;
}

// int	Client::processChunkedData()
// {
// 	std::cout << "Processing chunked data" << std::endl;
// 	while(1)
// 	{
// 		std::cout << RED << "buffer: " << _buffer << RESET << std::endl;

// 		if (_currentChunkSize == 0)
// 		{
// 			size_t pos = _buffer.find("\r\n");
// 			if (pos == std::string::npos)
// 				return READ_NOT_COMPLETE;
// 			std::string chunkSizeStr = _buffer.substr(0, pos);
// 			std::istringstream(chunkSizeStr) >> std::hex >> _currentChunkSize;
// 			_buffer.erase(0, pos + 2);
// 			if (_currentChunkSize == 0)
// 			{
// 				//std::cout << RED << "Chunked data complete" << RESET << std::endl;
// 				_isChunked = false;
// 				_readDone = true;
// 				return READ_COMPLETE;
// 			}
// 		}
// 		else
// 		{
// 			if (_buffer.size() < _currentChunkSize + 2)
// 				return READ_NOT_COMPLETE;
// 			_completeRequest.append(_buffer, 0, _currentChunkSize);
// 			_buffer.erase(0, _currentChunkSize + 2);
// 			_currentChunkSize = 0;
// 		}
// 	}
// }

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
	_readDone = true;
	_isChunked = false;
	_currentChunkSize = 0;
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

std::string	Client::getCompleteRequest()
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
	return _readDone;
}
