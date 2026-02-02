#pragma once

#include "../includes/Client.hpp"
#include "../includes/ICmd.hpp"
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>
#include <cerrno>
#include <stdio.h>
#include <exception>
#include <sstream>
#include <fcntl.h>

class Server{
	private:
		int _port;
		int _listenfd;
		std::string _password;
		std::map<int, Client> _clients;
		std::map<std::string, ICmd*> _commands;
		std::vector<pollfd> _pfds;

		bool handleCommand(Client &client, std::string &line);
		int  acceptNewClient();
		void receiveFromClient(int fd);
		void disconnectClient(int fd);

	public:
		Server(int port, std::string password);
		~Server();

		static volatile bool g_running;

		void setup();
		void run();
};