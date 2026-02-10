#include "../includes/KickCmd.hpp"
#include "../includes/Channel.hpp"

KickCmd::KickCmd(){}

KickCmd::~KickCmd(){}

void KickCmd::execute(Server& server, Client& client, const std::vector<std::string>& args){
	if (!client.isRegistered()){
		server.sendReply(client, "451 " + client.getNickname() + " :You have not registered");
		return;
	}

	if (args.size() < 2){
		server.sendReply(client, "461 " + client.getNickname() + " KICK :Not enough parameters");
		return;
	}

	Channel* channel = server.getChannel(args[0]);
	if (channel == NULL){
		server.sendReply(client, "403 " + client.getNickname() + " " + args[0] + " :No such channel");
		return;
	}

	if (!channel->isInChannel(&client)){
		server.sendReply(client, "442 " + client.getNickname() + " " + args[0] + " :You're not on that channel");
		return;
	}

	std::vector<Client *> admin = channel->getAdmin();
	std::vector<Client *>::const_iterator it = admin.begin();
	for(; it != admin.end(); it++){
		if (*it == &client)
			break;
	}
	if (it == admin.end()){
		server.sendReply(client, "482 " + client.getNickname() + " " + args[0] + " :You're not channel operator");
		return;
	}

	std::string target = args[1];
	Client* targetClient = server.getClientByNick(target);
	if (!targetClient || !channel->isInChannel(targetClient)){
		server.sendReply(client, "441 " + client.getNickname() + " " + target + " " + args[0] + " :They aren't on that channel");
		return;
	}

	std::string reason = (args.size() >= 3) ? args[2] : "You have been kicked";

	std::string kickMessage = ":" + client.getNickname() + "!" + client.getUsername() + "@" + server.getServerName() + " KICK " + args[0] + " " + target + " :" + reason;
	channel->broadcastMessage(server, kickMessage);
	channel->removeClient(targetClient);
}