#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "webserv.hpp"

class Logger {
	public:
		static void log(LogLevel level, const std::string& message, const std::string& method);

	private:
		static std::string getTimestamp();
		static std::string logLevelToString(LogLevel level);
		static std::string getColor(LogLevel level);
};

#endif // LOGGER_HPP
