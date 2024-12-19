/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Directive_Return.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smatthes  <smatthes@student.42berlin>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/08 16:42:02 by smatthes          #+#    #+#             */
/*   Updated: 2024/11/11 14:51:20 by smatthes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Directive.hpp"
#include "webserv.hpp"
#include "util.hpp"

class Directive_Return : public Directive
{
  public:
	Directive_Return();
	Directive_Return(const Directive_Return &other);
	Directive_Return &operator=(const Directive_Return &other);
	virtual ~Directive_Return(void);

	void check_and_add(std::vector<std::string> key_val);
	void process_return_definition(std::vector<std::string> &key_val);


	std::vector<util::Return_Definition> &get_config_defs(void);

	class InvalidReturnCodeFormat : public std::exception
	{
		public:
		InvalidReturnCodeFormat(std::string directive_name);
		virtual ~InvalidReturnCodeFormat() throw();
		virtual const char *what() const throw();

		private:
		std::string _directive_name;
		mutable std::string _msg;
	};

  private:
	std::vector<util::Return_Definition> _config_defs;
};
