/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdsiurds <mdsiurds@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/09 10:58:03 by mdsiurds          #+#    #+#             */
/*   Updated: 2026/02/09 17:58:46 by mdsiurds         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Bot.hpp"
#include "../includes/Server.hpp"
#include "../includes/Logger.hpp"

Bot::Bot() : _fd(42), _nickname("BOT_nick"), _username("BOT_user"), _realname("BOT_real"){}

//Bot::Bot(int fd_bot) : _fd(fd_bot), _nickname("BOT_nick"), _username("BOT_user"), _realname("BOT_real"){}

Bot::~Bot(){
    std::cout << "Bot die" << std::endl;
}

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

void Bot::sendMessage(/*recuperer le nom du channel*/){
    // std::vector<std::string> kind;
    
    // std::vector<std::string> angry;
    static int kind = 0;
    static int angry = 0;
    //std::cout << "Ca fais 10s que personne n'a ecris... je me sens comme un bit isolee" << std::endl;
    if (/*all client of this channel are kind*/1 != 1){
        std::cout << _kind[kind % 6] << std::endl;
        kind++;
    }
    else{
        std::cout << /*Client._nickname << */ _angry[angry % 8] << std::endl;
        if (angry % 8 == 7)
            /*channel.kickByBot(_nickname)*/ std::cout << "KICK" << std::endl;
        angry++;
    }
}

void Bot::setStr(){
    _kind.push_back("Ca fait plaisir de voir que ce monde est rempli de gens poli.");
    _kind.push_back("On va au Marigny tout a l'heure ?");
    _kind.push_back("J'aimerais bien aller au camp Forty2 avec vous.");
    _kind.push_back("Il parait que les huitres y sont super bonnes, j'ai entendu dire que la flipper Marvel est incroyable.");
    _kind.push_back("Faut Juste pas donner de micro a Mdsiurds...");
    _kind.push_back("Surtout n'ecoutez pas Abenabbou, quand il vous propose une sortie velo...");
    _kind.push_back("Et attention a Qdebraba, il aime bronzer sans habits...");

    _angry.push_back("404 : Politesse not found.");
    _angry.push_back("tu connais la politesse ?!");
    _angry.push_back("on s'est pas bien compris je crois !!");
    _angry.push_back("tu connais Vald ? Ben dis bonjour sinon...!!");
    _angry.push_back("attention, je vais te segfault au visage.");
    _angry.push_back("encore un mot et je fais `rm -rf /` sur ta session.");
    _angry.push_back("si l'intelligence était un bit, tu serais à 0.");
    _angry.push_back("aller tchao j'te kick. 5...4...3...2...1...");
    
}