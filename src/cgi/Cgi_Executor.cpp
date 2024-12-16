#include "Cgi_Controller.hpp"
#include "Cgi_Executor.hpp"
#include "ServerManager.hpp"

Cgi_Executor::Cgi_Executor(Cgi_Controller *val_corresponding_controller) : corresponding_controller(val_corresponding_controller),
	env_arr(NULL), argv_arr(NULL), body("")
{
	this->_corresponding_client = this->corresponding_controller->corresponding_client;
	this->_corresponding_server = this->corresponding_controller->corresponding_client.getServer();
	this->_corresponding_request = this->corresponding_controller->corresponding_client.getRequest();
	this->_corresponding_route = this->corresponding_controller->corresponding_client.getRoute();
	return ;
}

Cgi_Executor::Cgi_Executor(const Cgi_Executor &other)
{
	this->corresponding_controller = other.corresponding_controller;
	this->env_map = other.env_map;
	this->_corresponding_client = other._corresponding_client;
	this->_corresponding_server = other._corresponding_server;
	this->_corresponding_request = other._corresponding_request;
	this->_corresponding_route = other._corresponding_route;
	this->_path_analyser = other._path_analyser;
	return ;
}

Cgi_Executor &Cgi_Executor::operator=(const Cgi_Executor &other)
{
	if (this != &other)
	{
		this->corresponding_controller = other.corresponding_controller;
		this->env_map = other.env_map;
		this->_corresponding_client = other._corresponding_client;
		this->_corresponding_server = other._corresponding_server;
		this->_corresponding_request = other._corresponding_request;
		this->_corresponding_route = other._corresponding_route;
		this->_path_analyser = other._path_analyser;
	}
	return (*this);
}

Cgi_Executor::~Cgi_Executor(void)
{
	if (this->env_arr)
	{
		for (int i = 0; this->env_arr[i] != NULL; ++i)
		{
			delete[] this->env_arr[i];
			this->env_arr[i] = NULL;
		}
		delete[] this->env_arr;
		this->env_arr = NULL;
	}
	if (this->argv_arr)
	{
		for (int i = 0; this->argv_arr[i] != NULL; ++i)
		{
			delete[] this->argv_arr[i];
			this->argv_arr[i] = NULL;
		}
		delete[] this->argv_arr;
		this->argv_arr = NULL;
	}
	return ;
}

void Cgi_Executor::start_cgi()
{
	this->body = this->_corresponding_request.getBody();
	this->analyse_path();
	std::cout << "HERE" << std::endl;
	std::cout << this->_path_analyser;
	std::cerr << this->_path_analyser;
	this->init_env_map();
	this->add_http_headers_to_env_map();
	this->env_map_to_env_arr();
	this->create_argv_arr();
	this->put_request_body_into_stdin();
	if (dup2(this->corresponding_controller->pipe_receive_cgi_answer[1],
			STDOUT_FILENO) < 0)
		throw(CgiExecutorSystemFunctionFailed("dup2"));
	if (close(this->corresponding_controller->pipe_receive_cgi_answer[1]) < 0)
		throw(CgiExecutorSystemFunctionFailed("close"));
	if (close(this->corresponding_controller->pipe_receive_cgi_answer[0]) < 0)
		throw(CgiExecutorSystemFunctionFailed("close"));
	this->change_to_cgi_directory();
	this->run_script();
}

void Cgi_Executor::put_request_body_into_stdin()
{
	const char	*temp_file;
	int			fd;

	temp_file = this->corresponding_controller->tmp_file_name.c_str();
	std::ofstream out(temp_file);
	out << this->body;
	out.close();
	fd = open(temp_file, O_RDONLY);
	if (fd < 0)
		throw(CgiExecutorSystemFunctionFailed("open"));
	if (dup2(fd, STDIN_FILENO) < 0)
		throw(CgiExecutorSystemFunctionFailed("dup2"));
	if (close(fd) < 0)
		throw(CgiExecutorSystemFunctionFailed("close"));
}

void Cgi_Executor::init_env_map()
{
	// required for php-cgi >>>
	this->env_map["REDIRECT_STATUS"] = "200";
	this->env_map["SCRIPT_FILENAME"] = this->_path_analyser.script_name;
	// <<<< required for php-cgi
	if (this->body.length() > 0)
		this->env_map["CONTENT_LENGTH"] = util::int_to_string(body.length());
	std::string content_type = this->_corresponding_request.getHeader("Content-Type");
	if (content_type.length() > 0)
		this->env_map["CONTENT_TYPE"] = content_type;
	this->env_map["GATEWAY_INTERFACE"] = "CGI/1.1";
	this->env_map["PATH_INFO"] = this->_path_analyser.path_info;
	this->env_map["PATH_TRANSLATED"] = this->_path_analyser.path_translated;
	this->env_map["QUERY_STRING"] = this->_path_analyser.query_string;
	this->env_map["REMOTE_ADDR"] = this->_corresponding_client.getIp();
	this->env_map["REMOTE_HOST"] = this->_corresponding_client.getIp();
	this->env_map["REQUEST_METHOD"] = this->_corresponding_request.getMethod();
	// here check again
	this->env_map["SCRIPT_NAME"] = this->_path_analyser.script_path;
	std::vector<std::string> server_names = this->_corresponding_server.get_server_name();
	if (server_names.size() > 0)
		this->env_map["SERVER_NAME"] = server_names[0];
	else
		this->env_map["SERVER_NAME"] = this->_corresponding_server.get_ip();
	this->env_map["SERVER_PORT"] = util::int_to_string(this->_corresponding_server.get_port());
	this->env_map["SERVER_PROTOCOL"] = "HTTP/1.1";
	this->env_map["SERVER_SOFTWARE"] = "socket_squad_404/1.0";
	std::cerr << this->_corresponding_server.get_final_root(this->_corresponding_route);
}

void Cgi_Executor::analyse_path()
{
	std::string full_request_path = this->_corresponding_request.getPath();
	std::string final_root = this->_corresponding_server.get_final_root(this->_corresponding_route);
	this->_path_analyser.analyse(full_request_path, final_root);
}

void Cgi_Executor::add_http_headers_to_env_map()
{
	std::vector<std::string> headers_to_omit;
	headers_to_omit.push_back("Authorization");
	headers_to_omit.push_back("Connection");
	headers_to_omit.push_back("Content-Length");
	headers_to_omit.push_back("Content-Type");
	std::map<std::string,
		std::string> headers = this->_corresponding_request.getHeaderMap();
	for (std::map<std::string,
		std::string>::iterator it = headers.begin(); it != headers.end(); it++)
	{
		if (std::find(headers_to_omit.begin(), headers_to_omit.end(),
				it->first) == headers_to_omit.end())
		{
			std::string transformed = it->first;
			std::transform(transformed.begin(), transformed.end(),
				transformed.begin(), static_cast<int (*)(int)>(std::toupper));
			std::transform(transformed.begin(), transformed.end(),
				transformed.begin(), util::ReplaceDash());
			transformed.insert(0, "HTTP_");
			this->env_map[transformed] = it->second;
		}
	}
}

void Cgi_Executor::env_map_to_env_arr()
{
	int	i;

	this->env_arr = new char *[this->env_map.size() + 1];
	if (!this->env_arr)
		throw(CgiExecutorSystemFunctionFailed("new"));
	i = 0;
	std::map<std::string, std::string>::iterator it;
	for (it = this->env_map.begin(); it != this->env_map.end(); ++it)
	{
		std::string val = it->first + "=" + it->second;
		this->env_arr[i] = NULL;
		this->env_arr[i] = new char[val.size() + 1];
		if (!this->env_arr[i])
			throw(CgiExecutorSystemFunctionFailed("new"));
		std::strcpy(this->env_arr[i], val.c_str());
		i++;
		this->env_arr[i] = NULL;
	}
}

void Cgi_Executor::create_argv_arr()
{
	std::string arg_0 = this->_corresponding_route.get_cgi_interpreter(this->_path_analyser.script_extension);
	if (arg_0.length() == 0)
		arg_0 = this->_corresponding_route.get_cgi_interpreter("sh");
	std::string arg_1 = this->_path_analyser.script_name;
	this->argv_arr = new char *[3];
	this->argv_arr[0] = NULL;
	this->argv_arr[1] = NULL;
	this->argv_arr[2] = NULL;
	if (!this->argv_arr)
		throw(CgiExecutorSystemFunctionFailed("new"));
	this->argv_arr[0] = new char[arg_0.size() + 1];
	if (!this->argv_arr[0])
		throw(CgiExecutorSystemFunctionFailed("new"));
	std::strcpy(this->argv_arr[0], arg_0.c_str());
	this->argv_arr[1] = new char[arg_1.size() + 1];
	if (!this->argv_arr[1])
		throw(CgiExecutorSystemFunctionFailed("new"));
	std::strcpy(this->argv_arr[1], arg_1.c_str());
}

void Cgi_Executor::change_to_cgi_directory()
{
	if(chdir(this->_path_analyser.path_translated_folder.c_str()) == -1)
		throw(CgiExecutorSystemFunctionFailed("chdir"));
}

void Cgi_Executor::run_script()
{
	util::print_n_newlines(3);
	std::cerr << this->argv_arr[0] << std::endl;
	std::cerr << this->argv_arr[1] << std::endl;
	util::print_n_newlines(3);
	std::cout << std::flush;
	execve(this->argv_arr[0], this->argv_arr, this->env_arr);
	exit(12);
	// throw(CgiExecutorSystemFunctionFailed("execve"));
}

Cgi_Executor::CgiExecutorSystemFunctionFailed::CgiExecutorSystemFunctionFailed(std::string function_name) : _function_name(function_name)
{
}

Cgi_Executor::CgiExecutorSystemFunctionFailed::~CgiExecutorSystemFunctionFailed() throw()
{
}

const char *Cgi_Executor::CgiExecutorSystemFunctionFailed::what() const throw()
{
	this->_msg = "system call: ";
	this->_msg += this->_function_name;
	this->_msg += " failed inside the Cgi_Executor";
	this->_msg += "\n";
	return (this->_msg.c_str());
}
