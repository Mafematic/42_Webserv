#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <map>

class Request
{
	private:
		std::string _method;
		std::string _path;
		std::string _version;
		std::map<std::string, std::string> _headers;
		bool _valid;
		std::string _error_message;
		std::string _raw_request;

		void _parseRequest(const std::string &raw_request);
		void _validateRequest();

	public:
		Request(const std::string &raw_request);

		bool isValid() const;
		std::string getMethod() const;
		std::string getPath() const;
		std::string getVersion() const;
		std::string getErrorMessage() const;
		std::string getRawRequest() const;
		std::string getHeader(const std::string &key) const;

};

#endif