#ifndef SERVER_HPP
# define SERVER_HPP

#define BUFFER_SIZE 3000

#include <unistd.h>
#include "../Networking/ListeningSocket.hpp"
#include "../Request/Request.hpp"


class Server
{
	private:
		char _buffer[BUFFER_SIZE];
		ListeningSocket* _listeningSocket;
		int _clientSocket;
		std::string _root;
		std::string _lastResponse;
		void _accepter();

	public:
		Server(in_addr_t ip, int port, int backlog, const std::string& _root);
		void launch();
		void sendResponse(const Request &req);
		const std::string& getLastResponse() const;
		ListeningSocket * getListeningSocket();
		virtual ~Server() { delete _listeningSocket; }
};

#endif