#include "../includes/JoinCmd.hpp"

static std::vector<std::string> split(std::string const &str, char delimiter){
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while(std::getline(tokenStream, token, delimiter))
        tokens.push_back(token);
    return tokens;
}

JoinCmd::JoinCmd() {}

JoinCmd::~JoinCmd() {}

void JoinCmd::execute(Server &server, Client &client, std::vector<std::string> const &args) {
    if (!client.isRegistered()){
        server.sendReply(client, "451 " + client.getNickname() + " :You have not registered");
        return;
    }

    if (args.empty())
        return;
    std::vector<std::string> channelNames = split(args[0], ',');
    std::vector<std::string> keys;
    if (args.size() > 1)
        keys = split(args[1], ',');
    for (size_t i = 0; i < channelNames.size(); i++){
        std::string key = (i < keys.size() ? keys[i] : "");
        joinChannel(server, client, channelNames[i], key);
    }
}

void JoinCmd::joinChannel(Server &server, Client &client, std::string channelName, std::string key){
    if (channelName[0] != '#')
        return;
    Channel *channel = server.getChannel(channelName);
    if (channel == NULL){
        channel = server.createChannel(channelName, key);
        channel->addAdmin(&client);
    }
    else {
        if (channel->isInChannel(&client)){
            return;
        }
        if (channel->getUserLimit() > 0 && channel->getNumberOfUsers() >= channel->getUserLimit()){
            server.sendReply(client, ":" + server.getServerName() + " 471 " + client.getNickname() + " " + channelName + " ::Cannot join channel (+l)");
            return;
        }
        if (channel->getModeInvite() && !channel->isInvited(client)){
            server.sendReply(client, ":" + server.getServerName() + " 473 " + client.getNickname() + " " + channelName + " ::Cannot join channel (+i)");
            return;
        }
        std::string channelPwd = channel->getPasswordChannel();
        if (!channelPwd.empty() && key != channelPwd){
            server.sendReply(client, ":" + server.getServerName() + " 475 " + client.getNickname() + " " + channelName + " : Cannot join channel (+k)\r\n");
            return;
        }

    }
    channel->addClient(&client);
    client.addOneChannel();
    std::string joinMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost JOIN :" + channelName + "\r\n";
    channel->broadcastMessage(server, joinMsg);
    if (channel->getChannelTopic().empty())
        server.sendReply(client, ":" + server.getServerName() + " 331 " + client.getNickname() + " " + channelName + ": No Topic is set" + "\r\n");
    else
        server.sendReply(client, ":" + server.getServerName() + " 332 " + client.getNickname() + " " + channelName + ": Topic is " + channel->getChannelTopic() + "\r\n");
    server.sendReply(client, ":" + server.getServerName() + " 353 " + client.getNickname() + " = " + channelName + " :" + channel->getUserList());
    server.sendReply(client, ":" + server.getServerName() + " 366 " + client.getNickname() + " " + channelName + " :End of /NAMES list");
}