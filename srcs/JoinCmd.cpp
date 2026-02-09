#include "../includes/JoinCmd.hpp"

void JoinCmd::execute(Server &server, Client &client, std::vector<std::string> const &args) {
    if (args.empty())
        return;
    std::string channelName = args[0];
    if (channelName[0] != '#')
        return;
    Channel *channel = server.getChannel(channelName);
    if (channel == NULL){
        channel = server.createChannel(channelName);
        channel->addAdmin(&client);
    }
    channel->addClient(&client);
    std::string joinMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost JOIN :" + channelName + "\r\n";
    channel->broadcastMessage(server, joinMsg);
    if (channel->getChannelTopic().empty())
        server.sendReply(client, ":" + server.getServerName() + " 331 " + client.getNickname() + " #" + channelName + ": No Topic is set" + "\r\n");
    else
        server.sendReply(client, ":" + server.getServerName() + " 332 " + client.getNickname() + " #" + channelName + ": Topic is " + channel->getChannelTopic() + "\r\n");
    server.sendReply(client, ":" + server.getServerName() + " 353 " + client.getNickname() + "#" + channelName + " :" + channel->getUserList());
    server.sendReply(client, ":" + server.getServerName() + " 366" + client.getNickname() + " #" + channelName + " :End of /NAMES list")  
}
