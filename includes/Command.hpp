#pragma once

#include "main.hpp"

class Server;

void nick(Server *serv, std::string buffer, int sd);
void join(Server *serv, std::string buffer, int sd);
void privmsg(Server *serv, std::string buffer, int sd);
void ping(Server *serv, std::string buffer, int sd);
void pong(Server *serv, std::string buffer, int sd);
void part(Server *serv, std::string buffer, int sd);
void topic(Server *serv, std::string buffer, int sd);
void kick(Server *serv, std::string buffer, int sd);
void mode(Server *serv, std::string buffer, int sd);
void oper(Server *serv, std::string buffer, int sd);
void kill(Server *serv, std::string buffer, int sd);
void quit(Server *serv, std::string buffer, int sd);
void restart(Server *serv, std::string buffer, int sd);