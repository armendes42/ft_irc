#include "main.hpp"

bool checkInvalidCharacter(char c)
{
    return (c == '\0' || c == '\a' || c == '\r' || c == '\n' || c == ' ' || c == ',' || c == ':');
}

bool channelNameInvalid(std::string name)
{
    if (name.size() > 50)
        return false;
    std::string::iterator it = name.begin();
    std::string channelId = "#&+";
    if (!name.empty() && channelId.find(name[0]) == std::string::npos)
        return false;
    for (; it != name.end() && !checkInvalidCharacter(*it); it++);
    return (it == name.end());
}

void join(Server *serv, std::string buffer, int sd)
{
    size_t i = 0, j = 0;
    std::string buf(buffer);
    std::string channelsName = "";
    if ((i = buf.find_first_not_of(sep, 5)) != std::string::npos)
        channelsName = buf.substr(i, ((j = buf.find_first_of(sep, i)) - i));
    if (channelsName.empty())
    {
        sendMessage(sendRplErr(461, serv, FIND_USER(sd), "JOIN", ""), sd);
        return;
    }
    int nbOfChannels = 1 + std::count(channelsName.begin(), channelsName.end(), ',');
    std::string keysForChannels = "";
    if (buf.find_first_not_of(sep, j) != std::string::npos)
    {
        j = buf.find_first_not_of(sep, j);
        keysForChannels = buf.substr(j, (buf.find_first_of(sep, j) - j));
    }
    for (int i = 0; i < nbOfChannels; i++)
    {
        std::string channelName = channelsName.substr(0, channelsName.find(","));
        channelsName.erase(0, channelsName.find(",") + 1);
        if (!(FIND_USER(sd)->getChannelsSize() < 10))
        {
            sendMessage(sendRplErr(405, serv, FIND_USER(sd), channelName, ""), sd);
            break;
        }
        if (!channelNameInvalid(channelName))
        {
            sendMessage(channelName + ":Erroneous Channel Name", sd);
            continue;
        }
        std::string key = keysForChannels.substr(0, keysForChannels.find(","));
        keysForChannels.erase(0, keysForChannels.find(",") + 1);
        if (serv->getChannels().find(channelName) == serv->getChannels().end())
        {
            Channel *chan = new Channel(channelName);
            serv->setChannels(channelName, chan);
        }
        if (FIND_CHANNEL(channelName)->getMode().find("b") != std::string::npos)
        {
            if (FIND_CHANNEL(channelName)->isBan(FIND_USER(sd)->getNickname()) == true)
            {
                sendMessage(sendRplErr(474, serv, FIND_USER(sd), channelName, ""), sd);
                continue;
            }
        }
        if (FIND_CHANNEL(channelName)->getMode().find("k") != std::string::npos)
        {
            if (key.empty())
            {
                sendMessage(sendRplErr(461, serv, FIND_USER(sd), "JOIN", ""), sd);
                return;
            }
            if (FIND_CHANNEL(channelName)->getKey().compare(key) != 0)
            {
                sendMessage(sendRplErr(475, serv, FIND_USER(sd), channelName, ""), sd);
                continue;
            }
        }
        if (FIND_CHANNEL(channelName)->getMode().find("l") != std::string::npos)
        {
            if (FIND_CHANNEL(channelName)->getMaxUser() <= FIND_CHANNEL(channelName)->getUsersNumber())
            {
                sendMessage(sendRplErr(471, serv, FIND_USER(sd), channelName, ""), sd);
                continue;
            }
        }
        //Adding client to server
        if (FIND_CHANNEL(channelName)->getUsersNumber() == 0)
            FIND_CHANNEL(channelName)->addChanops(sd, FIND_USER(sd));
        else if (FIND_CHANNEL(channelName)->searchUserByNickname(FIND_USER(sd)->getNickname()) != -1)
            return;
        else
            FIND_CHANNEL(channelName)->addUser(sd, FIND_USER(sd));
        FIND_USER(sd)->addChannel(channelName);
        std::string userAnswer = userOutput(FIND_USER(sd));
        userAnswer += "JOIN " + channelName;
        if (FIND_CHANNEL(channelName)->getMode().find("a") == std::string::npos)
            sendEveryoneInChan(userAnswer, FIND_CHANNEL(channelName));
        if (FIND_CHANNEL(channelName)->getTopic() == "")
            sendMessage(sendRplErr(331, serv, FIND_USER(sd), channelName, ""), sd);
        else
            sendMessage(sendRplErr(332, serv, FIND_USER(sd), channelName, FIND_CHANNEL(channelName)->getTopic()), sd);
        std::string listOfUser = FIND_CHANNEL(channelName)->getListOfUserInChan();
        if (FIND_CHANNEL(channelName)->getMode().find("a") == std::string::npos)
        {
            sendMessage(sendRplErr(353, serv, FIND_USER(sd), channelName, listOfUser), sd);
            sendMessage(sendRplErr(366, serv, FIND_USER(sd), channelName, ""), sd);
        }
        if (!FIND_CHANNEL(channelName)->getMode().empty())
            sendMessage(sendRplErr(324, serv, FIND_USER(sd), channelName, FIND_CHANNEL(channelName)->getMode()), sd);
    }
}
