#include "../includes/KickCmd.hpp"
#include "../includes/Channel.hpp"

KickCmd::KickCmd(){}

KickCmd::~KickCmd(){}

void KickCmd::execute(Server& server, Client& client, const std::vector<std::string>& args){
	if (!client.isRegistered()){
		server.sendReply(client, ":" + server.getServerName() + " 451 " + client.getNickname() + " :You have not registered");
		return;
	}

	if (args.size() < 2){
		server.sendReply(client, ":" + server.getServerName() + " 461 " + client.getNickname() + " KICK :Not enough parameters");
		return;
	}

	std::string channelName = args[0];
	for (size_t i = 0; i < channelName.size(); i++) {channelName[i] = std::toupper(channelName[i]);}

	Channel* channel = server.getChannel(channelName);
	if (channel == NULL){
		server.sendReply(client, ":" + server.getServerName() + " 403 " + client.getNickname() + " " + channelName + " :No such channel");
		return;
	}

	if (!channel->isInChannel(&client)){
		server.sendReply(client, ":" + server.getServerName() + " 442 " + client.getNickname() + " " + channelName + " :You're not on that channel");
		return;
	}

	if (!channel->isOperator(&client) && !channel->isOwner(&client)){
		server.sendReply(client, ":" + server.getServerName() + " 482 " + client.getNickname() + " " + channelName + " :You're not channel operator");
		return;
	}

	std::string target = args[1];
	Client* targetClient = server.getClientByNick(target);
	if (channel->isOwner(targetClient)){
		server.sendReply(client, ":" + server.getServerName() + " 482 " + client.getNickname() + " " + channelName + " :Cannot kick channel owner");
		return;
	}

	if (!targetClient || !channel->isInChannel(targetClient)){
		server.sendReply(client, ":" + server.getServerName() + " 441 " + client.getNickname() + " " + target + " " + channelName + " :They aren't on that channel");
		return;
	}

	std::string reason = (args.size() >= 3) ? args[2] : "You have been kicked";

	std::string kickMessage = ":" + client.getNickname() + "!" + client.getUsername() + "@" + server.getServerName() + " KICK " + channelName + " " + target + " :" + reason;
	channel->broadcastMessage(server, kickMessage);
	channel->removeClient(targetClient);
}