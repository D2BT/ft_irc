#pragma once

#include "ICmd.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"

class JoinCmd: public ICmd{
    public:

        JoinCmd();
        virtual ~JoinCmd();

        void execute(Server& server, Client &client, const std::vector<std::string>& args);
    private:
        void joinChannel(Server &server, Client &client, std::string channelName, std::string key);
};