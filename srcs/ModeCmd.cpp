#include "../includes/ModeCmd.hpp"

ModeCmd::ModeCmd() {}

ModeCmd::~ModeCmd() {}

void ModeCmd::execute(Server &server, Client &client, std::vector<std::string> const &args) {
    if (!client.isRegistered()){
        server.sendReply(client, ":" + server.getServerName() + " 451 " + client.getNickname() + " :You have not registered");
        return;
    }
    if (args.size() < 2){
        server.sendReply(client, ":" + server.getServerName() + " 461 " + client.getNickname() + " MODE :Not enough parameters");
        return;
    }
    std::string channelName = args[0];
    for (size_t i = 0; i < channelName.size(); i++) {channelName[i] = std::toupper(channelName[i]);}
    
    if (channelName[0] != '#'){
        server.sendReply(client, ":" + server.getServerName() + " 476 " + client.getNickname() + " MODE :Bad channel mask");
        return;
    }
    Channel *channel = server.getChannel(channelName);
    if (!channel){
        server.sendReply(client, ":" + server.getServerName() + " 403 " + client.getNickname() + " MODE :No such channel");
        return;
    }
    if (!channel->isOperator(&client) && !channel->isOwner(&client)){
        server.sendReply(client, ":" + server.getServerName() + " 482 " + client.getNickname() + " MODE :Need operator privs for this action");
        return;
    }

    std::string flags = args[1];
    size_t argsIndex = 2;
    bool adding = true;
    std::string successFlag;
    std::string successArgs;

    for (size_t i = 0; i < flags.size(); i++){
        char mode = flags[i];
        if (mode == '+'){
            adding = true;
            continue;
        }
        else if (mode == '-'){
            adding = false;
            continue;
        }
        switch (mode) {
            case 'i' :{
                channel->setModeInvite(adding);
                successFlag += adding ? "+i" : "-i";
                break;
            }
            case 't' :{
                channel->setModeTopic(adding);
                successFlag += adding ? "+t" : "-t";
                break;
            }
            case 'k' :{
                if (adding) {
                    if (argsIndex >= args.size()){
                        server.sendReply(client, ":" + server.getServerName() + " 461 " + client.getNickname() + " MODE +k :Not enough parameters");
                        continue;
                    }
                    channel->setPassword(args[argsIndex]);
                    successFlag += "+k";
                    successArgs += " " + args[argsIndex];
                    argsIndex++;
                }
                else {
                    channel->setPassword("");
                    successFlag += "-k";
                }
                break;
            }
            case 'l' :{
                if (adding){
                    if (argsIndex >= args.size()){
                        server.sendReply(client, ":" + server.getServerName() + " 461 " + client.getNickname() + " MODE +l :Not enough parameters");
                        continue;
                    }
                    channel->setUserLimit(std::atoi(args[argsIndex].c_str()));
                    successFlag += "+l";
                    successArgs += " " + args[argsIndex];
                    argsIndex++;
                    continue;
                }
                else {
                    channel->setUserLimit(0);
                    successFlag += "-l";
                }
                break;
            }
            case 'o' :{
                if (argsIndex >= args.size()) {
                    std::string action = adding ? "+o" : "-o";
                    server.sendReply(client, ":" + server.getServerName() + " 461 " + client.getNickname() + " MODE " + action + " :Not enough parameters");
                    continue;
                }
                std::string const &target = args[argsIndex];
                Client *targetUser = server.getClientByNick(target);
                if (!targetUser || !channel->isInChannel(targetUser)) {
                    server.sendReply(client, ":" + server.getServerName() + " 401 " + client.getNickname() + " " + target + " :No such nick/channel");
                    argsIndex++;
                    continue;
                }
                if (adding){
                    channel->addAdmin(targetUser);
                    successFlag += "+o";
                    successArgs += " " + args[argsIndex];
                }
                else {
                    channel->removeAdmin(targetUser);
                    successFlag += "-o";
                    successArgs += " " + args[argsIndex];
                }
                argsIndex++;
                break;
            }
            default :{
				server.sendReply(client, ":" + server.getServerName() + " 472 " + client.getNickname() + " " + mode + " :Unknown mode char for " + channelName);
				break;
            }
        }
    }
    if (!successFlag.empty()) {
        std::string infos = ":" + client.getNickname() + "!" + client.getUsername() + "@" + server.getServerName() + " MODE " + channelName + " " + successFlag + successArgs;
	    channel->broadcastMessage(server, infos);
    }
}