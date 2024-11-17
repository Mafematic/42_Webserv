/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Directive_Autoindex.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smatthes <smatthes@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/08 16:42:31 by smatthes          #+#    #+#             */
/*   Updated: 2024/11/10 18:29:16 by smatthes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Directive.hpp"
#include "external.hpp"
#include "util.hpp"

class Directive_Autoindex : public Directive
{
  public:
	Directive_Autoindex();
	Directive_Autoindex(const Directive_Autoindex &other);
	Directive_Autoindex &operator=(const Directive_Autoindex &other);
	virtual ~Directive_Autoindex(void);

	void check_and_add(std::vector<std::string> key_val);

	std::vector<bool> &get_config_defs(void);

  private:
	std::vector<bool> _config_defs;
};
