#pragma once

#include "ICmd.hpp"
#include "Server.hpp"
#include "Client.hpp"

class QuitCmd: public ICmd{
    public:

        QuitCmd();
    	virtual ~QuitCmd();

        void execute(Server& server, Client& client, const std::vector<std::string>& args);
};