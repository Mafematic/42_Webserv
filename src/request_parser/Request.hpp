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
		//std::map<std::string, std::string> _bodyHeaders;
    	//std::string _contentDisposition;
    	//std::string _contentType;

		bool _valid;
		std::string _error_message;
		std::string _raw_request;
		std::string _body;

		bool _keepAlive;

		void _parseRequest();
		void _validateRequest();

	public:
		Request();
		Request(const std::string &raw_request);
		Request(const Request &src);
		Request &operator=(const Request &src);
		~Request();

		bool isValid() const;
		std::string getMethod() const;
		std::string getPath() const;
		std::string getVersion() const;
		std::string getErrorMessage() const;
		std::string getRawRequest() const;
		std::string getHeader(const std::string &key) const;

		std::string getContentDisposition() const;
    	std::string getContentType() const;

		std::string getBody() const;

		void setKeepAlive(bool keepAlive);
    	bool getKeepAlive() const;

};

#endif
