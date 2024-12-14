#include "Logger.hpp"

void Logger::log(LogLevel lvl, const std::string& msg)
{
	std::string levelStr = logLevelToString(lvl); // Call static functions directly
	std::string timestamp = getTimestamp();
	std::string colorcode = getColor(lvl);

	std::ostringstream logStream;
	logStream << "[" << timestamp << "] " << colorcode
			<< "[" << levelStr << "] "
			<< msg << RESET;

	std::cout << logStream.str() << std::endl;
}

std::string Logger::getColor(LogLevel level)
{
	switch (level)
	{
		case DEBUG: return BLUE;
		case INFO: return GREEN;
		case TRACE: return PURPLE;
		case WARNING: return YELLOW;
		case ERROR: return RED;
		default: return RESET;
	}
}

std::string Logger::getTimestamp()
{
	std::time_t now = std::time(NULL);
	char buf[20];
	std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
	return buf;
}

std::string Logger::logLevelToString(LogLevel level)
{
	switch (level)
	{
		case DEBUG: return "DEBUG";
		case INFO: return "INFO";
		case TRACE: return "TRACE";
		case WARNING: return "WARNING";
		case ERROR: return "ERROR";
		default: return "UNKNOWN";
	}
}
