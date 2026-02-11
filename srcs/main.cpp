// Point d'entrée du serveur IRC
#include <iostream>
#include <csignal>
#include "../includes/Exception.hpp"
#include "../includes/Color.hpp"
#include "../includes/Server.hpp"

// Vérifie si la chaîne s est un port valide (entre 1 et 65535)
static bool isValidPort(const std::string &s, int &port)
{
	if (s.empty())
		return false;
	for (size_t i = 0; i < s.size(); i++)
		if (!isdigit(s[i]))
			return false;
	long val = 0;
	for (size_t j = 0; j < s.size(); j++)
		val = val * 10 + (s[j] - '0');
	if (val < 1024 || val > 65535)
		return false;
	port = static_cast<int>(val);
	return true;
}

int main(int argc, char **argv)
{
	try {
		// Vérifie le nombre d'arguments (doit être 3 : programme, port, mot de passe)
		if (argc != 3)
			throw ErrorArg();

		int port;
		// Vérifie que le port est valide
		if (!isValidPort(argv[1], port))
			throw ErrorPort();

		// Récupère le mot de passe serveur
		std::string password = argv[2];

		// Crée le serveur IRC avec le port et le mot de passe
		Server server(port, password);

		signal(SIGINT, Server::signalHandler);
		signal(SIGTERM, Server::signalHandler);
		
		// Initialise le serveur (socket, options réseau, etc.)
		server.setup();

		// Lance la boucle principale du serveur (accepte les clients, traite les commandes)
		server.run();
	} catch (std::exception &e) {
		// Affiche l'erreur en rouge si problème
		std::cerr << RED << "Error: " << e.what() << RESET << std::endl;
		return 1;
	}
}