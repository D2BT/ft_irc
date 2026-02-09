#include "../includes/PartCmd.hpp"

PartCmd::PartCmd(){}

PartCmd::~PartCmd(){}

void PartCmd::execute(Server& server, Client& client, const std::vector<std::string>& args){
	if (args.empty()){
		server.sendReply(client, "461 PART :Not enough parameters");
		return;
	}
	std::vector<std::string> channelsToLeave;
	std::string channelToLeave = args[0];
	if (channelToLeave.find(',')){
		
	}
	else
		channelsToLeave.push_back(channelToLeave.substr(1));
}