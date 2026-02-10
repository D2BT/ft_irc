#include "../includes/PrivmsgCmd.hpp"

PrivmsgCmd::PrivmsgCmd(){}

PrivmsgCmd::~PrivmsgCmd(){}

void PrivmsgCmd::execute(Server& server, Client& client, const std::vector<std::string>& args){
	if (!client.isRegistered()){
		server.sendReply(client, "451 " + client.getNickname() + " :You have not register");
		return;
	}
	
	if (args.size() == 1){
		server.sendReply(client, "412 :No text to send");
		return;
	}
	
	if (args.size() != 2){
		server.sendReply(client, "461 PRIVMSG :Not enough parameters");
		return;
	}

	if (args[0][0] == '#'){ // cas ou c'est un channel

	}
	else{

	}
}