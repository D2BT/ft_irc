# pragma once

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "../includes/Client.hpp"

class Channel {
    private :
        std::string _name;
        std::string _topic;
        std::vector<Client *> _admin;
        std::vector<Client *> _users;

        Channel();
        
    public :
        Channel(std::string name);
        Channel(Channel const &src);
        ~Channel();
        
        void                addAdmin(Client *user);
        void                addClient(Client *user);
        void                removeClient(Client *user);

        const std::string   &getChannelName() const;
        const std::string   &getChannelTopic() const;
        void                setChannelTopic(std::string topic);

};