#include "../includes/PartCmd.hpp"

PartCmd::PartCmd(){}

PartCmd::~PartCmd(){}

void PartCmd::execute(Server& server, Client& client, const std::vector<std::string>& args){
	if (args.empty()){
		server.sendReply(client, "461 PART :Not enough parameters");
		return;
	}

	std::vector<std::string> channelsToLeave;
	if (args[0].find(',') != std::string::npos){
		std::string list = args[0];
		size_t start = 0;
		size_t pos;
		while ((pos = list.find(',', start)) != std::string::npos) {
			std::string channel = list.substr(start, pos - start);
			channelsToLeave.push_back(channel);
			start = pos + 1;
		}
		if (start < list.size()) {
			std::string channel = list.substr(start);
			if (!channel.empty())
			channelsToLeave.push_back(channel);
		}
	}
	else
		channelsToLeave.push_back(args[0]);

	for(size_t i = 0; i < channelsToLeave.size(); i++){
		Channel* channel = server.getChannel(channelsToLeave[i]);

		if (channel == NULL){
			server.sendReply(client, "403 " + client.getNickname() + " " + channelsToLeave[i] + " :No such channel");
			continue;
		}

		if (!channel->isInChannel(client)){
			server.sendReply(client, "442 " + client.getNickname() + " " + channelsToLeave[i]+ " :You're not on that channel");
			continue;
		}

		std::string partMessage;
		if (args.size() > 1)
			partMessage = ":" + client.getNickname() + "!" + client.getUsername() + "@" + server.getServerName() + " PART " + channelsToLeave[i] + " :" + args[args.size() - 1];
		else
			partMessage = ":" + client.getNickname() + "!" + client.getUsername() + "@" + server.getServerName() + " PART " + channelsToLeave[i];
		channel->broadcastMessage(server, partMessage);
		channel->removeClient(&client);
		client.removeOneChannel();
	}

}