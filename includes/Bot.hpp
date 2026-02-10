/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdsiurds <mdsiurds@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 11:59:18 by mdsiurds          #+#    #+#             */
/*   Updated: 2026/02/10 13:58:23 by mdsiurds         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>
#include <vector>
#include <map>
#include <ctime>
#include "./../includes/Client.hpp"

class Server;

class Bot{
    private:
        int _fd;
        std::string _nickname;
		std::string _username;
		std::string _realname;
		std::string _buffer;

        std::vector<std::string> _kind;
        std::vector<std::string> _angry;
        
        // bool _isRegistered;
        // bool _isAuthenticated;
        
    public:
        Bot();
        //Bot(int fd_bot);
        ~Bot();
        
        int getFd() const;
        //void setFd(int fd);

        bool getRegistered() const;
		bool getAuthenticated() const;

        void setup(int port, std::string password);
        void setStr();
        void sendMessage(Server& server);
        //void analyse(const std::string buffer);
};