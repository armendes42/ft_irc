#include "main.hpp"

void kill(Server *serv, std::string buffer, int sd)
{
    std::string buf(buffer);
    int j = 0;

    if (FIND_USER(sd)->getMode().find('o') == std::string::npos)
    {
        sendMessage(sendRplErr(481, serv, FIND_USER(sd), "", ""), sd);
        return;
    }
    size_t i;
    std::string name = "";
    if ((i = buf.find_first_not_of(sep, 5)) != std::string::npos)
        name = buf.substr(i, ((j = buf.find_first_of(sep, i)) - i));
    if (name.empty())
    {
        sendMessage(sendRplErr(461, serv, FIND_USER(sd), "KILL", ""), sd);
        return;
    }
    if (serv->searchUserByNickname(name) == -1)
    {
        sendMessage(sendRplErr(401, serv, FIND_USER(sd), name, ""), sd);
        return;
    }
    disconnectUser(serv, serv->searchUserByNickname(name));
}