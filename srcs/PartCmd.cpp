#include "../includes/PartCmd.hpp"

PartCmd::PartCmd(){}

PartCmd::~PartCmd(){}

void PartCmd::execute(Server& server, Client& client, const std::vector<std::string>& args){
	if (args.empty()){
		server.sendReply(client, "461 PART :Not enough parameters");
		return;
	}
	std::vector<std::string> channelsToLeave;
	if (args[0].find(',')){
		for(size_t i = 0; i < args[0].size(); i++){
			int j = 0;
			while (args[0][j] && args[0][j] != ,)
		}
		for(size_t i = 0; i < args[0].size(); i++){	channelsToLeave[i].substr(1);}
	}
	else
		channelsToLeave.push_back(args[0].substr(1));


}