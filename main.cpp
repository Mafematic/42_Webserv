#include "ServerManager.hpp"

bool g_running = true;

void signalHandler(int signal)
{
	(void)signal;
	g_running = false;
}

int main(int argc, char **argv)
{
	if (argc <= 2)
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
		std::cerr << RED << "Invalid number of arguments! \nREQUIERMENT: ./webserv /CONFIG_FILE" << std::endl;
	return 0;
}
