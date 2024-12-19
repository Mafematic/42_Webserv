#include "Path_Analyser.hpp"
#include "ServerManager.hpp"
#include "webserv.hpp"

bool	g_running = true;

void signalHandler(int signal)
{
	(void)signal;
	g_running = false;
}

int main(int argc, char **argv)
{
	if (argc > 2)
	{
		std::cout << "REQUIERED ARGUMENTS:" << std::endl;
		std::cout << "----> [./webserv] = default config file" << std::endl;
		std::cout << "----> [./webserv 'configfilepath'] = specific config file" << std::endl;
		return (1);
	}
	try
	{
		std::string config_file;
		if (argc == 1)
			config_file = "./config/default.conf";
		else
			config_file = argv[1];

		signal(SIGINT, signalHandler);
		ServerManager	manager;

		Logger::log(INFO, "Accessing config-file: " + config_file, "", "", -1);
		manager.setup(config_file);
		manager.run();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return (0);
}

// int	main(void)
// {
// 	Path_Analyser analyser;

// 	std::string test_str = "/cgi-bin/hello.py/test/hi/test.txt?a=1&b=2";
// 	std::string root = "./root/www/";
// 	analyser.analyse(test_str, root);
// 	std::cout << analyser;

// }
