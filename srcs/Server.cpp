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
#include "../includes/Channel.hpp"

volatile bool Server::g_running = true;

Server::Server(int port, std::string password): _port(port), _listenfd(-1), _serverName("my_irc_server"), _password(password){
    try {
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
        /*_commands["INVITE"] = new InviteCmd(); */
    }
    catch (std::bad_alloc & e) {
        for(std::map<std::string, ICmd*>::iterator it = _commands.begin(); it != _commands.end(); it++) {
            delete it->second;
        }
        _commands.clear();
        throw;
    }
}

Server::~Server(){
	std::map<std::string, ICmd*>::iterator ite = _commands.begin();
    for (; ite != _commands.end(); ++ite)
        delete ite->second;
    _commands.clear();

    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it){
        close (it->first);
		delete it->second;
    }
    _clients.clear();

	for (std::map<std::string, Channel *>::iterator it = _channels.begin(); it != _channels.end(); it++){
		delete it->second;
	}
	_channels.clear();
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

void Server::setup(){
	_listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (_listenfd < 0)
		throw SocketError();
	int opt = 1;
	if (setsockopt(_listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		throw SocketError();
	if (fcntl(_listenfd, F_SETFL, O_NONBLOCK) < 0)
		throw SocketError();

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

	try {
		_clients[client_fd] = new Client(client_fd);
    }
    catch (const std::bad_alloc& e) {
        Logger::log(ERROR, "Impossible d'allouer le client - new error");
        throw;
    }

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
    
	std::map<int, Client*>::iterator it = _clients.find(fd);
	if (it == _clients.end())
    	return;

	Client* client = it->second;
	client->addToBuffer(std::string(buffer, bytes_read));

	std::string& clientBuffer = client->getBuffer();
	size_t pos;

	while ((pos = clientBuffer.find("\r\n")) != std::string::npos) {
		std::string line = clientBuffer.substr(0, pos);
		clientBuffer.erase(0, pos + 2);
		bool clientDisconnected = handleCommand(*client, line);
        if (clientDisconnected) {
            return;
        }
	}
}

void Server::disconnectClient(int fd){
	std::map<int, Client*>::iterator clientIt = _clients.find(fd);
	if (clientIt != _clients.end()){ //par la suite supprimer le client de tout les channels dans lequels il est 
		delete clientIt->second;
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

void Server::notifyClientQuit(Client& client, const std::string& message){
    for (std::map<std::string, Channel*>::const_iterator it = _channels.begin(); it != _channels.end(); ++it){
        if (it->second->isInChannel(&client)){
            it->second->broadcastMessage(*this, message);
        }
    }
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
	if (it != _commands.end()){
		std::ostringstream msg;
        std::string nick;
        if (client.getNickname().empty()){
            std::ostringstream oss_nick;
            oss_nick << "fd=" << client.getFd();
            nick = oss_nick.str();
        }
        else
            nick = client.getNickname();
        msg << "Exécution de " << cmd << " par '" << nick << "'";
        Logger::log(CMD, msg.str());

		int clientFd = client.getFd();
        it->second->execute(*this, client, args);
		if (_clients.find(clientFd) == _clients.end()) {
            return true;
        }
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

void Server::relayMessage(Client& client, const std::string& message){
    std::string fullMessage = message + "\r\n";

    if (send(client.getFd(), fullMessage.c_str(), fullMessage.length(), 0) < 0){
        std::ostringstream msg;
        msg << "Erreur sur send() lors du relai a " << client.getNickname();
        Logger::log(ERROR,msg.str());
    }
    std::ostringstream msg;
    msg << "S--> Relaye a [" << client.getNickname() << "]" << message;
    Logger::log(DEBUG, msg.str());
}

Client* Server::getClientByNick(const std::string& nick) {
    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if (it->second->getNickname() == nick)
            return (it->second);
    }
    return NULL;
}

const std::string& Server::getPassword() const{
	return _password;
}

const std::string& Server::getServerName() const{
	return _serverName;
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

void Server::sendToClient(Client& client, const std::string& message){
    std::string fullMessage = message + "\r\n";

    size_t total = 0;
    while (total < fullMessage.size()) {
        ssize_t bytes = send(client.getFd(), fullMessage.c_str() + total, fullMessage.size() - total, 0);
        if (bytes < 0) {
			std::ostringstream msg;
			msg << "Erreur: impossible d'envoyer des données au client (fd: " << client.getFd() << ").";
			Logger::log(DEBUG, msg.str());
            break;
        }
        total += bytes;
    }
    std::ostringstream msg;
    msg << "S--> [" << client.getNickname() << "] " << message;
    Logger::log(DEBUG, msg.str());
}

Channel *Server::getChannel(std::string const &channelName) const {
	std::map<std::string, Channel *>::const_iterator it = _channels.find(channelName);
	if (it == _channels.end())
		return NULL;
	return it->second;
}

Channel *Server::createChannel(std::string channelName, std::string password){
	if (getChannel(channelName) == NULL){
		Channel *newChannel = new Channel(channelName, "", password);
		_channels[channelName] = newChannel;
		return newChannel;
	}
	return _channels[channelName];
}


std::map<std::string, Channel *> Server::getChannels() const {
	return _channels;
}
