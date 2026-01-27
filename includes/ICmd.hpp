#pragma once

#include <string>
#include <vector>

class Server;
class Client;

class ICmd{
   public:

	   virtual ~ICmd(){};
	   virtual void execute(Server& server, Client& client, const std::vector<std::string>& args) = 0;
};