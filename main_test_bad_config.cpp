#include "Config_Parser.hpp"
#include "external.hpp"

bool g_running = true;

void	test_bad_configs(void);
void	normal(void);
void	test_bad_config(std::string file_path);
void	test_bad_general(void);
void	test_bad_directive_location_block(void);
void	test_bad_directive_server_block(void);

int	main(int argc, char *argv[])
{
	(void)argv;
	(void)argc;
	if (argc <= 1)
		normal();
	else
		test_bad_configs();
	return (0);
}

void	normal(void)
{
	std::string path = "config/config_steffen.conf";
	Config_Parser parser(path);
	try
	{
		parser.parse_config();
		// parser.print_file_line_by_line();
		// parser.print_server_blocks();
	}
	catch (std::exception &err)
	{
		std::cerr << err.what();
	}
}

void	test_bad_configs(void)
{
	// test_bad_general();
	test_bad_directive_location_block();
	// test_bad_directive_server_block();
}

void	test_bad_general(void)
{
	std::string subfolder = "src/config_parser/bad_configs/";
	std::string file_name;
	std::vector<std::string> test_config_names;
	test_config_names.push_back("directory.conf");
	test_config_names.push_back("no_access_rights.conf");
	test_config_names.push_back("empty.conf");
	test_config_names.push_back("too_small.conf");
	test_config_names.push_back("invalid_keyword_1.conf");
	test_config_names.push_back("invalid_keyword_2.conf");
	test_config_names.push_back("invalid_keyword_3.conf");
	test_config_names.push_back("invalid_keyword_4.conf");
	test_config_names.push_back("invalid_keyword_5.conf");
	test_config_names.push_back("no_opening_bracket_server_block_1.conf");
	test_config_names.push_back("no_opening_bracket_server_block_2.conf");
	test_config_names.push_back("no_opening_bracket_server_block_3.conf");
	test_config_names.push_back("no_closing_bracket_server_block_1.conf");
	test_config_names.push_back("no_closing_bracket_server_block_2.conf");
	test_config_names.push_back("no_closing_bracket_server_block_3.conf");
	test_config_names.push_back("invalid_keyword_after_first_server_block.conf");
	test_config_names.push_back("bad_location_definition_1.conf");
	test_config_names.push_back("bad_location_definition_2.conf");
	test_config_names.push_back("bad_location_definition_3.conf");
	test_config_names.push_back("bad_location_definition_4.conf");
	test_config_names.push_back("bad_location_definition_5.conf");
	test_config_names.push_back("bad_location_definition_6.conf");
	test_config_names.push_back("multiple_definitions_of_same_location_1.conf");
	test_config_names.push_back("multiple_definitions_of_same_location_2.conf");
	test_config_names.push_back("multiple_definitions_of_same_location_3.conf");
	test_config_names.push_back("multiple_definitions_of_same_location_4.conf");
	for (std::vector<std::string>::const_iterator it = test_config_names.begin(); it != test_config_names.end(); ++it)
	{
		test_bad_config(subfolder + *it);
	}
	std::cout << std::endl
				<< std::endl;
	std::cout << std::endl
				<< std::endl;
}

void	test_bad_directive_location_block(void)
{
	std::string subfolder = "src/config_parser/bad_configs/directives_location_block/";
	std::string file_name;
	std::vector<std::string> test_config_names;
	// test_config_names.push_back("invalid_keyword_inside_location_block_1.conf");
	// test_config_names.push_back("invalid_keyword_inside_location_block_2.conf");
	// test_config_names.push_back("invalid_keyword_inside_location_block_3.conf");
	// test_config_names.push_back("location_block_root_1.conf");
	// test_config_names.push_back("location_block_root_2.conf");
	// test_config_names.push_back("location_block_root_3.conf");
	// test_config_names.push_back("location_block_root_4.conf");
	// test_config_names.push_back("location_block_index_1.conf");
	// test_config_names.push_back("location_block_alias_1.conf");
	// test_config_names.push_back("location_block_alias_2.conf");
	// test_config_names.push_back("location_block_alias_3.conf");
	// test_config_names.push_back("location_block_alias_4.conf");
	// test_config_names.push_back("location_block_error_page_1.conf");
	// test_config_names.push_back("location_block_error_page_2.conf");
	// test_config_names.push_back("location_block_error_page_3.conf");
	// test_config_names.push_back("location_block_error_page_4.conf");
	// test_config_names.push_back("location_block_error_page_5.conf");
	// test_config_names.push_back("location_block_client_body_size_1.conf");
	// test_config_names.push_back("location_block_client_body_size_2.conf");
	// test_config_names.push_back("location_block_client_body_size_3.conf");
	// test_config_names.push_back("location_block_client_body_size_4.conf");
	// test_config_names.push_back("location_block_client_body_size_5.conf");
	// test_config_names.push_back("location_block_client_body_size_6.conf");
	// test_config_names.push_back("location_block_client_body_size_7.conf");
	// test_config_names.push_back("location_block_client_body_size_8.conf");
	// test_config_names.push_back("location_block_client_body_size_9.conf");
	// test_config_names.push_back("location_block_client_body_size_10.conf");
	// test_config_names.push_back("location_block_client_body_size_11.conf");
	// test_config_names.push_back("location_block_client_body_size_12.conf");
	// test_config_names.push_back("location_block_client_body_size_13.conf");
	// test_config_names.push_back("location_block_client_body_size_14.conf");
	// test_config_names.push_back("location_block_client_body_size_15.conf");
	// test_config_names.push_back("location_block_client_body_size_16.conf");
	// test_config_names.push_back("location_block_client_body_size_17.conf");
	// test_config_names.push_back("location_block_allowed_methods_1.conf");
	// test_config_names.push_back("location_block_allowed_methods_2.conf");
	// test_config_names.push_back("location_block_allowed_methods_3.conf");
	// test_config_names.push_back("location_block_allowed_methods_4.conf");
	// test_config_names.push_back("location_block_allowed_methods_5.conf");
	// test_config_names.push_back("location_block_allowed_methods_6.conf");
	// test_config_names.push_back("location_block_allowed_methods_7.conf");
	// test_config_names.push_back("location_block_allowed_methods_8.conf");
	// test_config_names.push_back("location_block_allowed_methods_9.conf");
	// test_config_names.push_back("location_block_autoindex_1.conf");
	// test_config_names.push_back("location_block_autoindex_2.conf");
	// test_config_names.push_back("location_block_autoindex_3.conf");
	// test_config_names.push_back("location_block_autoindex_4.conf");
	// test_config_names.push_back("location_block_autoindex_5.conf");
	// test_config_names.push_back("location_block_autoindex_6.conf");
	// test_config_names.push_back("location_block_autoindex_7.conf");
	// test_config_names.push_back("location_block_return_1.conf");
	// test_config_names.push_back("location_block_return_2.conf");
	// test_config_names.push_back("location_block_return_3.conf");
	// test_config_names.push_back("location_block_return_4.conf");
	// test_config_names.push_back("location_block_return_5.conf");
	// test_config_names.push_back("location_block_return_6.conf");
	// test_config_names.push_back("location_block_return_7.conf");
	// test_config_names.push_back("location_block_return_8.conf");
	test_config_names.push_back("location_block_cgi_1.conf");
	test_config_names.push_back("location_block_cgi_2.conf");
	test_config_names.push_back("location_block_cgi_3.conf");
	test_config_names.push_back("location_block_cgi_4.conf");
	test_config_names.push_back("location_block_cgi_5.conf");
	test_config_names.push_back("location_block_cgi_6.conf");
	test_config_names.push_back("location_block_cgi_7.conf");
	test_config_names.push_back("location_block_cgi_8.conf");
	test_config_names.push_back("location_block_cgi_9.conf");
	test_config_names.push_back("location_block_cgi_10.conf");
	test_config_names.push_back("location_block_cgi_11.conf");
	for (std::vector<std::string>::const_iterator it = test_config_names.begin(); it != test_config_names.end(); ++it)
	{
		test_bad_config(subfolder + *it);
	}
	std::cout << std::endl
				<< std::endl;
	std::cout << std::endl
				<< std::endl;
}

void	test_bad_directive_server_block(void)
{
	std::string subfolder = "src/config_parser/bad_configs/directives_server_block/";
	std::string file_name;
	std::vector<std::string> test_config_names;
	test_config_names.push_back("invalid_keyword_inside_location_block_1.conf");
	test_config_names.push_back("invalid_keyword_inside_server_block_1.conf");
	test_config_names.push_back("invalid_keyword_inside_server_block_2.conf");
	test_config_names.push_back("invalid_keyword_inside_server_block_3.conf");
	test_config_names.push_back("server_block_autoindex_1.conf");
	test_config_names.push_back("server_block_autoindex_2.conf");
	test_config_names.push_back("server_block_autoindex_3.conf");
	test_config_names.push_back("server_block_autoindex_4.conf");
	test_config_names.push_back("server_block_autoindex_5.conf");
	test_config_names.push_back("server_block_autoindex_6.conf");
	test_config_names.push_back("server_block_autoindex_7.conf");
	test_config_names.push_back("server_block_client_body_size_10.conf");
	test_config_names.push_back("server_block_client_body_size_11.conf");
	test_config_names.push_back("server_block_client_body_size_12.conf");
	test_config_names.push_back("server_block_client_body_size_13.conf");
	test_config_names.push_back("server_block_client_body_size_14.conf");
	test_config_names.push_back("server_block_client_body_size_15.conf");
	test_config_names.push_back("server_block_client_body_size_16.conf");
	test_config_names.push_back("server_block_client_body_size_17.conf");
	test_config_names.push_back("server_block_client_body_size_1.conf");
	test_config_names.push_back("server_block_client_body_size_2.conf");
	test_config_names.push_back("server_block_client_body_size_3.conf");
	test_config_names.push_back("server_block_client_body_size_4.conf");
	test_config_names.push_back("server_block_client_body_size_5.conf");
	test_config_names.push_back("server_block_client_body_size_6.conf");
	test_config_names.push_back("server_block_client_body_size_7.conf");
	test_config_names.push_back("server_block_client_body_size_8.conf");
	test_config_names.push_back("server_block_client_body_size_9.conf");
	test_config_names.push_back("server_block_error_page_1.conf");
	test_config_names.push_back("server_block_error_page_2.conf");
	test_config_names.push_back("server_block_error_page_3.conf");
	test_config_names.push_back("server_block_error_page_4.conf");
	test_config_names.push_back("server_block_error_page_5.conf");
	test_config_names.push_back("server_block_index_1.conf");
	test_config_names.push_back("server_block_listen_10.conf");
	test_config_names.push_back("server_block_listen_11.conf");
	test_config_names.push_back("server_block_listen_12.conf");
	test_config_names.push_back("server_block_listen_13.conf");
	test_config_names.push_back("server_block_listen_14.conf");
	test_config_names.push_back("server_block_listen_15.conf");
	test_config_names.push_back("server_block_listen_16.conf");
	test_config_names.push_back("server_block_listen_17.conf");
	test_config_names.push_back("server_block_listen_18.conf");
	test_config_names.push_back("server_block_listen_19.conf");
	test_config_names.push_back("server_block_listen_1.conf");
	test_config_names.push_back("server_block_listen_20.conf");
	test_config_names.push_back("server_block_listen_21.conf");
	test_config_names.push_back("server_block_listen_22.conf");
	test_config_names.push_back("server_block_listen_23.conf");
	test_config_names.push_back("server_block_listen_24.conf");
	test_config_names.push_back("server_block_listen_2.conf");
	test_config_names.push_back("server_block_listen_3.conf");
	test_config_names.push_back("server_block_listen_4.conf");
	test_config_names.push_back("server_block_listen_5.conf");
	test_config_names.push_back("server_block_listen_6.conf");
	test_config_names.push_back("server_block_listen_7.conf");
	test_config_names.push_back("server_block_listen_8.conf");
	test_config_names.push_back("server_block_listen_9.conf");
	test_config_names.push_back("server_block_root_1.conf");
	test_config_names.push_back("server_block_root_2.conf");
	test_config_names.push_back("server_block_root_3.conf");
	test_config_names.push_back("server_block_root_4.conf");
	test_config_names.push_back("server_block_server_name_1.conf");
	test_config_names.push_back("server_block_server_name_2.conf");
	test_config_names.push_back("server_block_return_1.conf");
	test_config_names.push_back("server_block_return_2.conf");
	test_config_names.push_back("server_block_return_3.conf");
	test_config_names.push_back("server_block_return_4.conf");
	test_config_names.push_back("server_block_return_5.conf");
	test_config_names.push_back("server_block_return_6.conf");
	test_config_names.push_back("server_block_return_7.conf");
	test_config_names.push_back("server_block_return_8.conf");
	for (std::vector<std::string>::const_iterator it = test_config_names.begin(); it != test_config_names.end(); ++it)
	{
		test_bad_config(subfolder + *it);
	}
	std::cout << std::endl
				<< std::endl;
	std::cout << std::endl
				<< std::endl;
}

void	test_bad_config(std::string file_path)
{
	Config_Parser parser(file_path);
	try
	{
		parser.parse_config();
		std::cout << "BAD ERROR NOT FOUND" << std::endl;
		std::cout << "File: " << file_path << std::endl;
		std::cout << std::endl
					<< std::endl;
	}
	catch (std::exception &err)
	{
		std::cout << "OK" << std::endl;
		std::cout << err.what();
		std::cout << std::endl
					<< std::endl;
	}
}
