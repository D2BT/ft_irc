#pragma once

#include "ICmd.hpp"
#include "Server.hpp"
#include "Client.hpp"

class NickCmd: public ICmd{
    public:

        NickCmd();
        virtual ~NickCmd();

        void execute(Server& server, Client& client, const std::vector<std::string>& args);
};