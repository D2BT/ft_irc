/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test2.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdsiurds <mdsiurds@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/03 08:20:05 by mdsiurds          #+#    #+#             */
/*   Updated: 2026/02/09 09:57:30 by mdsiurds         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>
#include <vector>
#include <map>
#include "Bot.hpp"
#include "./includes/Client.hpp"

#define MAX_CLIENT 500

int main(int argc, char **argv){
    if (argc < 2)
        return(std::cout << "Usage: /ircserv <port> <password>" << std::endl, 1);
    int port = atoi(argv[1]);
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    
    std::map<int, Client> clients;
    
    sockaddr_in serverAdress; //structure de configuration reseau
    serverAdress.sin_family = AF_INET;
    serverAdress.sin_port = htons(port);
    serverAdress.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr*)&serverAdress, sizeof(serverAdress)) == -1)
        return(std::cout << "Bind error" << std::endl, 1);
    
    if (listen(serverSocket, MAX_CLIENT) == -1)
        return(std::cout << "Listen error" << std::endl, 1);

    std::vector<struct pollfd> vector_PollFds;
    
    struct pollfd pollFdServer;
    
    pollFdServer.fd = serverSocket;
    pollFdServer.events = POLLIN;
    pollFdServer.revents = 0;
    vector_PollFds.push_back(pollFdServer);
    
    // CREATION BOT:
    struct sockaddr_in botAddr;
    socklen_t BOT_len = sizeof(botAddr);
    
    int fd_bot = accept(serverSocket, (struct sockaddr*)&botAddr, &BOT_len);
    if (fd_bot >= 0){
        std::cout << "BOT connected, fd: " << fd_bot << std::endl;
        
        struct pollfd botPollfd; //structure d'observation d'evenement ainsi que son fd
        botPollfd.fd = fd_bot;
        botPollfd.events = POLLIN; 
        botPollfd.revents = 0;
        vector_PollFds.push_back(botPollfd);
        clients.insert({fd_bot, Client(fd_bot)});

        
    }
    else
        std::cout << "botPollfd error" << std::endl;
    Bot iRoBot(fd_bot);
    while(1){
        //std::cout << "Welcome to Max IRC SERVER" << std::endl;
        int activity = poll(vector_PollFds.data(), vector_PollFds.size(), 1000);
        if (activity < 0){
            std::cout << "Poll error" << std::endl;
            break;
        }
        
        for (size_t i = 0; i < vector_PollFds.size(); i++){
            if (vector_PollFds[i].revents & POLLIN){  // On utilise '&' (ET binaire) car revents contient une combinaison de flags (bits), (revents & POLLIN) renvoie VRAI si le bit POLLIN est activé, indépendamment des autres bits.
                if (vector_PollFds[i].fd == serverSocket){ // si cest le serveur: nouvelle connextion
                    struct sockaddr_in client;
                    socklen_t client_len = sizeof(client);
        
                    int fd_client = accept(serverSocket, (struct sockaddr*)&client, &client_len);
					if (fd_client >= 0){
						std::cout << "New client connected, fd: " << fd_client << std::endl;
						
						struct pollfd newClient; //structure d'observation d'evenement ainsi que son fd
						newClient.fd = fd_client;
						newClient.events = POLLIN; 
						newClient.revents = 0;
						vector_PollFds.push_back(newClient);
					}
                }
                else { // message recu d'un client
					char buffer[512];
					int taille = recv(vector_PollFds[i].fd, buffer, sizeof(buffer) -1, 0);
					
					if (taille <= 0){ //erreur ou deconnextion
						std::cout << "Deconnexion client: " << vector_PollFds[i].fd << std::endl;
						close(vector_PollFds[i].fd);
						vector_PollFds.erase(vector_PollFds.begin() + i);
						i-- ; //recommence avec le meme i 
					}
					else{ //message recu
						buffer[taille] = '\0';
						std::cout << "Message de " << vector_PollFds[i].fd << ": " << buffer << std::endl;
                        iRoBot.analyse((std::string)buffer);
					}
				}
			}
        }
    }

	for (size_t i = 0; i < vector_PollFds.size(); i++){
		close(vector_PollFds[i].fd);
	}
}
//  donc actuellemtn jaccept mon fd bot alors que je devrais dabord socket et connect ?

// EXACTEMENT ! Vous avez tout compris.

// Actuellement vous faites :
// accept (J'attends que quelqu'un vienne) -> Le serveur se met en pause et attend... attend...

// Ce que vous devez faire :

// socket (Le bot achète un téléphone).
// connect (Le bot lance l'appel).
// Et ensuite seulement, le serveur verra arriver cet appel dans son poll et fera accept.
// Je corrige test2.cpp immédiatement pour respecter cet ordre.

// Attention : je dois aussi m'assurer que votre Bot.hpp est compatible avec ce qu'on va écrire (qu'il a bien une méthode auth ou similaire). Je vais jeter un œil rapide avant.

// creation socket
// bind
// listen
// pollfd
// accept
// close