/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Path_Analyser.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smatthes <smatthes@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/15 16:35:59 by smatthes          #+#    #+#             */
/*   Updated: 2024/11/15 16:47:47 by smatthes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "webserv.hpp"
#include "util.hpp"

class Path_Analyser
{
  public:
	Path_Analyser();
	Path_Analyser(const Path_Analyser &other);
	Path_Analyser &operator=(const Path_Analyser &other);
	virtual ~Path_Analyser(void);

	void analyse(std::string path, std::string root);

	std::string path_to_process;
	std::string original_path_to_process;

	// /cgi-bin/hello.py/test/hi/test.txt?a=1&b=2"

	// hello.py
	std::string script_name;
	// /cgi-bin/hello.py
	std::string script_path;

	// ./root/cgi/cgi-bin/hello.py
	std::string path_translated;

	// py
	std::string script_extension;

	// /test/hi/test.txt
	std::string path_info;

	// a=1&b=2
	std::string query_string;

  private:
	void extract_query_string();
	void find_first_dot();
	void find_first_slash_after_dot();
	size_t	_pos_dot;
	size_t	_pos_slash;
	size_t	_pos_script_start;

};

std::ostream &operator<<(std::ostream &os, Path_Analyser const &path_analyser);
