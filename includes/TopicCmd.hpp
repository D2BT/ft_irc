#pragma once

#include "ICmd.hpp"
#include "Server.hpp"
#include "Client.hpp"

class TopicCmd: public ICmd{
    public:

        TopicCmd();
        virtual ~TopicCmd();

        void execute(Server& server, Client& client, const std::vector<std::string>& args);
};