#include "../includes/QuitCmd.hpp"

QuitCmd::QuitCmd(){}

QuitCmd::~QuitCmd(){}

void QuitCmd::execute(Server& server, Client& client, const std::vector<std::string>& args){
	std::string reason;
	if (args.size() >= 1)
		reason = args[0];
	else
		reason = "Client Quit";
	
	std::string quitMessage = ":" + client.getNickname() + "!" + client.getUsername() + "@" + server.getServerName() + " QUIT :" + reason;
	server.notifyClientQuit(client, quitMessage);
	server.disconnectClient(client.getFd());
}