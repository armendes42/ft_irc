#include "main.hpp"

extern int clientSocket[maxClients];

void disconnectUser(Server* serv, int sd)
{
    std::set<std::string> userChannels = FIND_USER(sd)->getChannels();
    // erase user from each channel;
    for (std::set<std::string>::iterator it = userChannels.begin(); it != userChannels.end(); it++)
    {
        std::string userAnswer = userOutput(FIND_USER(sd));
        userAnswer += "PART " + *it;
        sendEveryoneInChan(userAnswer, FIND_CHANNEL(*it));
        FIND_CHANNEL(*it)->leftUser(sd);
        if (FIND_CHANNEL(*it)->getUsersNumber() == 0)
        {
            delete serv->getChannels().find(*it)->second;
            serv->getChannels().erase(*it);
        }
    }
    FIND_USER(sd)->getChannels().clear();
    delete FIND_USER(sd);
    serv->getUsers().erase(sd);
    std::cout << "Host disconnected , ip " << inet_ntoa(serv->getServer().sin_addr) << " , port " << ntohs(serv->getServer().sin_port) << " , number of users: " <<  serv->getUsers().size() << std::endl;
    for (int i = 0; i < maxClients; i++)
        if (clientSocket[i] == sd)
            clientSocket[i] = 0;
    close(sd);
}

void quit(Server *serv, std::string buffer, int sd)
{
    std::string buf(buffer);
    size_t i;
    std::string message;
    if ((i = buf.find_first_not_of(sep, 5)) != std::string::npos)
        message = buf.substr(i, (buf.find_first_of(sep, i) - i));
    std::string userAnswer;
    if (!message.empty())
        userAnswer = userOutput(FIND_USER(sd)) + buffer;
    else
        userAnswer = userOutput(FIND_USER(sd)) + "QUIT :leaving";
    disconnectUser(serv, sd);
    sendEveryone(userAnswer, serv->getUsers());
}