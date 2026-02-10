#include "../includes/UserCmd.hpp"

UserCmd::UserCmd(){}

UserCmd::~UserCmd(){}

void UserCmd::execute(Server& server, Client& client, const std::vector<std::string>& args){
	if (client.isRegistered()){
        server.sendReply(client, ":" + server.getServerName() + " 462 " + client.getNickname() + " :You may not reregister");
        return;
    }

	if (!client.isAuthenticated()){
        server.sendReply(client, ":" + server.getServerName() + " 451 " + client.getNickname() + " :You have not registered");
        return;
    }

	if (args.size() < 4){
        server.sendReply(client, ":" + server.getServerName() + " 461 " + client.getNickname() + " USER :Not enough parameters");
        return;
    }

	client.setUsername(args[0]);
    client.setRealname(args[3]);

    if (!client.getNickname().empty()){
        client.setRegistered(true);
        server.sendReply(client, ":" + server.getServerName() + " 001 " + client.getNickname() + " :Welcome to the IRC Network, " + client.getNickname());
    }
}