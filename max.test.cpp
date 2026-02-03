/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   max.test.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: max <max@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 08:44:29 by mdsiurds          #+#    #+#             */
/*   Updated: 2026/02/03 17:53:18 by max              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <iostream>
#include <vector>
#include <algorithm>


int main(int argc, char **argv){
    if(argc < 2)
        return(std::cout << "argv2 <port>, argv3 <password>" << std::endl, 1);
    int port = atoi(argv[1]);
    int socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
        return(std::cout << "Erreur socket_fd" << std::endl, 1);

    std::cout << "Socket_fd: " << socket_fd << std::endl;

    struct sockaddr_in socket_adresse;
    socket_adresse.sin_addr.s_addr = INADDR_ANY;
    socket_adresse.sin_family = AF_INET;
    socket_adresse.sin_port = htons(port);
    
    //bind(s, (struct sockaddr*)&sin, sizeof(sin));
    if (bind(socket_fd, (struct sockaddr*)&socket_adresse, sizeof(socket_adresse)) != 0)
        return(std::cout << "Erreur bind" << std::endl, 1);
    
    if (listen(socket_fd, 512) == -1)
        return(std::cout << "Erreur listen" << std::endl, 1);
    std::cout << "Le serveur ecoute sur le port: " << port << std::endl;

    std::vector< struct pollfd> fds;

    struct pollfd server_pollfd;
    server_pollfd.fd = socket_fd;
    server_pollfd.events = POLLIN;
    fds.push_back(server_pollfd);
    
    while(1){
        
        int activity = poll(fds.data(), fds.size(), -1);
        if (activity < 0){
            std::cout << "Erreur de poll()" << std::endl;
            break;
        }

        for (size_t i = 0; i < fds.size(); i++){
            if (fds[i].revents & POLLIN){
                if (fds[i].fd == socket_fd){
                    struct sockaddr_in newClient;
                    socklen_t newClient_len = sizeof(newClient);

                    int client_fd = accept(socket_fd, (struct sockaddr*)&newClient, &newClient_len);
                    if (client_fd >= 0){
                        std::cout << "Nouveau client connecte: " << client_fd << std::endl;

                        struct pollfd newClient_pollfd;
                        newClient_pollfd.fd = client_fd;
                        newClient_pollfd.events = POLLIN;
                        newClient_pollfd.revents = 0;
                        fds.push_back(newClient_pollfd);
                    }
                }
                else {
                    char buffer[512];
                    int taille = recv(fds[i].fd, buffer, sizeof(buffer) - 1, 0);
                    if (taille <= 0){
                        std::cout << "Deconnexion du client: " << fds[i].fd << std::endl;
                        close(fds[i].fd);
                        fds.erase(fds.begin() + i);
                        i--;
                    } 
                    else {
                        buffer[taille] = '\0';
                        std::cout << "Message de " << fds[i].fd << ": " << buffer << std::endl;
                    }         
                }
            }
        }
    }
    for (size_t i = 0; i < fds.size(); i++)
        close(fds[i].fd);
}

// socket() // creer le tunnel DU SERVEUR
// bind() // lier le socket a un port specifique(argv[2])
// listen() // serveur pret a recevoir des clients


