#pragma once

#include "ICmd.hpp"
#include "Server.hpp"
#include "Client.hpp"

class PrivmsgCmd: public ICmd{
    public:

        PrivmsgCmd();
        virtual ~PrivmsgCmd();

        void execute(Server& server, Client& client, const std::vector<std::string>& args);
};