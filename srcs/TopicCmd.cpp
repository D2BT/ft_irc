#include "../includes/TopicCmd.hpp"
#include "../includes/Channel.hpp"

TopicCmd::TopicCmd(){}

TopicCmd::~TopicCmd(){}

void TopicCmd::execute(Server& server, Client& client, const std::vector<std::string>& args){
	if (!client.isRegistered()){
		server.sendReply(client, ":" + server.getServerName() + " 451 " + client.getNickname() + " :You have not registered");
		return;
	}

	if (args.size() < 1){
		server.sendReply(client, ":" + server.getServerName() + " 461 " + client.getNickname() + " TOPIC :Not enough parameters");
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

	if (args.size() == 1){
		const std::string& topic = channel->getChannelTopic();
		if (topic.empty()){
			server.sendReply(client, ":" + server.getServerName() + " 331 " + client.getNickname() + " " + channelName + " :No topic is set");
			return;
		}
		else{
			server.sendReply(client, ":" + server.getServerName() + " 332 " + client.getNickname() + " " + channelName + " :" + channel->getChannelTopic());
		}
	}
	else if (args.size() > 1 && channel->getModeTopic()){
		if (!channel->isOperator(&client) && !channel->isOwner(&client)){
			server.sendReply(client, ":" + server.getServerName() + " 482 " + client.getNickname() + " " + channelName + " :You're not channel operator");
			return;
		}
		channel->setChannelTopic(args[1]);
		std::string topicChangeMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@" + server.getServerName() + " TOPIC " + channelName + " :" + channel->getChannelTopic();
		channel->broadcastMessage(server, topicChangeMsg);
	}
	else{
		channel->setChannelTopic(args[1]);
		std::string topicChangeMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@" + server.getServerName() + " TOPIC " + channelName + " :" + channel->getChannelTopic();
		channel->broadcastMessage(server, topicChangeMsg);
	}
}