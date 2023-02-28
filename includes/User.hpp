#pragma once

#include "main.hpp"

class User{
	private:
		std::string _nickname;
		std::string _username;
		std::string _hostname;
		std::string _realName;
		std::set<std::string> _channels;
		std::string _mode;

	public:
		User(std::string nick, std::string user, std::string hostname, std::string realName);
		~User();
		std::string getNickname() const;
		std::string getUsername() const;
		std::string getHostname() const;
		std::string getRealName() const;
		std::set<std::string> &getChannels();
		size_t getChannelsSize() const;
		std::string getMode() const;
		void addChannel(std::string channelName);
		void setNick(std::string newNickname);
		void setMode(std::string mode);
};
