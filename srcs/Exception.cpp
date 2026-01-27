#include "../includes/Exception.hpp"

const char *ErrorArg::what() const throw(){
	return "Pls use ./ircserv <port> <password>";
}

const char *ErrorPort::what() const throw(){
	return "Invalid Port !";
}

const char *SocketError::what() const throw(){
	return "Problem with socket";
}

const char *PollError::what() const throw(){
	return "Problem with poll";
}