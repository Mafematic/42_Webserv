#include "Server.hpp"
#include "../Request/Request.hpp"
#include "../RequestRouter/RequestRouter.hpp"
#include "../FileUploader/Uploader.hpp"
#include <cstring>
#include <sstream>
#include <fstream>
#include <iostream>

Server::Server(in_addr_t ip, int port, int backlog, const std::string& root) : _root(root)
{
	memset(_buffer, 0, sizeof(_buffer));

	int domain = AF_INET;
    int service = SOCK_STREAM;
    int protocol = 0;

    _listeningSocket = new ListeningSocket(domain, service, protocol, port, ip, backlog);

	launch();
}

ListeningSocket *Server::getListeningSocket()
{
	return _listeningSocket;
}

void Server::_accepter()
{
    struct sockaddr_in address = getListeningSocket()->get_address();
    int addrlen = sizeof(address);
    _clientSocket = accept(getListeningSocket()->get_sock(), (struct sockaddr *)&address, (socklen_t *)&addrlen);
    if (_clientSocket < 0)
    {
        perror("Failed to accept connection");
        exit(EXIT_FAILURE);
    }

    // Read the client's request into buffer (headers + possible body)
    int bytes_read = read(_clientSocket, _buffer, sizeof(_buffer) - 1);
    if (bytes_read <= 0)
    {
        perror("Failed to read request");
        close(_clientSocket);
        return;
    }
    _buffer[bytes_read] = '\0';

    // Extract Content-Length from headers to read full body
    std::string request(_buffer);
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
            int extra_bytes = read(_clientSocket, temp_buffer, sizeof(temp_buffer) - 1);
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
    strncpy(_buffer, request.c_str(), sizeof(_buffer) - 1);
    _buffer[sizeof(_buffer) - 1] = '\0';
}

void Server::launch()
{
    while (true)
    {
        std::cout << "====== WAITING =====" << std::endl;

        _accepter();
        std::cout << "===== RECEIVED REQUEST =====" << std::endl;
        std::cout << _buffer << std::endl;

        Request req(_buffer); // Parse the raw request

        _lastResponse = RequestRouter::route(req);
        sendResponse(req);

        std::cout << "====== DONE =====" << std::endl;
    }
}


void Server::sendResponse(const Request &req)
{
	bool keepAlive = req.getHeader("Connection") == "keep-alive";

    ssize_t bytesSent = send(_clientSocket, _lastResponse.c_str(), _lastResponse.length(), 0); // 0 = no flags
    if (bytesSent < 0)
    {
        perror("Failed to send response");
    }
	if (!keepAlive)
    {
        close(_clientSocket);
    }
}

const std::string& Server::getLastResponse() const
{
    return _lastResponse;
}

