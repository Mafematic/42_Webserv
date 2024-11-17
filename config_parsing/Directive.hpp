#pragma once
#include "external.hpp"
#include "util.hpp"

class Directive
{
  public:
	std::string get_directive_name(void) const;

  protected:
	Directive();
	Directive(const Directive &other);
	Directive &operator=(const Directive &other);
	virtual ~Directive(void);

	std::vector<std::string> _key_val;
	std::string _directive_name;
	bool _many_args_allowed;
	bool _duplicate_definition_allowed;
	std::vector<std::string> _allowed_values;

	void check_for_invalid_num_args(void);
	void check_for_allowed_values(void);

	void set_key_val(std::vector<std::string> new_key_val);

	template <typename T>
	void check_for_duplicate_identifier(const std::vector<T> &vector)
	{
		if (!this->_duplicate_definition_allowed)
		{
			if (vector.size() > 0)
			{
				throw DuplicateIdentifier(this->get_directive_name());
			}
		}
	}

	class InvalidNumberOfArguments : public std::exception
	{
		public:
		InvalidNumberOfArguments(std::string directive_name);
		virtual ~InvalidNumberOfArguments() throw();
		virtual const char *what() const throw();

		private:
		std::string _directive_name;
		mutable std::string _msg;
	};

	class DuplicateIdentifier : public std::exception
	{
		public:
		DuplicateIdentifier(std::string directive_name);
		virtual ~DuplicateIdentifier() throw();
		virtual const char *what() const throw();

		private:
		std::string _directive_name;
		mutable std::string _msg;
	};

	class UnallowedArgumentForDirective : public std::exception
	{
		public:
		UnallowedArgumentForDirective(std::string directive_name);
		virtual ~UnallowedArgumentForDirective() throw();
		virtual const char *what() const throw();

		private:
		std::string _directive_name;
		mutable std::string _msg;
	};

	class AliasNotAllowedWithRoot : public std::exception
	{
		public:
		AliasNotAllowedWithRoot(std::string directive_name);
		virtual ~AliasNotAllowedWithRoot() throw();
		virtual const char *what() const throw();

		private:
		std::string _directive_name;
		mutable std::string _msg;
	};
};
