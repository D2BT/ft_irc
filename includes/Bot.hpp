/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdsiurds <mdsiurds@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 11:59:18 by mdsiurds          #+#    #+#             */
/*   Updated: 2026/02/09 11:04:30 by mdsiurds         ###   ########.fr       */
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

class Bot{
    private:
        int _fd;
        std::string _nickname;
		std::string _username;
		std::string _realname;
		std::string _buffer;
        
        // bool _isRegistered;
        // bool _isAuthenticated;
        
    public:
        Bot();
        Bot(int fd_bot);
        ~Bot();
        
        int getFd() const;
        //void setFd(int fd);

        bool getRegistered() const;
		bool getAuthenticated() const;

        void setup();
        void sendMessage();
        //void analyse(const std::string buffer);
};