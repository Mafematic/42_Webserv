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
	if (argc == 2)
	{
		try
		{
			std::string config_file;
			if (argc == 1)
				config_file = "/config/default.conf";
			else
				config_file = argv[1];

			signal(SIGINT, signalHandler);
			ServerManager	manager;

			manager.setup(config_file);
			manager.run();
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
		}
	}
	else
		std::cerr << RED << "Invalid number of arguments! \nREQUIERMENT: ./webserv/CONFIG_FILE" << RESET << std::endl;
	return (0);
}

// int	main(void)
// {
// 	Path_Analyser analyser;

// 	std::string test_str = "/cgi-bin/hello.py/test/hi/test.txt?a=1&b=2";
// 	std::string root = "./root/www/";
// 	analyser.analyse(test_str, root);
// 	std::cout << analyser;

<<<<<<< Updated upstream
// }
=======
// }
>>>>>>> Stashed changes
