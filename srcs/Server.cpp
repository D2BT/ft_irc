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
void Server::setup(){
	// Crée la socket d'écoute (TCP)
	_listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (_listenfd < 0)
		throw SocketError();
	// Permet de réutiliser l'adresse rapidement après un crash
	int opt = 1;
	if (setsockopt(_listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		throw SocketError();
	// Met la socket en mode non-bloquant (important pour poll/select)
	if (fcntl(_listenfd, F_SETFL, O_NONBLOCK) < 0)
		throw SocketError();

	// Prépare la structure d'adresse pour l'écoute sur toutes les interfaces
	sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(_port);
	
	if (bind(_listenfd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
		throw SocketError();
	if (listen(_listenfd, SOMAXCONN) < 0)
		throw SocketError();
	std::cout << "IRC server listening on port " << _port << std::endl;
}

int  Server::acceptNewClient(){
	sockaddr_in clientAddr;
	socklen_t clientLen = sizeof(clientAddr);

	int client_fd = accept(_listenfd, (sockaddr*)&clientAddr, &clientLen);
	if (client_fd < 0){
		Logger::log(ERROR, "Erreur lors de l'acceptation du client");
		return -1;
	}

	fcntl(client_fd, F_SETFL, O_NONBLOCK);

    _clients[client_fd] = Client(client_fd);

	pollfd newFd;
	newFd.fd = client_fd;
	newFd.events = POLLIN;
	newFd.revents = 0;
	_pfds.push_back(newFd);

	char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &clientAddr.sin_addr, ip, INET_ADDRSTRLEN);
    int port = ntohs(clientAddr.sin_port);

    std::ostringstream msg;
    msg << "Nouveau client connecté (fd=" << client_fd << ", " << ip << ":" << port << ")";
    Logger::log(INFO, msg.str());

    return client_fd;
}

void Server::receiveFromClient(int fd){
	char buffer[512];
    size_t bytes_read = recv(fd, buffer, sizeof(buffer) - 1, 0);

	if (bytes_read == 0){
        std::ostringstream msg;
        msg << "Client (fd=" << fd << ") a fermé la connexion.";
        Logger::log(INFO, msg.str());
        disconnectClient(fd);
        return;
    }
	else if (bytes_read < 0){
		Logger::log(ERROR, "Erreur recv()");
        disconnectClient(fd);
	}

	buffer[bytes_read] = '\0';
    Client &client = _clients[fd];
	client.addToBuffer(std::string(buffer, bytes_read));

	std::string& clientBuffer = client.getBuffer();
    size_t pos;

	while ((pos = clientBuffer.find("\r\n")) != std::string::npos) {
		std::string line = clientBuffer.substr(0, pos);
		clientBuffer.erase(0, pos + 2);
		handleCommand(client, line);
	}
}

void Server::disconnectClient(int fd){
	std::map<int, Client>::iterator clientIt = _clients.find(fd);
	if (clientIt != _clients.end()){ //par la suite supprimer le client de tout les channels dans lequels il est 
		_clients.erase(clientIt);
	}

	close(fd);

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

void Server::run(){
	pollfd p;
	p.fd = _listenfd;
	p.events = POLLIN;
	p.revents = 0;
	_pfds.push_back(p);
	while(Server::g_running)
	{
		int activity = poll(&_pfds[0], _pfds.size(), 1000);
		if (activity < 0 && Server::g_running){
			throw PollError();
		}

		for (size_t i = 0; i < _pfds.size(); i++){
			if (_pfds[i].revents & POLLIN){
				if(_pfds[i].fd == _listenfd)
					acceptNewClient();
				else
					receiveFromClient(_pfds[i].fd);
			}
		}
	}
}