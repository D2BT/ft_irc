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
#include "ICmd.hpp"

class Server{
	private:
		int _port;
		int _listenfd;
		std::string _serverName;
		std::string _password;
		std::map<int, Client> _clients;
		std::map<std::string, ICmd*> _commands;
		std::vector<pollfd> _pfds;

		int  acceptNewClient();
		// Reçoit et traite les données envoyées par un client
		void receiveFromClient(int fd);
		bool handleCommand(Client &client, std::string &line);
		
		public:
		Server(int port, std::string password);
		// Destructeur : libère les ressources
		~Server();
		
		static volatile bool g_running;
		static void signalHandler(int signum);
		
		void sendReply(const Client& client, const std::string& message);
		void disconnectClient(int fd);

		// Prépare le serveur à écouter les connexions
		void setup();
		// Boucle principale : gère les connexions et les messages
		void run();
};