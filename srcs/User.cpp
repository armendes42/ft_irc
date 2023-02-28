#include "main.hpp"

User::User(std::string nickname, std::string username, std::string hostname, std::string realName) : _nickname(nickname), _username(username), _hostname(hostname), _realName(realName){
}

User::~User()
{
    this->_channels.clear();
}

std::string User::getNickname() const
{
    return this->_nickname;
}

std::string User::getUsername() const
{ 
    return this->_username;
}

std::string User::getHostname() const
{
    return this->_hostname;
}

std::string User::getRealName() const
{
    return this->_realName;
}

std::set<std::string> & User::getChannels()
{
    return this->_channels;
}

size_t User::getChannelsSize() const
{
    return this->_channels.size();
}

std::string User::getMode() const
{
    return this->_mode;
}

void User::setMode(std::string mode)
{
    this->_mode = mode;
}

void User::addChannel(std::string channelName)
{
    if (this->_channels.find(channelName) == this->_channels.end())
        this->_channels.insert(channelName);
}

void User::setNick(std::string newNickname)
{
    this->_nickname = newNickname;
}

