#include "../includes/TopicCmd.hpp"
#include "../includes/Channel.hpp"

TopicCmd::TopicCmd(){}

TopicCmd::~TopicCmd(){}

void TopicCmd::execute(Server& server, Client& client, const std::vector<std::string>& args){
	if (!client.isRegistered()){
		server.sendReply(client, "451 " + client.getNickname() + " :You have not registered");
		return;
	}

	if (args.size() < 1){
		server.sendReply(client, "461 " + client.getNickname() + " TOPIC :Not enough parameters");
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

	if (args.size() == 1){
		const std::string& topic = channel->getChannelTopic();
		if (topic.empty()){
			server.sendReply(client, "331 " + client.getNickname() + " " + args[0] + " :No topic is set");
			return;
		}
		else
			server.sendReply(client, "332 " + client.getNickname() + " " + args[0] + " :" + topic);
	}
	else if (args.size() > 1 && channel->getModeTopic()){
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
		channel->setChannelTopic(args[1]);
		std::string topicChangeMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@" + server.getServerName() + " TOPIC " + args[0] + " :" + args[1];
		channel->broadcastMessage(server, topicChangeMsg);

	}
	else{
		channel->setChannelTopic(args[1]);
		std::string topicChangeMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@" + server.getServerName() + " TOPIC " + args[0] + " :" + args[1];
		channel->broadcastMessage(server, topicChangeMsg);
	}
}