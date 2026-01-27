#pragma once

#include "ICmd.hpp"
#include "Server.hpp"
#include "Client.hpp"

class InviteCmd: public ICmd{
    public:

        InviteCmd();
        virtual ~InviteCmd();

        void execute(Server& server, Client& client, const std::vector<std::string>& args);
};