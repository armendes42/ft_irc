#include "main.hpp"

int checkav2(std::string str)
{
	if (str.size() != 4)
		return 1;
	for (int i = 0;str[i]; i++)
		if (isdigit(str[i]) == 0)
			return 1;
	return 0;
}

int checkav1(char *av1)
{
	if (strlen(av1) != 4)
		return 1;
	int port = static_cast<int>(std::strtoul(av1, NULL, 0));
	if ((port >= 6660 && port <= 6669) || port == 6679 || port == 6697)
		return 0;
	return 1;
}

int main(int ac, char **av)
{
	try
	{
		if (ac != 3)
			throw std::runtime_error("Usage: ./ircserv <port> <password>");
		if (checkav1(av[1]) == 1)
			throw std::runtime_error("Wrong port.");
		if (checkav2(av[2]) == 1)
			throw std::runtime_error("Wrong password.");
		Server server(av[1], av[2]);
		server.connectToServer();
	}
	catch (const std::exception &ex)
	{
		std::cerr << ex.what() << std::endl;
		return 1;
	}
	return (0);
}