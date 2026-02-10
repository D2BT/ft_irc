#include "../includes/Channel.hpp"
#include "../includes/Server.hpp"

Channel::Channel() : _name("Default"), _topic("Default"), _password(""), _invitOnly(false), _restrictedTopic(true), _userLimit(0) {}

Channel::Channel(std::string name, std::string topic, std::string password) : _name(name), _topic(topic), _password(password), _invitOnly(false), _restrictedTopic(true), _userLimit(0) {}

Channel::Channel(Channel const &src){
    _name = src._name;
    _topic = src._topic;
    _password = src._password;
    _users = src._users;
    _admin = src._admin;
    _invitOnly = src._invitOnly;
    _invits = src._invits;
    _restrictedTopic = src._restrictedTopic;
    _userLimit = src._userLimit;
}

Channel &Channel::operator=(Channel const &rhs){
    if (this != &rhs){
        _name = rhs._name;
        _topic = rhs._topic;
        _password = rhs._password;
        _users = rhs._users;
        _admin = rhs._admin;
        _invitOnly = rhs._invitOnly;
        _invits = rhs._invits;
        _userLimit = rhs._userLimit;
        _restrictedTopic = rhs._restrictedTopic;
    }
    return *this;
}

Channel::~Channel() {}

void Channel::addClient(Client *user) {
    if (std::find(_users.begin(), _users.end(), user) != _users.end()){
        std::cout << "User " << user->getNickname() << " is already in the channel." << std::endl;
        return;
    }
    _users.push_back(user);
    return;
}

void Channel::addAdmin(Client *user) {
    std::vector<Client *>::const_iterator isAdmin = std::find(_admin.begin(), _admin.end(), user);
    std::vector<Client *>::const_iterator isInChan = std::find(_users.begin(), _users.end(), user);
    if (isAdmin == _admin.end())
        _admin.push_back(user);
    if (isInChan == _users.end())
        _users.push_back(user);
    return;
}

void Channel::removeClient(Client *user) {
    std::vector<Client *>::iterator here = std::find(_users.begin(), _users.end(), user);
    if (here == _users.end()){
        std::cout << "No such user in the channel." << std::endl;
        return;
    }
    _users.erase(here);
    std::vector<Client *>::iterator hereAdm = std::find(_admin.begin(), _admin.end(), user);
    if (hereAdm != _admin.end())
        _admin.erase(hereAdm);
    return; 
}

std::string const &Channel::getChannelName() const {
    return this->_name;
}

std::string const &Channel::getChannelTopic() const {
    return this->_topic;
}

std::string const &Channel::getPasswordChannel() const {
    return this->_password;
}

void Channel::setChannelTopic(Client *user, std::string newTopic) {
    std::vector<Client *>::const_iterator isAdmin = std::find(_admin.begin(), _admin.end(), user);
    if (isAdmin == _admin.end()) {
        throw NotAdmin();
    }
    _topic = newTopic;
}

bool Channel::isInChannel(Client *user) const {
    if (!user)
        return false;

    for (std::vector<Client *>::const_iterator it = _users.begin(); it != _users.end(); ++it) {
        if (*it == user)
            return true;
    }
    return false;
}

bool Channel::isInvited(Client &user) const{
    for (std::vector<Client *>::const_iterator it = _invits.begin(); it != _invits.end(); it++){
        if (*it == &user)
            return true;
    }
    return false;
}

bool Channel::isOperator(Client *user) const {
    for (std::vector<Client *>::const_iterator it = _admin.begin(); it != _admin.end(); it++){
        if (*it == user)
            return true;
    }
    return false;
}

int Channel::getNumberOfUsers() const {
    return static_cast<int>(_users.size());
}

bool Channel::getModeInvite() const {
    return _invitOnly;
}

bool Channel::getModeTopic() const {
    return _restrictedTopic;
}

int Channel::getUserLimit() const {
    return _userLimit;
}

void Channel::setModeInvite(bool val) {
    _invitOnly = val;
}

void Channel::setModeTopic(bool val) {
    _restrictedTopic = val;
}

void Channel::setUserLimit(int max) {
    _userLimit = max;
}

void Channel::setPassword(std::string password) {
    _password = password;
}



void Channel::broadcastMessage(Server &server, std::string const &message){
    for (size_t i = 0; i < _users.size(); i++)
        server.sendToClient(*_users[i], message);
}

void Channel::broadcastToOther(Server &server, std::string const &message, Client &client){
    for (std::vector<Client *>::iterator it = _users.begin(); it != _users.end(); it++){
        if (*it && *it != &client)
            server.sendToClient(**it, message);
    }
}

std::string const Channel::getUserList() const {
    std::string userList;
    for (size_t i = 0; i < _users.size(); i++){
        Client *current = _users[i];
        std::vector<Client *>::const_iterator itAdmin = std::find(_admin.begin(), _admin.end(), current);
        if (itAdmin != _admin.end())
            userList += '@';
        userList += current->getNickname();
        if (i < _users.size() - 1)
            userList += " ";
    }
    return userList;
}

const char *Channel::NotAdmin::what() const throw(){
    return "Not a Channel Operator.";
}

std::vector<Client *> Channel::getAdmin() const{
    return _admin;
}