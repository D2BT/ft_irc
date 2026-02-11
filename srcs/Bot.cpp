/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdsiurds <mdsiurds@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/09 10:58:03 by mdsiurds          #+#    #+#             */
/*   Updated: 2026/02/11 10:46:56 by mdsiurds         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Bot.hpp"
#include "../includes/Server.hpp"
#include "../includes/Logger.hpp"
#include <map>

Bot::Bot() : _fd(42), _nickname("BOT_nick"), _username("BOT_user"), _realname("BOT_real"){}

//Bot::Bot(int fd_bot) : _fd(fd_bot), _nickname("BOT_nick"), _username("BOT_user"), _realname("BOT_real"){}

Bot::~Bot(){}

void Bot::setup(int port, std::string password){
    this->_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(_fd < 0)
        Logger::log(ERROR, "Erreur socket bot");
    
    fcntl(_fd, F_SETFL, O_NONBLOCK);

    struct sockaddr_in bot_Addr;
    bot_Addr.sin_family = AF_INET;
    bot_Addr.sin_port = htons(port);
    bot_Addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //adresse du serveur (localhost)
    
    if (connect(_fd, (struct sockaddr*)&bot_Addr, sizeof(bot_Addr)) < 0){
        if (errno != EINPROGRESS){ //ENETUNREACH (Réseau inaccessible) / EALREADY (Déjà en cours) / ETIMEDOUT (Délai dépassé) / EACCES (Pas la permission)
            Logger::log(ERROR, "Erreur connect pour le bot");
        }
    }
    
    std::string PASS = "PASS " + password + "\r\n";
    std::string NICK = "NICK NICK_BOT\r\n";
    std::string USER = "USER USER_BOT 0 * :iRoBot, la fin est proche\r\n";

    send(_fd, PASS.c_str(), PASS.size(), 0);
    send(_fd, NICK.c_str(), NICK.size(), 0);
    send(_fd, USER.c_str(), USER.size(), 0);
    
    this->setStr();
}

int Bot::getFd()const{
    return(this->_fd);
}

// void Bot::sendMessage(/*recuperer le nom du client ET serveur*/Server& server){
//     static int kind = 0;
//     bool someoneAngry = false;

//     std::map<int, Client *> clientsList = server.getClients();
//     std::map<int, Client *>::iterator it = clientsList.begin();
//     for(;it != clientsList.end(); it++){
//         //std::cout << "ici" << std::endl;
//         if (it->second->isKind() == false && it->second->getNbChannelIn() > 0){
//             someoneAngry = true;
//             std::cout << "quelqu'un pas gentil ici" << std::endl;
//             server.sendReply(*it->second, it->second->getNickname() + _angry[it->second->getAngryLevel() % 8]);
//             if (it->second->getAngryLevel() % 8 == 7){
//                 server.disconnectClient(it->second->getFd());
//                 //sendreply all
//                 /*channel.kickByBot(_nickname)*/ //std::cout << "KICK FD: " << it->second->getFd() << std::endl;
//                 continue;
//             }
//             std::cout << "Level angry de " << it->second->getNickname() << "est de" << it->second->getAngryLevel() << std::endl;
//             it->second->addLevelAngry(); //fonction dans client a faire
//         }
//     }
//     if (someoneAngry == false && ){
//         server.sendToAllClient(_kind[kind % 6]);
//         kind++;
//     }
// } 

void Bot::messageToBadPeople(Server& server){
    std::map<int, Client *> clientsList = server.getClients();
    std::map<int, Client *>::iterator clientBegin = clientsList.begin();
    std::map<int, Client *>::iterator clientEnd = clientsList.end();
    for(;clientBegin != clientEnd; clientBegin++){
        if (clientBegin->second->isKind() == false && clientBegin->second->getNbChannelIn() > 0){
            server.sendReply(*clientBegin->second, clientBegin->second->getNickname() + _angry[clientBegin->second->getAngryLevel() % 8]);
                if (clientBegin->second->getAngryLevel() % 8 == 7){
                    for (/*all channel ou est le client*/){
                        std::string msg
                        server.notifyClientQuit(msg);
                    }
                    server.disconnectClient(clientBegin->second->getFd());
                    //msg to all client
                    
                    /*channel.kickByBot(_nickname)*/ //std::cout << "KICK FD: " << clientBegin->second->getFd() << std::endl;
                    continue;
                }
                //std::cout << "Level angry de " << clientBegin->second->getNickname() << "est de" << clientBegin->second->getAngryLevel() << std::endl;
                clientBegin->second->addLevelAngry(); //fonction dans client a faire
            }
    }
}

void Bot::sendMessage(/*recuperer le nom du client ET serveur*/Server& server){
    static int kind = 0;

    std::map<std::string, Channel *> channel = server.getChannels();
    for (std::map<std::string, Channel *>::iterator itChannel = channel.begin(); itChannel != channel.end(); itChannel++){
        std::vector<Client *> users = itChannel->second->getUsers();
        std::vector<Client *>::const_iterator itClient = users.begin();
        for (; itClient != users.end(); itClient++){
            if (!((*itClient)->isKind())){
                break;
            }
        }
        if (itClient == users.end()){
            std::string botMsg =  ":bot!bot@" + server.getServerName() + " BOTMSG " + itChannel->second->getChannelName() + " :" + _kind[kind % 6];
            itChannel->second->broadcastMessage(server, botMsg);
        }
        else{
            std::string botMsg =  ":bot!bot@" + server.getServerName() + " BOTMSG " + itChannel->second->getChannelName() +" :WARNING ! Someone is not polite there !";
            itChannel->second->broadcastMessage(server, botMsg);
        }
    }
    kind++;
    messageToBadPeople(server);
}

void Bot::setStr(){
    _kind.push_back("Ca fait plaisir de voir que ce monde est rempli de gens poli.");
    _kind.push_back("On va au Marigny tout a l'heure ?");
    _kind.push_back("J'aimerais bien aller au camp Forty2 avec vous.");
    _kind.push_back("Il parait que les huitres y sont super bonnes, j'ai entendu dire que la flipper Marvel est incroyable.");
    _kind.push_back("Faut Juste pas donner de micro a Mdsiurds...");
    _kind.push_back("Surtout n'ecoutez pas Abenabbo, quand il vous propose une sortie velo...");
    _kind.push_back("Et attention a Qdebraba, il aime bronzer sans habits...");

    _angry.push_back(" error 404 : Politesse not found.");
    _angry.push_back(" connais-tu la politesse ?!");
    _angry.push_back(" on ne s'est pas bien compris je crois !!");
    _angry.push_back(" tu connais Vald ? Ben dis bonjour sinon on va te niquer ta carte mere !!");
    _angry.push_back(" attention, je vais te segfault au visage.");
    _angry.push_back(" encore un mot et je fais `rm -rf /` sur ta session.");
    _angry.push_back(" si l'intelligence était un bit, tu serais à 0.");
    _angry.push_back(" ?!!aller tchao j'te kick. 5...4...3...2...1...");
}