#pragma once

#include "ICmd.hpp"
#include "Server.hpp"
#include "Client.hpp"

class PassCmd: public ICmd{
    public:

        PassCmd();
        virtual ~PassCmd();

        void execute(Server& server, Client& client, const std::vector<std::string>& args);
};