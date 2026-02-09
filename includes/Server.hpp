#pragma once

#include "../includes/Client.hpp"
#include "../includes/ICmd.hpp"
#include "../includes/Channel.hpp"
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
		std::string _serverName;
		std::string _password;
		std::map<int, Client> _clients;
		std::map<std::string, ICmd*> _commands;
		std::map<std::string, Channel *> _channels;
		std::vector<pollfd> _pfds;

		int  acceptNewClient();
		void receiveFromClient(int fd);
		bool handleCommand(Client &client, std::string &line);
		
	public:
		Server(int port, std::string password);
		~Server();
		
		static volatile bool g_running;
		static void signalHandler(int signum);

		Client* getClientByNick(const std::string& nick);
        const std::string& getPassword() const;
        const std::string& getServerName() const;
		
		void sendReply(const Client& client, const std::string& message);
		void disconnectClient(int fd);
		
		void sendToClient(Client& client, const std::string& message);
		Channel *createChannel(std::string channelName, std::string password = "");
		Channel *getChannel(std::string const &channelName) const;
		std::map<std::string, Channel *> getChannels() const;

		void setup();
		void run();
};