#pragma once

#include "ICmd.hpp"
#include "Server.hpp"
#include "Client.hpp"

class PingCmd: public ICmd{
    public:

        PingCmd();
        virtual ~PingCmd();

        void execute(Server& server, Client& client, const std::vector<std::string>& args);
};