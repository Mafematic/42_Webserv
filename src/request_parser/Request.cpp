#include "Request.hpp"
#include <sstream>
#include <iostream>


Request::Request(const std::string &raw_request)
    : _valid(true), _raw_request(raw_request)
{
    _parseRequest(raw_request);
    _validateRequest();
}

void Request::_parseRequest(const std::string &raw_request_param)
{
    std::istringstream request_stream(raw_request_param);
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
        size_t delimiter = line.find(":");
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
        _body = std::string(std::istreambuf_iterator<char>(request_stream), std::istreambuf_iterator<char>());
    }
}


void Request::_validateRequest()
{
    if (_method.empty() || _path.empty() || _version.empty())
    {
        _valid = false;
        _error_message = "Malformed request line";
        return;
    }

    if (_version != "HTTP/1.1")
    {
        _valid = false;
        _error_message = "Unsupported HTTP version";
        return;
    }

    if (_method != "GET" && _method != "POST" && _method != "DELETE")
    {
        _valid = false;
        _error_message = "Unsupported HTTP method";
        return;
    }

    // Use getHeader to check for the Host header
    std::string host = getHeader("Host");
    if (host.empty())
    {
        _valid = false;
        _error_message = "Missing or empty Host header";
        return;
    }
}


bool Request::isValid() const
{
    return _valid;
}

std::string Request::getMethod() const
{
    return _method;
}

std::string Request::getPath() const
{
    return _path;
}

std::string Request::getVersion() const
{
    return _version;
}

std::string Request::getErrorMessage() const
{
    return _error_message;
}

std::string Request::getRawRequest() const
{
    return _raw_request;
}

std::string Request::getHeader(const std::string &key) const
{
    std::map<std::string, std::string>::const_iterator it = _headers.find(key);
    if (it != _headers.end())
    {
        return it->second;
    }
    else
    {
        return "";
    }
}

std::string Request::getBody() const
{
	return _body;
}

