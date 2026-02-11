#include "../includes/PassCmd.hpp"

PassCmd::PassCmd(){}

PassCmd::~PassCmd(){}

void PassCmd::execute(Server& server, Client& client, const std::vector<std::string>& args){
	if (client.isRegistered()){
        server.sendReply(client, ":" + server.getServerName() + " 462 " + client.getNickname() + " :You may not reregister");
        return;
    }

	if (args.empty() || args[0].empty()){
		 server.sendReply(client, ":" + server.getServerName() + " 461 " + client.getNickname() + " PASS :Not enough parameters");
        return;
	}

	if (args[0] == server.getPassword())
		client.setAuthenticated(true);
	else
		server.sendReply(client, ":" + server.getServerName() + " 464 " + client.getNickname() + " :Password incorrect");
}