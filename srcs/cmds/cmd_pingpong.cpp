#include "main.hpp"

void ping(Server *serv, std::string buffer, int sd)
{
    (void)serv;
    buffer[1] = 'O';
    sendMessage(buffer, sd);
}

void pong(Server *serv, std::string buffer, int sd)
{
    (void)serv;
    buffer[1] = 'I';
    sendMessage(buffer, sd);
}