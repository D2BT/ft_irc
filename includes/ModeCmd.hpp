#pragma once

#include "ICmd.hpp"
#include "Server.hpp"
#include "Client.hpp"

class ModeCmd: public ICmd{
    public:

        ModeCmd();
        virtual ~ModeCmd();

        void execute(Server& server, Client& client, const std::vector<std::string>& args);
};