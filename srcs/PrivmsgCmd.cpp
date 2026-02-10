#include "../includes/PrivmsgCmd.hpp"
#include <algorithm>
#include <cctype>

PrivmsgCmd::PrivmsgCmd(){}

PrivmsgCmd::~PrivmsgCmd(){}

std::string toLower(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

bool isPolite(std::string &message){
	if (toLower(message).find(toLower("bonjour")) != std::string::npos) {return true;}
	if (toLower(message).find(toLower("bonsoir")) != std::string::npos) {return true;}
	if (toLower(message).find(toLower("salut")) != std::string::npos) {return true;}
	if (toLower(message).find(toLower("coucou")) != std::string::npos) {return true;}
	if (toLower(message).find(toLower("hello")) != std::string::npos) {return true;}
	if (toLower(message).find(toLower("hi")) != std::string::npos) {return true;}
	if (toLower(message).find(toLower("good morning")) != std::string::npos) {return true;}
	if (toLower(message).find(toLower("good afternoon")) != std::string::npos) {return true;}
	if (toLower(message).find(toLower("hola")) != std::string::npos) {return true;}
	if (toLower(message).find(toLower("biere")) != std::string::npos) {return true;}
	if (toLower(message).find(toLower("guten tag")) != std::string::npos) {return true;}
	if (toLower(message).find(toLower("boa tarde")) != std::string::npos) {return true;}
	if (toLower(message).find(toLower("buongiorno")) != std::string::npos) {return true;}
	return false;
}

void PrivmsgCmd::execute(Server& server, Client& client, const std::vector<std::string>& args){
	if (!client.isRegistered()){
		server.sendReply(client, ":" + server.getServerName() + " 451 " + client.getNickname() + " :You have not register");
		return;
	}
	
	if(args.empty()){
		server.sendReply(client, ":" + server.getServerName() + " 411 " + client.getNickname() + " :No recipient given (PRIVMSG)");
		return;
	}

	if (args.size() != 2){
		server.sendReply(client, ":" + server.getServerName() + " 461 " + client.getNickname() + " PRIVMSG :Not enough parameters");
        return;
	}

	if (args.size() == 1 || args[1].empty()){
		server.sendReply(client, ":" + server.getServerName() + " 412 " + client.getNickname() + " PRIVMSG :No text to send");
        return;
	}

	std::string target = args[0];
	std::string message = args[1];

	std::string full = ":" + client.getNickname() + "!" +
                       client.getUsername() + "@" +
                       server.getServerName() +
                       " PRIVMSG " + target + " :" + message;

	if (target[0] == '#'){
		Channel* channel = server.getChannel(target);
        if (!channel) {
			server.sendReply(client, ":" + server.getServerName() + " 403 " + client.getNickname() + " " + target + " :No such channel");
            return;
        }
        if (!channel->isInChannel(&client)) {
			server.sendReply(client, ":" + server.getServerName() + " 404 " + client.getNickname() + " " + target + " :Cannot send to channel");
            return;
        }
		if (isPolite(message)){
			client.saidHello();
		}
		channel->broadcastToOther(server, full, client);
	}
	else{
		Client* targetClient = server.getClientByNick(target);
		if (targetClient == NULL){
			server.sendReply(client, ":" + server.getServerName() + " 401 " + client.getNickname() + " " + target + " :No such nick/channel");
			return;
		}
		server.relayMessage(*targetClient, full);
	}
}