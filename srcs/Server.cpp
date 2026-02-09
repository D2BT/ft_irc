#include "../includes/Server.hpp"
#include "../includes/Exception.hpp"
#include "../includes/Logger.hpp"
#include "../includes/Client.hpp"
#include "../includes/NickCmd.hpp"
#include "../includes/UserCmd.hpp"
#include "../includes/PassCmd.hpp"
#include "../includes/PingCmd.hpp"
#include "../includes/ModeCmd.hpp"
#include "../includes/JoinCmd.hpp"
#include "../includes/PartCmd.hpp"
#include "../includes/PrivmsgCmd.hpp"
#include "../includes/KickCmd.hpp"
#include "../includes/QuitCmd.hpp"
#include "../includes/TopicCmd.hpp"
#include "../includes/InviteCmd.hpp"
#include "../includes/Bot.hpp"
//#include "../includes/Channel.hpp"

volatile bool Server::g_running = true;

Server::Server(int port, std::string password): _port(port), _listenfd(-1), _serverName("my_irc_server"), _password(password){
    /* try {
        _commands["NICK"] = new NickCmd();
        _commands["USER"] = new UserCmd();
        _commands["PASS"] = new PassCmd();
        _commands["PING"] = new PingCmd();
        _commands["MODE"] = new ModeCmd();
        _commands["JOIN"] = new JoinCmd();
        _commands["PRIVMSG"] = new PrivmsgCmd();
        _commands["PART"] = new PartCmd();
        _commands["KICK"] = new KickCmd();
        _commands["QUIT"] = new QuitCmd();
        _commands["TOPIC"] = new TopicCmd();
        _commands["INVITE"] = new InviteCmd();
    }
    catch (std::bad_alloc & e) {
        for(std::map<std::string, ICmd*>::iterator it = _commands.begin(); it != _commands.end(); it++) {
            delete it->second;
        }
        _commands.clear();
        throw;
    } */
}

// Destructeur du serveur IRC
// Libère la mémoire des commandes et des clients, ferme les sockets
Server::~Server(){
	std::map<std::string, ICmd*>::iterator ite = _commands.begin();
    for (; ite != _commands.end(); ++ite)
        delete ite->second;
    _commands.clear();

    for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it){
        close (it->first);
    }
    _clients.clear();

    if (_listenfd != -1)
        close (_listenfd);

	// close le bot ici si il est actif
}

void Server::signalHandler(int signum){
	(void)signum;
    std::ostringstream msg;
    msg << "Signal " << signum << " reçu. Arrêt du serveur...";
    Logger::log(INFO, msg.str());
    Server::g_running = false;
}

// Prépare le serveur à écouter les connexions entrantes
// Initialise la socket serveur et la prépare à accepter des connexions
void Server::setup(){
	// Crée la socket d'écoute (AF_INET = IPv4, SOCK_STREAM = TCP)
	_listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (_listenfd < 0)
		throw SocketError();

	// Permet de réutiliser rapidement le port après un crash (évite l'erreur "address already in use")
	int opt = 1;
	if (setsockopt(_listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		throw SocketError();

	// Met la socket en mode non-bloquant (indispensable pour poll/select)
	if (fcntl(_listenfd, F_SETFL, O_NONBLOCK) < 0)
		throw SocketError();


	// --- VERSION ACTUELLE (IPv4 uniquement) ---
	// Prépare la structure d'adresse pour écouter sur toutes les interfaces réseau (0.0.0.0)
	sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET; // IPv4 uniquement ici
	addr.sin_addr.s_addr = INADDR_ANY; // Accepte les connexions sur toutes les IP locales
	addr.sin_port = htons(_port); // Port choisi par l'utilisateur

	// Associe la socket à l'adresse et au port (bind = "réserver" le port sur la machine)
	if (bind(_listenfd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
		throw SocketError();

	// Met la socket en mode écoute (prête à accepter des connexions entrantes)
	if (listen(_listenfd, SOMAXCONN) < 0) // SOMAXCONN (souvent 4096 sur Linux) définit la taille de la file d’attente des connexions en attente,
		throw SocketError();              // pas le nombre total de clients connectés.

	std::cout << "IRC server listening on port " << _port << std::endl;
}

// Accepte une nouvelle connexion client
int  Server::acceptNewClient(){
	sockaddr_in clientAddr;
	socklen_t clientLen = sizeof(clientAddr);

	// Accepte la connexion entrante (retourne un nouveau fd pour le client)
	int client_fd = accept(_listenfd, (sockaddr*)&clientAddr, &clientLen);
	if (client_fd < 0){
		Logger::log(ERROR, "Erreur lors de l'acceptation du client");
		return -1;
	}

	// Met la socket client en non-bloquant
	fcntl(client_fd, F_SETFL, O_NONBLOCK);

	// Ajoute le client à la map des clients (clé = fd)
	_clients[client_fd] = Client(client_fd);

	// Ajoute le fd du client à la liste surveillée par poll()
	// Prépare une structure pollfd pour surveiller ce nouveau client
	pollfd newFd;
	newFd.fd = client_fd;         // Le file descriptor du client (socket créée par accept)
	newFd.events = POLLIN;        // On veut être notifié quand le client envoie des données (POLLIN = données à lire)
	newFd.revents = 0;            // Champ réservé pour les événements détectés par poll (initialisé à 0)
	_pfds.push_back(newFd);       // On ajoute ce client à la liste des sockets surveillées par poll()

	// Log l'adresse IP et le port du client
	char ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &clientAddr.sin_addr, ip, INET_ADDRSTRLEN);
	int port = ntohs(clientAddr.sin_port);

	std::ostringstream msg;
	msg << "Nouveau client connecté (fd=" << client_fd << ", " << ip << ":" << port << ")";
	Logger::log(INFO, msg.str());

	return client_fd;
}

// Reçoit et traite les données envoyées par un client
void Server::receiveFromClient(int fd){
	char buffer[512];
	// Lit les données envoyées par le client (non-bloquant)
	size_t bytes_read = recv(fd, buffer, sizeof(buffer) - 1, 0); // ", 0" = comportement par defaut

	// Si bytes_read == 0, le client a fermé la connexion
	if (bytes_read == 0){
		std::ostringstream msg;
		msg << "Client (fd=" << fd << ") a fermé la connexion.";
		Logger::log(INFO, msg.str());
		disconnectClient(fd);
		return;
	}
	// Si erreur de lecture
	else if (bytes_read < 0){
		Logger::log(ERROR, "Erreur recv()");
		disconnectClient(fd);
	}

	// Ajoute les données reçues au buffer du client
	buffer[bytes_read] = '\0';
	Client &client = _clients[fd];
	client.addToBuffer(std::string(buffer, bytes_read));

	// On traite les commandes complètes (finies par \r\n)
	std::string& clientBuffer = client.getBuffer();
	size_t pos;
	while ((pos = clientBuffer.find("\r\n")) != std::string::npos) {
		std::string line = clientBuffer.substr(0, pos);
		clientBuffer.erase(0, pos + 2);
		handleCommand(client, line);
	}
}

// Déconnecte proprement un client (ferme la socket, retire des listes)
void Server::disconnectClient(int fd){
	// Retire le client de la map (à améliorer : le retirer aussi des channels)
	std::map<int, Client>::iterator clientIt = _clients.find(fd);
	if (clientIt != _clients.end()){
		_clients.erase(clientIt);
	}

	// Ferme la socket du client
	close(fd);

	// Retire le fd de la liste surveillée par poll()
	for (std::vector<pollfd>::iterator pIt = _pfds.begin(); pIt != _pfds.end(); ++pIt){
		if (pIt->fd == fd){
			_pfds.erase(pIt);
			break;
		}
	}

	std::ostringstream msg;
	msg << "Client (fd=" << fd << ") déconnecté.";
	Logger::log(INFO, msg.str());
}

static std::string& trimLeft(std::string& s){
    size_t startpos = s.find_first_not_of(" \t\r\n");
    if (std::string::npos != startpos)
        s = s.substr(startpos);
    return s;
}

bool Server::handleCommand(Client &client, std::string &line){
	trimLeft(line);

	if (line.empty()) {return false;}

	std::istringstream iss(line);
	std::string cmd;
	iss >> cmd;

	std::string rest;
    std::getline(iss, rest);
    rest = trimLeft(rest);

	std::vector<std::string> args;
	std::string token;
	std::istringstream argStream(rest);

	while (argStream >> token){
		if (!token.empty() && token[0] == ':'){
			std::string new_token = token.substr(1);
			std::string tmp;
			std::getline(argStream, tmp);
			if (!tmp.empty())
				new_token += tmp;
			args.push_back(new_token);
			break;
		}
		args.push_back(token);
	}

	for (size_t i = 0; i < cmd.size(); i++) {cmd[i] = std::toupper(cmd[i]);}

	std::map<std::string, ICmd*>::iterator it = _commands.find(cmd);
	if (it != _commands.end()){ // cas ou la commade est connu
		// ajouter verif que le client existe 
        it->second->execute(*this, client, args);
	}
	else { // Commande Inconnue !
        std::ostringstream msg;
        msg << "Commande inconnue '" << cmd << "' par '" << client.getNickname() << "'";
        Logger::log(INFO, msg.str());
        sendReply(client, "421 " + client.getNickname() + " " + cmd + " :Unknown command");
	}
	return false;
}

void Server::sendReply(const Client& client, const std::string& message){
	std::string fullMessage = ":" + _serverName + " " + message + "\r\n";

	size_t total = 0;
	while (total < fullMessage.size()){
		ssize_t bytes = send(client.getFd(), fullMessage.c_str() + total, fullMessage.length() - total, 0);
		if (bytes < 0){
			std::ostringstream msg;
			msg << "Erreur sur send() vers le client " << client.getNickname() << "(fd: " << client.getFd() << ")";
			Logger::log(ERROR, msg.str());
			break;
		}
		total += bytes;
	}
	std::ostringstream msg;
	msg << "S--> [" << client.getNickname() << "]" << message;
	Logger::log(DEBUG, msg.str());
}

void Server::run(){
	time_t start = time(NULL);

	// On prépare la structure pollfd pour la socket d'écoute (serveur)
	pollfd p;
	p.fd = _listenfd;         // Le file descriptor de la socket d'écoute
	p.events = POLLIN;        // On veut être notifié quand il y a une connexion entrante (POLLIN = données à lire)
	p.revents = 0;            // Champ réservé pour les événements détectés par poll (initialisé à 0)
	_pfds.push_back(p);       // On ajoute cette socket à la liste des sockets surveillées par poll()

	//BOT
	// int fd_bot = acceptNewClient();
	// std::cout << "TEST:BOT connected, fd: " << fd_bot << std::endl;
	Bot iRoBot;
	iRoBot.setup(this->_port, this->_password);

	while(Server::g_running)
	{
		//std::cout << "Recoit signal 1" << std::endl;
		if ((time(NULL) - start) > 10){
			iRoBot.sendMessage();
			start = time(NULL);
		}

		// poll() surveille toutes les sockets (écoute + clients)
		int activity = poll(&_pfds[0], _pfds.size(), 500);
		if (activity < 0 && Server::g_running){
			throw PollError();
		}
		//std::cout << "Recoit signal 2" << std::endl;

		// Parcourt toutes les sockets surveillées
		for (size_t i = 0; i < _pfds.size(); i++){
			// Si une socket a des données à lire
			if (_pfds[i].revents & POLLIN){
				// Si c'est la socket d'écoute : nouveau client
				if(_pfds[i].fd == _listenfd)
					acceptNewClient();
				// Sinon : données d'un client existant
				else{
					start = time(NULL);													//TIME
					std::cout << "Recoit signal 3" << " time: " << start << std::endl;
					receiveFromClient(_pfds[i].fd);
				}
			}
		}
	}
}