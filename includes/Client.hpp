#pragma once

#include <iostream>

class Client{
	protected:
		int _fd;
		std::string _nickname;
		std::string _username;
		std::string _realname;
		std::string _buffer;
        bool _isRegistered;
        bool _isAuthenticated;
		bool _saidHello;
		int _nbChannelIn;

	public:
		Client();
		Client(int fd);
		~Client();

		void setFd(int fd);
		void setNickname(const std::string(nickname));
		void setUsername(const std::string(username));
		void setRealname(const std::string(realname));
		void setRegistered(bool status);
		void setAuthenticated(bool status);

		int getFd() const;
		const std::string& getNickname() const;
		const std::string& getUsername() const;
		const std::string& getRealname() const;
		bool isRegistered() const;
		bool isAuthenticated() const;
		std::string& getBuffer();
		void addOneChannel();
		void removeOneChannel();

		void addToBuffer(const std::string& data);
};