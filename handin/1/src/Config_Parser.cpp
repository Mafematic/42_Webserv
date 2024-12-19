/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config_Parser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smatthes  <smatthes@student.42berlin>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/08 14:55:39 by smatthes          #+#    #+#             */
/*   Updated: 2024/11/11 14:51:20 by smatthes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config_Parser.hpp"
#include "webserv.hpp"

Config_Parser::Config_Parser(std::string file_path) : _file_path(file_path)
{
	return ;
}

Config_Parser::Config_Parser(const Config_Parser &other)
{
	(void)other;
	return ;
}

Config_Parser &Config_Parser::operator=(const Config_Parser &other)
{
	if (this != &other)
	{
		;
	}
	return (*this);
}

Config_Parser::~Config_Parser(void)
{
	return ;
}

std::vector<Server> Config_Parser::parse_config()
{
	this->read_whole_file();
	this->trim_each_line();
	this->remove_empty_lines();
	this->remove_comments();
	this->check_for_invalid_key_words();
	this->lines_to_one_big_string();
	this->ensure_only_one_consecutive_ws();
	this->isolate_server_block_strings();
	this->parse_server_blocks();
	Server_Creator creator;
	return creator.create_servers(this->_server_parser);
}

void Config_Parser::ensure_only_one_consecutive_ws()
{
	this->_file_processed_one_string = util::ensure_only_one_consecutive_ws(this->_file_processed_one_string);
}
void Config_Parser::parse_server_blocks()
{
	for (std::vector<std::string>::const_iterator it = this->_server_block_strings.begin(); it != this->_server_block_strings.end(); ++it)
	{
		Server_Parser parser(*it);
		parser.parse_server_block();
		this->_server_parser.push_back(parser);
	}
}

void Config_Parser::lines_to_one_big_string()
{
	this->_file_processed_one_string = util::join_lines(this->_file_line_by_line);
}

void Config_Parser::isolate_server_block_strings()
{
	std::string remaining_config = this->_file_processed_one_string;
	while (remaining_config.length() > 0)
	{
		remaining_config = this->isolate_server_block(remaining_config);
	}
}

std::string &Config_Parser::isolate_server_block(std::string &remaining_config)
{
	int	index_opening_bracket;
	int	index_closing_bracket;

	if (remaining_config.find("server") != 0)
		throw InvalidConfigKeyWord();
	index_opening_bracket = util::find_first_opening_bracket_only_ws(remaining_config,
			std::string("server").length());
	if (index_opening_bracket < 0)
		throw OpeningBracketForServerBlockNotFound();
	index_closing_bracket = util::find_matching_closing_bracket(remaining_config,
			index_opening_bracket);
	if (index_closing_bracket < 0)
		throw ClosingBracketForServerBlockNotFound();
	std::string server_block = remaining_config.substr(index_opening_bracket
			+ 1, index_closing_bracket - index_opening_bracket - 1);
	remaining_config = remaining_config.substr(index_closing_bracket + 1,
			std::string::npos);
	remaining_config = util::trim(remaining_config);
	this->_server_block_strings.push_back(server_block);
	return (remaining_config);
}

void Config_Parser::check_for_invalid_key_words()
{
	bool	found;

	std::string first_word_of_line;
	for (std::vector<std::string>::iterator it_line = this->_file_line_by_line.begin(); it_line != this->_file_line_by_line.end(); ++it_line)
	{
		found = false;
		first_word_of_line = util::split(*it_line, ' ', 1, 0)[0];
		for (std::vector<std::string>::const_iterator it_keyword = Config_Parser::ALLOWED_CONFIG_KEYS.begin(); it_keyword != Config_Parser::ALLOWED_CONFIG_KEYS.end(); ++it_keyword)
		{
			if (first_word_of_line == *it_keyword)
			{
				found = true;
				break ;
			}
		}
		if (!found)
			throw InvalidConfigKeyWord();
	}
}

void Config_Parser::trim_each_line()
{
	for (std::vector<std::string>::iterator it = this->_file_line_by_line.begin(); it != this->_file_line_by_line.end(); ++it)
	{
		util::trim(*it);
	}
}

void Config_Parser::remove_empty_lines()
{
	std::vector<std::string>::iterator it = this->_file_line_by_line.begin();
	while (it != this->_file_line_by_line.end())
	{
		if (it->empty())
			it = this->_file_line_by_line.erase(it);
		else
			++it;
	}
}
void Config_Parser::remove_comments()
{
	std::vector<std::string>::iterator it = this->_file_line_by_line.begin();
	while (it != this->_file_line_by_line.end())
	{
		if ((*it)[0] == '#')
			it = this->_file_line_by_line.erase(it);
		else
			++it;
	}
}

void Config_Parser::read_whole_file()
{
	this->check_file_access();
	std::ifstream ifs(this->_file_path.c_str());
	if (!ifs.is_open())
		throw FileAccessError();
	std::string line;
	while (std::getline(ifs, line))
	{
		this->_file_line_by_line.push_back(line);
	}
	if (!ifs.eof() && (ifs.fail() || ifs.bad()))
		throw ReadFileStreamError();
	ifs.close();
}

void Config_Parser::check_file_access()
{
	struct stat	pathStat;

	if (access(this->_file_path.c_str(), R_OK) != 0)
		throw FileAccessError();
	if (stat(this->_file_path.c_str(), &pathStat) != 0)
		throw FileAccessError();
	if (S_ISDIR(pathStat.st_mode))
		throw FileAccessError();
	if (pathStat.st_size < 20)
		throw FileTooSmall();
}

void Config_Parser::print_file_line_by_line(void)
{
	for (std::vector<std::string>::const_iterator it = this->_file_line_by_line.begin(); it != this->_file_line_by_line.end(); ++it)
	{
		std::cout << *it << std::endl;
	}
}

void Config_Parser::print_server_blocks(void)
{
	for (std::vector<std::string>::const_iterator it = this->_server_block_strings.begin(); it != this->_server_block_strings.end(); ++it)
	{
		std::cout << *it << std::endl;
	}
}

const char *Config_Parser::FileAccessError::what() const throw()
{
	return ("Could not access/open config-file! Possible reasons include:\n"
			"- file does not exist\n"
			"- unable to retrieve file information\n"
			"- no access permissions for file\n"
			"- file is a directory\n");
}

const char *Config_Parser::FileTooSmall::what() const throw()
{
	return ("Config-file is too small and thus can't be a valid config file!\n");
}

const char *Config_Parser::ReadFileStreamError::what() const throw()
{
	return ("File stream reading the config file encountered and error!\n");
}

const char *Config_Parser::InvalidConfigKeyWord::what() const throw()
{
	return ("An invalid keyword was encountered while parsing the config file!\n");
}

const char *Config_Parser::OpeningBracketForServerBlockNotFound::what() const throw()
{
	return ("An opening bracket after the server keyword couldn't be found!\n");
}

const char *Config_Parser::ClosingBracketForServerBlockNotFound::what() const throw()
{
	return ("The closing bracket for a server block couldn't be found!\n");
}

static std::vector<std::string> create_allowed_config_keys()
{
	std::vector<std::string> keys;
	keys.push_back("server");
	keys.push_back("location");
	keys.push_back("listen");
	keys.push_back("root");
	keys.push_back("index");
	keys.push_back("server_name");
	keys.push_back("error_page");
	keys.push_back("client_max_body_size");
	keys.push_back("allowed_methods");
	keys.push_back("autoindex");
	keys.push_back("alias");
	keys.push_back("{");
	keys.push_back("}");
	keys.push_back("return");
	keys.push_back("cgi");
	// keys.push_back("send_timeout");
	// keys.push_back("CGI");
	// keys.push_back("exec");
	// keys.push_back("php");
	return (keys);
}

const std::vector<std::string> Config_Parser::ALLOWED_CONFIG_KEYS = create_allowed_config_keys();
