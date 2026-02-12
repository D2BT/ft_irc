#pragma once

#include <iostream>
#include <string>
#include <ctime>
#include "../includes/Color.hpp"

enum LogLevel{
	DEBUG = 0,
	INFO = 1,
	CMD = 2,
	ERROR = 3,
	RECIVE = 4,
	REPLY = 5
};

class Logger{
	public:

		static void log(LogLevel level, const std::string& message){
			std::string prefix;
			std::string color;
			const std::string reset = "\033[0m";

			switch (level){
				case DEBUG:
					prefix = "[DEBUG]";
					color = WHITE;
					break;
				case INFO:
					prefix = "[INFO]";
					color = GREEN;
					break;
				case CMD:
					prefix = "[CMD]";
					color = YELLOW;
					break;
				case ERROR:
					prefix = "[ERROR]";
					color = RED;
					break;
				case RECIVE:
					prefix = "[RECIVE]";
					color = ORANGE;
					break;
				case REPLY:
					prefix = "[SERV REPLY]";
					color = BLUE;

				}

		char time_buff[80];
		std::time_t now = std::time(0);
		std::strftime(time_buff, sizeof(time_buff), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

		if (level == ERROR)
				std::cerr << std::string(time_buff) << " " << color << prefix << " " << message << reset << std::endl;
		else
				std::cout << std::string(time_buff) << " " << color << prefix << " " << message << reset << std::endl;
		}
};