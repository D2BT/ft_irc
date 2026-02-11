#include "../includes/PingCmd.hpp"

PingCmd::PingCmd(){}

PingCmd::~PingCmd(){}

void PingCmd::execute(Server& server, Client& client, const std::vector<std::string>& args){
	if (args.empty()){
		server.sendReply(client, ":" + server.getServerName() + " 409 " + client.getNickname() + " :No origin specified");
		return;
	}
	server.sendReply(client, ":" + server.getServerName() + " PONG " + server.getServerName() + " :" + args[0]);
}