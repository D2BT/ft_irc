#pragma once

#include "ICmd.hpp"
#include "Server.hpp"
#include "Client.hpp"

class JoinCmd: public ICmd{
    public:

        JoinCmd();
        virtual ~JoinCmd();

        void execute(Server& server, Client& client, const std::vector<std::string>& args);
};