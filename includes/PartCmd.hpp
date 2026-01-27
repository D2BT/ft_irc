#pragma once

#include "ICmd.hpp"
#include "Server.hpp"
#include "Client.hpp"

class PartCmd: public ICmd{
    public:

        PartCmd();
        virtual ~PartCmd();

        void execute(Server& server, Client& client, const std::vector<std::string>& args);
};