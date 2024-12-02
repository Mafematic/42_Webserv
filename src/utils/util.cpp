#include "external.hpp"
#include "util.hpp"

namespace util
{
std::string &ltrim(std::string &str)
{
	std::string::iterator it = str.begin();
	while (it != str.end() && std::isspace(*it))
	{
		it++;
	}
	str.erase(str.begin(), it);
	return (str);
}

std::string &rtrim(std::string &str)
{
	std::string::reverse_iterator rit = str.rbegin();
	while (rit != str.rend() && std::isspace(*rit))
	{
		++rit;
	}
	str.erase(rit.base(), str.end());
	return (str);
}

std::string &trim(std::string &str)
{
	return (ltrim(rtrim(str)));
}

std::vector<std::string> split(const std::string &str, char delimiter)
{
	std::vector<std::string> tokens;
	std::string token;
	for (size_t i = 0; i < str.size(); ++i)
	{
		if (str[i] == delimiter)
		{
			if (!token.empty())
			{
				tokens.push_back(trim(token));
				token.clear();
			}
		}
		else
			token += str[i];
	}
	if (!token.empty())
		tokens.push_back(trim(token));
	return (tokens);
}

std::vector<std::string> split(const std::string &str, char delimiter,
	size_t maxTokens = std::string::npos)
{
	size_t	tokenCount;

	std::vector<std::string> tokens;
	std::string token;
	tokenCount = 0;
	for (size_t i = 0; i < str.size(); ++i)
	{
		if (str[i] == delimiter)
		{
			if (!token.empty())
			{
				tokens.push_back(trim(token));
				token.clear();
				tokenCount++;
			}
			if (maxTokens != std::string::npos && tokenCount >= maxTokens)
				break ;
		}
		else
		{
			token += str[i];
		}
	}
	if (!token.empty() && (maxTokens == std::string::npos
			|| tokenCount < maxTokens))
		tokens.push_back(trim(token));
	return (tokens);
}

std::vector<std::string> split(const std::string &str, char delimiter,
	size_t maxTokens = std::string::npos, size_t start_index = 0)
{
	size_t	tokenCount;

	std::vector<std::string> tokens;
	std::string token;
	tokenCount = 0;
	for (size_t i = start_index; i < str.size(); ++i)
	{
		if (str[i] == delimiter)
		{
			if (!token.empty())
			{
				tokens.push_back(trim(token));
				token.clear();
				tokenCount++;
			}
			if (maxTokens != std::string::npos && tokenCount >= maxTokens)
				break ;
		}
		else
		{
			token += str[i];
		}
	}
	if (!token.empty() && (maxTokens == std::string::npos
			|| tokenCount < maxTokens))
		tokens.push_back(trim(token));
	return (tokens);
}

std::string join_lines(const std::vector<std::string> &lines)
{
	return (std::accumulate(lines.begin(), lines.end(), std::string("")));
}

int	find_first_opening_bracket_only_ws(const std::string &str,
		size_t start_pos = 0)
{
	while (start_pos < str.length() && std::isspace(str[start_pos]))
		++start_pos;
	if (start_pos < str.length() && str[start_pos] == '{')
		return (start_pos);
	else
		return (-1);
}

int	find_matching_closing_bracket(const std::string &str, int openPos)
{
	int	counter;

	if (str[openPos] != '{')
		return (-1);
	counter = 1;
	for (size_t i = openPos + 1; i < str.size(); ++i)
	{
		if (str[i] == '{')
			++counter;
		else if (str[i] == '}')
		{
			--counter;
			if (counter == 0)
				return (i);
		}
	}
	return (-1);
}

int	find_matching_closing_bracket_no_nesting(const std::string &str,
		int openPos)
{
	if (str[openPos] != '{')
		return (-1);
	for (size_t i = openPos + 1; i < str.size(); ++i)
	{
		if (str[i] == '{')
			break ;
		if (str[i] == '}')
			return (i);
	}
	return (-1);
}

std::string ensure_only_one_consecutive_ws(const std::string &input)
{
	bool	inWhitespace;

	std::string result;
	inWhitespace = false;
	for (std::string::const_iterator it = input.begin(); it != input.end(); ++it)
	{
		if (std::isspace(*it))
		{
			if (!inWhitespace)
			{
				result += ' ';
				inWhitespace = true;
			}
		}
		else
		{
			result += *it;
			inWhitespace = false;
		}
	}
	return (result);
}

char	get_last_char(std::string &str)
{
	return (str[str.length()]);
}

bool	is_digits_only(const std::string &str)
{
	for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
	{
		if (!std::isdigit(*it))
		{
			return (false);
		}
	}
	return (true);
}

bool	addressExists(const Address &a, const Address &b)
{
	return (a.ip == b.ip && a.port == b.port);
}

bool Address_Comparator::operator()(const Address &addr) const
{
	return (addr.ip == target.ip && addr.port == target.port);
}

Address_Comparator::Address_Comparator(const Address &target_address) : target(target_address)
{
}

std::map<std::string, std::string> get_color_map()
{
	std::map<std::string, std::string> color_map;
	color_map["reset"] = "\033[0m";    // Reset to default
	color_map["black"] = "\033[30m";   // Black
	color_map["red"] = "\033[31m";     // Red
	color_map["green"] = "\033[32m";   // Green
	color_map["yellow"] = "\033[33m";  // Yellow
	color_map["blue"] = "\033[34m";    // Blue
	color_map["magenta"] = "\033[35m"; // Magenta
	color_map["cyan"] = "\033[36m";    // Cyan
	color_map["white"] = "\033[37m";   // White
	return (color_map);
}

int	is_valid_http_code(std::string err_code)
{
	int	num;

	if (!util::is_digits_only(err_code))
		return (-1);
	num = atoi(err_code.c_str());
	if (num < 300 || num > 599)
		return (-1);
	return (num);
}

std::string to_string(int value)
{
	std::stringstream ss;
	ss << value;
	return ss.str();
}

bool ends_with(const std::string &str, const std::string &suffix)
{
	return str.size() >= suffix.size() &&
		   str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

bool directoryExists(const std::string &path)
{
	struct stat info;
	if (stat(path.c_str(), &info) != 0)
	{
		return false; // Path does not exist
	}
	return (info.st_mode & S_IFDIR) != 0;
}

bool fileExists(const std::string &path)
{
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0 && S_ISREG(buffer.st_mode));
}

} // namespace util
