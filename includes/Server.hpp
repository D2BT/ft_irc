#pragma once

#include "../includes/Client.hpp"
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
		int _port; // Port d'écoute du serveur IRC
		int _listenfd; // File descriptor de la socket d'écoute (serveur)
		std::string _password; // Mot de passe requis pour se connecter au serveur
		std::map<int, Client> _clients; // Liste des clients connectés (clé = fd)
		std::map<std::string, ICommand*> _commands; // Commandes IRC disponibles (NICK, USER, JOIN, ...)
		std::vector<pollfd> _pfds; // Liste des sockets surveillées par poll()

		// Traite une commande reçue d'un client
		bool handleCommand(Client &client, std::string &line);
		// Accepte une nouvelle connexion client
		int  acceptNewClient();
		// Reçoit et traite les données envoyées par un client
		void receiveFromClient(int fd);
		// Déconnecte proprement un client
		void disconnectClient(int fd);

	public:
		// Constructeur : initialise le serveur avec port et mot de passe
		Server(int port, std::string password);
		// Destructeur : libère les ressources
		~Server();

		// Variable globale pour arrêter proprement la boucle principale
		static volatile bool g_running;

		// Prépare le serveur à écouter les connexions
		void setup();
		// Boucle principale : gère les connexions et les messages
		void run();
};