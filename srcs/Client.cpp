#include "../includes/Client.hpp"

// Constructeur par défaut
Client::Client(){}

// Constructeur avec le file descriptor (fd) du client
// fd = identifiant de la connexion réseau (socket)
Client::Client(int fd): _fd(fd), _nickname(""), _username(""), _realname(""), _isRegistered(false), _isAuthenticated(false){}

// Destructeur
Client::~Client(){}

// Modifie le file descriptor du client
void Client::setFd(int fd){
	this->_fd = fd;
}

// Définit le pseudo du client
void Client::setNickname(const std::string(nickname)){
	this->_nickname = nickname;
}

// Définit le nom d'utilisateur (utilisé lors de l'authentification IRC)
void Client::setUsername(const std::string(username)){
	this->_username = username;
}

// Définit le vrai nom du client (optionnel, info IRC)
void Client::setRealname(const std::string(realname)){
	this->_realname = realname;
}

// Indique si le client est inscrit (a passé toutes les étapes d'inscription IRC)
void Client::setRegistered(bool status){
	this->_isRegistered = status;
}

// Indique si le client est authentifié (mot de passe correct, etc.)
void Client::setAuthenticated(bool status){
	this->_isAuthenticated = status;
}

// Récupère le file descriptor du client
int Client::getFd() const{
	return this->_fd;
}

// Récupère le pseudo du client
const std::string& Client::getNickname() const{
	return this->_nickname;
}

// Récupère le nom d'utilisateur
const std::string& Client::getUsername() const{
	return this->_username;
}

// Récupère le vrai nom du client
const std::string& Client::getRealname() const{
	return this->_realname;
}

// Vérifie si le client est inscrit
bool Client::getRegistered() const{
	return this->_isRegistered;
}

// Vérifie si le client est authentifié
bool Client::getAuthenticated() const{
	return this->_isAuthenticated;
}

// Récupère le buffer associé au client (sert à stocker les messages reçus avant traitement)
std::string& Client::getBuffer(){
	return this->_buffer;
}

void Client::addToBuffer(const std::string& data){
	this->_buffer += data;
}