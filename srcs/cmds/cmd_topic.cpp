#include "main.hpp"

void topic(Server *serv, std::string buffer, int sd)
{
    std::string buf(buffer);
    size_t i, j = 0;
    std::string channelName = "";
    if ((i = buf.find_first_not_of(sep, 6)) != std::string::npos)
        channelName = buf.substr(i, ((j = buf.find_first_of(sep, i)) - i));
    if (channelName.empty())
        sendMessage(sendRplErr(461, serv, FIND_USER(sd), "TOPIC", ""), sd);
    else if (serv->getChannels().find(channelName) == serv->getChannels().end())
        sendMessage(sendRplErr(403, serv, FIND_USER(sd), channelName, ""), sd);
    else if (FIND_USER(sd)->getChannels().find(channelName) == FIND_USER(sd)->getChannels().end())
        sendMessage(sendRplErr(442, serv, FIND_USER(sd), channelName, ""), sd);
    else if ((FIND_CHANNEL(channelName)->getMode().find("t") != std::string::npos) && (FIND_USER(sd)->getMode().find('r') != std::string::npos))
        sendMessage(sendRplErr(484, serv, FIND_USER(sd), "", ""), sd);
    else if ((FIND_CHANNEL(channelName)->getMode().find("t") != std::string::npos) && (FIND_CHANNEL(channelName)->getChanops().find(sd) == FIND_CHANNEL(channelName)->getChanops().end()))
        sendMessage(sendRplErr(482, serv, FIND_USER(sd), channelName, ""), sd);
    else
    {
        std::string topic = "";
        if ((j = buf.find_first_not_of(sep, j)) != std::string::npos)
        {
            if (buf[j] == ':')
                j++;
            topic = buf.substr(j, (buf.find_first_of(endBuf, j) - j));
        }
        if (topic.empty())
            sendMessage(sendRplErr(332, serv, FIND_USER(sd), channelName, FIND_CHANNEL(channelName)->getTopic()), sd);
        else
        {
            FIND_CHANNEL(channelName)->setTopic(topic);
            sendMessage(sendRplErr(332, serv, FIND_USER(sd), channelName, FIND_CHANNEL(channelName)->getTopic()), sd);
            std::string userAnswer = userOutput(FIND_USER(sd));
            if (FIND_CHANNEL(channelName)->getMode().find("a") != std::string::npos)
                userAnswer = anonymousOutput();
            userAnswer += "TOPIC " + FIND_CHANNEL(channelName)->getTopic();
            sendEveryoneInChan(userAnswer, FIND_CHANNEL(channelName));
        }
    }
}