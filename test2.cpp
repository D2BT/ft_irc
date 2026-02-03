/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test2.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdsiurds <mdsiurds@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/03 08:20:05 by mdsiurds          #+#    #+#             */
/*   Updated: 2026/02/03 14:54:55 by mdsiurds         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>
#include <vector>

#define MAX_CLIENT 500

int main(int argc, char **argv){
    if (argc < 2)
        return(std::cout << "Usage: /ircserv <port> <password>" << std::endl, 1);
    int port = atoi(argv[1]);
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    
    sockaddr_in serverAdress; //structure de configuration reseau
    serverAdress.sin_family = AF_INET;
    serverAdress.sin_port = htons(port);
    serverAdress.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr*)&serverAdress, sizeof(serverAdress)) == -1)
        return(std::cout << "Bind error" << std::endl, 1);
    
    if (listen(serverSocket, MAX_CLIENT) == -1)
        return(std::cout << "Listen error" << std::endl, 1);

    std::vector<struct pollfd> fds;
    
    struct pollfd pollFdServer;
    
    pollFdServer.fd = serverSocket;
    pollFdServer.events = POLLIN;
    pollFdServer.revents = 0;
    fds.push_back(pollFdServer);

    while(1){
        int activity = poll(fds.data(), fds.size(), -1); //-1 se reveille uniquement si un evenement se passe
        if (activity < 0){
            std::cout << "Poll error" << std::endl;
            break;
        }
        if (pollFdServer.revents){
            struct pollfd newClient; //structure d'observation d'evenement ainsi que son fd
            newClient.events = ;
            newClient.fd = accept(serverSocket, (struct sockaddr*)&);
        }
    }

    for (int i = 0; i < fds.size(); i++){
        close(fds[i].fd);
    }
}

// creation socket
// bind
// listen
// pollfd
// accept
// close