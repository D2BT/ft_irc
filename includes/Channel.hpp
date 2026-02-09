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
        std::string _password;
        bool        _invit_only;
        unsigned int _limit;
        std::vector<Client *> _admin;
        std::vector<Client *> _users;
        std::vector<Client *> _invits;

        Channel();
        
    public :
        Channel(std::string name, std::string topic, std::string password);
        Channel(Channel const &src);
        Channel &operator=(Channel const &rhs);
        ~Channel();
        
        void                addAdmin(Client *user);
        void                addClient(Client *user);
        void                removeClient(Client *user);

        std::string const &getChannelName() const;
        std::string const &getChannelTopic() const;
        std::string const &getPasswordChannel() const;

        void                setChannelTopic(Client *user, std::string topic);

        class NotAdmin : public std::exception {
            public :
                const char *what() const throw();
        };
};