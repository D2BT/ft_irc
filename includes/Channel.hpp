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
        bool        _invitOnly;
        bool        _restrictedTopic;
        int         _userLimit;
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

        std::string const  &getChannelName() const;
        std::string const  &getChannelTopic() const;
        std::string const  &getPasswordChannel() const;
        std::string const  &getUserList() const;

        void                setModeInvite(bool val);
        void                setModeTopic(bool val);
        void                setUserLimit(int val);
        void                setPassword(std::string password);

        bool                getModeInvite() const;
        bool                getModeTopic() const;
        int                 getUserLimit() const;

        bool                isInChannel(Client &user) const;

        void                setChannelTopic(Client *user, std::string topic);

        void                broadcastMessage(Server &server, std::string const &message);

        class NotAdmin : public std::exception {
            public :
                const char *what() const throw();
        };
};