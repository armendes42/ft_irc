#include "main.hpp"

void    part(Server *serv, std::string buffer, int sd)
{
    std::string buf(buffer);
    size_t i;
    std::string channelsName = "";
    if ((i = buf.find_first_not_of(sep, 5)) != std::string::npos)
        channelsName = buf.substr(i, (buf.find_first_of(sep, i) - i));
    if (channelsName.empty())
    {
        sendMessage(sendRplErr(461, serv, FIND_USER(sd), "PART", ""), sd);
        return;
    }
    std::string message = "";
    size_t j = i;
    j = buf.find_first_of(sep, j);
    j = buf.find_first_not_of(sep, j);
    int u = j;
    if ((j = buf.find_first_of(endBuf, j)) != std::string::npos)
        message = buf.substr(u, (buf.find_first_of(endBuf, u) - u));
    int nbOfChannels = 1 + std::count(channelsName.begin(), channelsName.end(), ',');
    for (int i = 0; i < nbOfChannels; i++)
    {
        std::string channelName = channelsName.substr(0, channelsName.find(","));
        channelsName.erase(0, channelsName.find(",") + 1);
        if (serv->getChannels().find(channelName) == serv->getChannels().end())
            sendMessage(sendRplErr(403, serv, FIND_USER(sd), channelName, ""), sd);
        else if (FIND_USER(sd)->getChannels().find(channelName) == FIND_USER(sd)->getChannels().end())
            sendMessage(sendRplErr(442, serv, FIND_USER(sd), channelName, ""), sd);
        else
        {
            std::string userAnswer = userOutput(FIND_USER(sd));
            userAnswer += "PART " + channelName + " " + message;
            if (FIND_CHANNEL(channelName)->getMode().find("a") == std::string::npos)
                sendEveryoneInChan(userAnswer, FIND_CHANNEL(channelName));
            else
                sendMessage(userAnswer, sd);
            FIND_CHANNEL(channelName)->leftUser(sd);
            if (FIND_CHANNEL(channelName)->getUsersNumber() == 0)
            {
                delete serv->getChannels().find(channelName)->second;
                serv->getChannels().erase(channelName);
            }
            FIND_USER(sd)->getChannels().erase(channelName);
        }
    }
}
