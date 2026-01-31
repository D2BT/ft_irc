#include "../includes/Server.hpp"
#include "../includes/Exception.hpp"
#include "../includes/Logger.hpp"

// Variable globale pour savoir si le serveur doit continuer à tourner
volatile bool Server::g_running = true;

// Constructeur du serveur IRC
// Initialise le port, le mot de passe et les commandes IRC disponibles
Server::Server(int port, std::string password): _port(port), _listenfd(-1), _password(password){
	try {
		// Création des objets pour chaque commande IRC prise en charge
		_commands["NICK"] = new NickCommand();
		_commands["USER"] = new UserCommand();
		_commands["PASS"] = new PassCommand();
		_commands["PING"] = new PingCommand();
		_commands["MODE"] = new ModeCommand();
		_commands["JOIN"] = new JoinCommand();
		_commands["PRIVMSG"] = new PrivmsgCommand();
		_commands["PART"] = new PartCommand();
		_commands["KICK"] = new KickCommand();
		_commands["QUIT"] = new QuitCommand();
		_commands["TOPIC"] = new TopicCommand();
		_commands["INVITE"] = new InviteCommand();
	}
	catch (std::bad_alloc & e) {
		// Libère la mémoire en cas d'erreur d'allocation
		for(std::map<std::string, ICommand*>::iterator it = _commands.begin(); it != _commands.end(); it++) {
			delete it->second;
		}
		_commands.clear();
		throw;
	}
}

// Destructeur du serveur IRC
// Libère la mémoire des commandes et des clients, ferme les sockets
Server::~Server(){
	// Libère les commandes IRC
	std::map<std::string, ICommand*>::iterator ite = _commands.begin();
	for (; ite != _commands.end(); ++ite)
		delete ite->second;
	_commands.clear();

	// Libère les clients connectés
	for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it){
		close (it->first);
		delete it->second;
	}
	_clients.clear();

	// Ferme la socket d'écoute si elle existe
	if (_listenFd != -1)
		close (_listenFd);
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




	return true;
}

// Boucle principale du serveur : surveille toutes les sockets avec poll()
void Server::run(){
	// On prépare la structure pollfd pour la socket d'écoute (serveur)
	pollfd p;
	p.fd = _listenfd;         // Le file descriptor de la socket d'écoute
	p.events = POLLIN;        // On veut être notifié quand il y a une connexion entrante (POLLIN = données à lire)
	p.revents = 0;            // Champ réservé pour les événements détectés par poll (initialisé à 0)
	_pfds.push_back(p);       // On ajoute cette socket à la liste des sockets surveillées par poll()

	while(Server::g_running)
	{
		// poll() surveille toutes les sockets (écoute + clients)
		int activity = poll(&_pfds[0], _pfds.size(), 1000);
		if (activity < 0 && Server::g_running){
			throw PollError();
		}

		// Parcourt toutes les sockets surveillées
		for (size_t i = 0; i < _pfds.size(); i++){
			// Si une socket a des données à lire
			if (_pfds[i].revents & POLLIN){
				// Si c'est la socket d'écoute : nouveau client
				if(_pfds[i].fd == _listenfd)
					acceptNewClient();
				// Sinon : données d'un client existant
				else
					receiveFromClient(_pfds[i].fd);
			}
		}
	}
}