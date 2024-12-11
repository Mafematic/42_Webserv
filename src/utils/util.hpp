#include "external.hpp"
#pragma once

namespace util
{
// functions
std::string &ltrim(std::string &str);
std::string &rtrim(std::string &str);
std::string &trim(std::string &str);
std::vector<std::string> split(const std::string &str, char delimiter);
std::vector<std::string> split(const std::string &str, char delimiter,
	size_t maxTokens);
std::vector<std::string> split(const std::string &str, char delimiter,
	size_t maxTokens, size_t start_index);

std::string join_lines(const std::vector<std::string> &lines);
int		find_first_opening_bracket_only_ws(const std::string &str,
			size_t start_pos);
int		find_matching_closing_bracket(const std::string &str, int openPos);
std::string ensure_only_one_consecutive_ws(const std::string &input);
char	get_last_char(std::string &str);
int		find_matching_closing_bracket_no_nesting(const std::string &str,
			int openPos);
bool	is_digits_only(const std::string &str);
std::map<std::string, std::string> get_color_map();
int		is_valid_http_code(std::string err_code);

std::string to_string(int value);
bool	ends_with(const std::string &str, const std::string &suffix);
bool	directoryExists(const std::string &path);
bool	fileExists(const std::string &path);
std::string int_to_string(int number);
void	print_n_newlines(uint n);
void	replace_all(std::string &str, std::string search_for,
			std::string replace_with);

// template functions
template <typename Container> void print_container(const Container &container,
	std::ostream &os = std::cout)
{
	typename Container::const_iterator it;
	os << "***Print Container***" << std::endl;
	for (it = container.begin(); it != container.end(); ++it)
	{
		os << *it << std::endl;
	}
	os << "***End of Container***" << std::endl << std::endl;
}

template <typename T> void vector_append_from_index(std::vector<T> &dest,
	const std::vector<T> &src, std::size_t start_index)
{
	if (start_index < src.size())
	{
		dest.insert(dest.end(), src.begin() + start_index, src.end());
	}
}

// structs

struct	Return_Definition
{
	int	status_code;
	std::string url;
};

struct	Address
{
	std::string ip;
	int	port;
};

struct	Address_Comparator
{
	const Address &target;

	Address_Comparator(const Address &target_address);

	bool operator()(const Address &addr) const;
};

struct	ReplaceDash
{
	char operator()(char c) const
	{
		return (c == '-' ? '_' : c);
	}
};

} // namespace util
