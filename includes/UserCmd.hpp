#pragma once

#include "ICmd.hpp"
#include "Server.hpp"
#include "Client.hpp"

class UserCmd: public ICmd{
    public:

        UserCmd();
        virtual ~UserCmd();

        void execute(Server& server, Client& client, const std::vector<std::string>& args);
};