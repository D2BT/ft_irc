/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdsiurds <mdsiurds@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/09 10:58:03 by mdsiurds          #+#    #+#             */
/*   Updated: 2026/02/09 11:04:54 by mdsiurds         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Bot.hpp"

Bot::Bot(){}

Bot::Bot(int fd_bot) : _fd(fd_bot), _nickname("BOT_nick"), _username("BOT_user"), _realname("BOT_real"){}

Bot::~Bot(){
    std::cout << "Bot die" << std::endl;
}

int Bot::getFd()const{
    return(this->_fd);
}

