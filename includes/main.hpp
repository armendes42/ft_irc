#pragma once

#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <algorithm>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <csignal>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <cstdlib>
#include <fstream>
#include <ctime>

#include "../bot/includes/IRCbot.hpp"
#include "User.hpp"
#include "Command.hpp"
#include "Channel.hpp"
#include "Server.hpp"
#include "RPL_ERR.hpp"

#define SERVER_NAME "localhost"
#define ver "1.0"
#define USER_MODE "ior"
#define CHANNEL_MODE "ovamntlkb"
#define OPER_PW "5498"

#define FIND_USER(sd) \
    serv->getUsers().find(sd)->second

#define FIND_CHANNEL(channelName) \
    serv->getChannels().find(channelName)->second

#define CHECKUSERMODE(sd, mode) \
    FIND_USER(sd)->getMode().find(mode) == std::string::npos ? true : false

#define CHECKCHANNELMODE(channelName, mode) \
    FIND_CHANNEL(channelName)->getMode().find(mode) == std::string::npos ? true : false

#define maxClients 10

#define endBuf std::string("\r\n")
#define sep std::string(" \t\r\n")

class Server;
class User;
class Channel;
class IRCbot;

std::string userOutput(User *user);
std::string anonymousOutput();
std::string botOutput(std::string name);
void disconnectUser(Server *serv, int sd);
void sendMessage(std::string message, int sd);
void sendEveryoneInChan(std::string message, Channel *chan);
void sendEveryoneInChanExceptUser(std::string message, Channel *chan, int sd);
void sendEveryone(std::string message, std::map<int, User *> users);
bool nicknameIsInUse(Server *serv, std::string nick);
bool nicknameIsValid(std::string nick);
time_t	getTime(void);