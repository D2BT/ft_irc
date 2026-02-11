#include "../includes/NickCmd.hpp"
#include "../includes/Channel.hpp"

NickCmd::NickCmd(){}

NickCmd::~NickCmd(){}

static bool IsValidNick(std::string &str){
	for(size_t i = 0; i < str.size(); i++){
		char c = str[i];
		if (!(isalnum(static_cast<unsigned char>(c)) || c == '_' || c == '[' || c == '\\' || c ==  ']' || c == '^' || c == '`' || c == '{'  || c == '|' || c == '}'))
			return false;
	}
	return true;
}


void NickCmd::execute(Server& server, Client& client, const std::vector<std::string>& args){
	if (args.empty()){
		server.sendReply(client, ":" + server.getServerName() + " 431 " + client.getNickname() + " :No nickname given");
        return;
	}
	if (args.size() > 1){
		server.sendReply(client, ":" + server.getServerName() + " 461 " + client.getNickname() + " :Too many arguments");
        return;
	}
	if (args[0].size() > 9){
		server.sendReply(client, ":" + server.getServerName() + " 432 " + client.getNickname() + " :Erroneus nickname");
        return;
	}
	std::string newNick = args[0];
	if (!IsValidNick(newNick)){
		server.sendReply(client, ":" + server.getServerName() + " 432 " + client.getNickname() + " :Erroneus nickname");
        return;
	}
	const std::string oldNick = client.getNickname();
	if (oldNick == newNick)
		return;

	Client* existingClient = server.getClientByNick(newNick);
	if (existingClient != NULL){
		std::string nickToUse;
		if (oldNick.empty())
			nickToUse = "*";
		else;
			nickToUse = oldNick;
		server.sendReply(client, ":" + server.getServerName() + " 433 " + nickToUse + " " + newNick + " :Nickname is already in use");
		return;
	}

	std::string nickChangeMessage = ":" + oldNick + "!" + client.getUsername() + "@" + server.getServerName() + " NICK :" + newNick;
	std::map<std::string, Channel *> channels = server.getChannels();
    for (std::map<std::string, Channel*>::iterator it = channels.begin(); it != channels.end(); ++it){
        Channel* currentChannel = it->second;
        if (currentChannel->isInChannel(&client))
            currentChannel->broadcastMessage(server, nickChangeMessage);
    }

	server.sendToClient(client, nickChangeMessage);
	client.setNickname(newNick);

	if (!client.isRegistered() && !client.getUsername().empty()){
        client.setRegistered(true);
        server.sendReply(client, ":" + server.getServerName() + " 001 " + newNick + " :Welcome to my IRC Network, " + newNick);
		server.sendReply(client, ":" + server.getServerName() + " 002 " + newNick + " :Your host is " + server.getServerName() + " , running version 1.0");
		server.sendReply(client, ":" + server.getServerName() + " 003 " + newNick + " :This server was created " + server.getServerCreationDateRPL003());
		server.sendReply(client, ":" + server.getServerName() + " 004 " + newNick + " " + server.getServerName() + " 1.0 none itkol");
    }
}