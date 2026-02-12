#include <iostream>
#include "../includes/Exception.hpp"
#include "../includes/Color.hpp"
#include "../includes/Server.hpp"

static bool	isValidPort(const std::string &s, int &port)
{
	if (s.empty())
		return false;
	
	for (size_t i = 0; i < s.size(); i++)
		if (!isdigit(s[i])) {return false;}
	
	long val = 0;
	for (size_t j = 0; j < s.size(); j++){
        val = val * 10 + (s[j] - '0');
	}
	if (val < 1 || val > 65535) {return false;}

	port = static_cast<int>(val);
	return true;
}

int main(int argc, char **argv)
{
	try{
		if (argc !=3 || !argv[2][0])
			throw ErrorArg();
		int port;
		if (!isValidPort(argv[1], port))
			throw ErrorPort();
		std::string password = argv[2];
		Server server(port, password);
		
		signal(SIGINT, Server::signalHandler);
		signal(SIGTERM, Server::signalHandler);

		server.setup(); 
		server.run();
	} catch (std::exception &e){
		std::cerr << RED << "Error: " << e.what() << RESET << std::endl;
		return 1;
	}
}