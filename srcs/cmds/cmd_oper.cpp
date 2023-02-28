#include "main.hpp"

void oper(Server *serv, std::string buffer, int sd)
{
    std::string buf(buffer);
    int j = 0;
    size_t i;
    std::string user;
    if ((i = buf.find_first_not_of(sep, 5)) != std::string::npos)
        user = buf.substr(i, ((j = buf.find_first_of(sep, i)) - i));
    if (user.empty())
    {
        sendMessage(sendRplErr(461, serv, FIND_USER(sd), "OPER", ""), sd);
        return;
    }
    if (serv->searchUserByNickname(user) == -1)
    {
        sendMessage(sendRplErr(401, serv, FIND_USER(sd), user, ""), sd);
        return;
    }
    j = buf.find_first_not_of(sep, j);
    std::string password = buf.substr(j, (buf.find_first_of(sep, j) - j));
    if (password.empty())
    {
        sendMessage(sendRplErr(461, serv, FIND_USER(sd), "OPER", ""), sd);
        return;
    }
    if (password.compare(OPER_PW) == 0)
    {
        std:: string userAnswer = userOutput(FIND_USER(serv->searchUserByNickname(user)));
        userAnswer += "MODE " + FIND_USER(serv->searchUserByNickname(user))->getNickname() + " +o";
        FIND_USER(serv->searchUserByNickname(user))->setMode(FIND_USER(serv->searchUserByNickname(user))->getMode() + "o");
        sendMessage(userAnswer, serv->searchUserByNickname(user));
        sendMessage(sendRplErr(381, serv, FIND_USER(serv->searchUserByNickname(user)), "", ""), serv->searchUserByNickname(user));
    }
    else
        sendMessage(sendRplErr(464, serv, FIND_USER(serv->searchUserByNickname(user)), "", ""), sd);
}