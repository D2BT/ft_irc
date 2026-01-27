#pragma once

#include "ICmd.hpp"
#include "Server.hpp"
#include "Client.hpp"

class KickCmd: public ICmd{
    public:

        KickCmd();
        virtual ~KickCmd();

        void execute(Server& server, Client& client, const std::vector<std::string>& args);
};