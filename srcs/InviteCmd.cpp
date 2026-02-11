#include "../includes/InviteCmd.hpp"

InviteCmd::InviteCmd(){}

InviteCmd::~InviteCmd(){}

void InviteCmd::execute(Server& server, Client& client, const std::vector<std::string>& args){
    if (args.empty() || args.size() < 2){
        server.sendReply(client, ":" + server.getServerName() + " 461 " + client.getNickname() + " INVITE :Not enough parameters");
        return;
    }
    std::string channelName = args[1];
    Channel *channel = server.getChannel(channelName);
    std::string targetName = args[0];
    Client *target = server.getClientByNick(targetName);
    if (!channel) {
        server.sendReply(client, ":" + server.getServerName() + " 403 " + client.getNickname() + " " + channelName + " :No such channel");
		return;
    }
    if (!target){
        server.sendReply(client, ":" + server.getServerName() + " 401 " + client.getNickname() + " " + targetName + " :No such nick/channel");
		return;
    }
    if (!channel->isInChannel(&client)){
        server.sendReply(client, ":" + server.getServerName() + " 442 " + client.getNickname() + " " + channelName + " :You're not on that channel");
		return;
    }
    if (channel->isInChannel(target)){
        server.sendReply(client, ":" + server.getServerName() + " 443 " + client.getNickname() + " " + channelName + " :Is already on channel");
		return;
    }
    if (channel->getModeInvite() && !channel->isOperator(&client)){
        server.sendReply(client, ":" + server.getServerName() + " 482 " + client.getNickname() + " " + channelName + " :Youre are not channel operator");
		return;
    }
    channel->addInvited(target);
    server.sendReply(client, ":" + server.getServerName() + " 341 " + client.getNickname() + " " + targetName + " " + channelName);
    server.sendReply(*target, ":" + client.getNickname() + " INVITE " + targetName + " :" + channelName);
}