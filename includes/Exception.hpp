#pragma once

#include <iostream>

class ErrorArg: public std::exception{
	public:
		virtual const char *what() const throw();
};

class ErrorPort: public std::exception{
	public:
		virtual const char *what() const throw();
};

class SocketError: public std::exception{
	public:
		virtual const char *what() const throw();
};

class PollError: public std::exception{
	public:
		virtual const char *what() const throw();
};