#include "Request.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>

Request::Request()
{
}

Request::Request(const std::string &raw_request) : _valid(true),
	_raw_request(raw_request), _body(""), _keepAlive(true)
{
	_parseRequest();
	_validateRequest();
}

Request::Request(const Request &src)
{
	*this = src;
}

Request &Request::operator=(const Request &src)
{
	if (this == &src)
		return (*this);
	_method = src._method;
	_path = src._path;
	_version = src._version;
	_headers = src._headers;
	_valid = src._valid;
	_error_message = src._error_message;
	_raw_request = src._raw_request;
	_body = src._body;
	_keepAlive = src._keepAlive;
	return (*this);
}

Request::~Request()
{
}

void Request::_parseRequest()
{
	size_t	delimiter;

	std::istringstream request_stream(this->_raw_request);
	std::string line;
	// Parse the request line
	if (std::getline(request_stream, line))
	{
		std::istringstream line_stream(line);
		line_stream >> _method >> _path >> _version;
	}
	// Parse headers
	while (std::getline(request_stream, line) && line != "\r")
	{
		delimiter = line.find(":");
		if (delimiter != std::string::npos)
		{
			std::string key = line.substr(0, delimiter);
			std::string value = line.substr(delimiter + 1);
			value.erase(0, value.find_first_not_of(" \t"));
			value.erase(value.find_last_not_of(" \t\r") + 1);
			_headers[key] = value;
		}
	}
	// Read the rest as body
	if (request_stream.peek() != std::istringstream::traits_type::eof())
	{
		_body = std::string(std::istreambuf_iterator<char>(request_stream),
				std::istreambuf_iterator<char>());
	}
}

void Request::_validateRequest()
{
	if (_method.empty() || _path.empty() || _version.empty())
	{
		_valid = false;
		_error_message = "Malformed request line";
		return ;
	}
	if (_version != "HTTP/1.1")
	{
		_valid = false;
		_error_message = "Unsupported HTTP version";
		return ;
	}
	if (_method != "GET" && _method != "POST" && _method != "DELETE")
	{
		_valid = false;
		_error_message = "Unsupported HTTP method";
		return ;
	}
	// Use getHeader to check for the Host header
	std::string host = getHeader("Host");
	if (host.empty())
	{
		_valid = false;
		_error_message = "Missing or empty Host header";
		return ;
	}
	std::string connectionHeader = getHeader("Connection");
	std::transform(connectionHeader.begin(), connectionHeader.end(),
		connectionHeader.begin(), ::tolower);
	if (_version == "HTTP/1.1" && connectionHeader != "close")
	{
		_keepAlive = true;
	}
	else if (_version == "HTTP/1.0" && connectionHeader == "keep-alive")
	{
		_keepAlive = true;
	}
	else
	{
		_keepAlive = false;
	}
}

bool Request::isValid() const
{
	return (_valid);
}

std::string Request::getMethod() const
{
	return (_method);
}

std::string Request::getPath() const
{
	return (_path);
}

void Request::setPath(const std::string &newPath)
{
    _path = newPath;
}

std::string Request::getVersion() const
{
	return (_version);
}

std::string Request::getErrorMessage() const
{
	return (_error_message);
}

std::string Request::getRawRequest() const
{
	return (_raw_request);
}

std::string Request::getHeader(const std::string &key) const
{
	std::map<std::string, std::string>::const_iterator it = _headers.find(key);
	if (it != _headers.end())
	{
		return (it->second);
	}
	else
	{
		return ("");
	}
}

std::map<std::string, std::string> Request::getHeaderMap() const
{
	return (this->_headers);
}

std::string Request::getBody() const
{
	return (_body);
}

void Request::setKeepAlive(bool keepAlive)
{
	_keepAlive = keepAlive;
}

bool Request::getKeepAlive() const
{
	return (_keepAlive);
}

void Request::print_header() const
{
	std::cout << "Request Headers" << std::endl;
	for (std::map<std::string,
		std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it)
	{
		std::cout << it->first << ": " << it->second << std::endl;
	}
	std::cout << std::endl << std::endl << std::endl;
}