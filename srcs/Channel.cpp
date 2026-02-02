#include "../includes/Channel.hpp"

Channel::Channel() : _name("Default"), _topic("Default") {}

Channel::Channel(std::string name) : _name(name), _topic() {}

void Channel::addClient(Client *user) {
    if (std::find(_users.begin(), _users.end(), user) != _users.end()){
        std::cout << "User " << user->getNickname() << " is already in the channel." << std::endl;
        return;
    }
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