#include "main.hpp"

void kick(Server *serv, std::string buffer, int sd)
{
    std::string buf(buffer);
    int kickCount = std::count(buf.begin(), buf.end(), '\n');
    int k = 0;
    size_t i, j;
    std::string channelsName = "";
    if ((i = buf.find_first_not_of(sep, 5)) != std::string::npos)
        channelsName = buf.substr(i, ((j = buf.find_first_of(sep, i)) - i));
    if (channelsName.empty())
    {
        sendMessage(sendRplErr(461, serv, FIND_USER(sd), "KICK", ""), sd);
        return;
    }
    int nbOfChannels = 1 + std::count(channelsName.begin(), channelsName.end(), ',');
    j = buf.find_first_not_of(sep, j);
    k = j;
    for (int l = 0; l < kickCount; l++)
    {
        std::string usersNick = buf.substr(k, (buf.find_first_of(sep, k) - k));
        int nbUsers = 1 + std::count(usersNick.begin(), usersNick.end(), ',');
        if (usersNick.empty())
        {
            sendMessage(sendRplErr(461, serv, FIND_USER(sd), "KICK", ""), sd);
            return;
        }
        for (int j = 0; j < nbUsers; j++)
        {
            std::string userNick = usersNick.substr(0, usersNick.find(","));
            usersNick.erase(0, usersNick.find(",") + 1);
            for (int i = 0; i < nbOfChannels; i++)
            {
                std::string tmp = channelsName;
                std::string channelName = tmp.substr(0, tmp.find(","));
                tmp.erase(0, tmp.find(",") + 1);
                if (serv->getChannels().find(channelName) == serv->getChannels().end())
                {
                    if (j == 0)
                        sendMessage(sendRplErr(403, serv, FIND_USER(sd), channelName, ""), sd);
                    continue;
                }
                if (FIND_CHANNEL(channelName)->getMode().find("a") != std::string::npos)
                    continue;
                if (FIND_CHANNEL(channelName)->searchUserByNickname(FIND_USER(sd)->getNickname()) == -1)
                {
                    sendMessage(sendRplErr(442, serv, FIND_USER(sd), channelName, ""), sd);
                    continue;
                }
                if (FIND_USER(sd)->getMode().find('r') != std::string::npos)
                {
                    sendMessage(sendRplErr(484, serv, FIND_USER(sd), "", ""), sd);
                    continue ;
                }
                if (!FIND_CHANNEL(channelName)->isChanop(sd))
                {
                    sendMessage(sendRplErr(482, serv, FIND_USER(sd), channelName, ""), sd);
                    continue ;
                }
                int userToKickSd;
                if ((userToKickSd = FIND_CHANNEL(channelName)->searchUserByNickname(userNick)) == -1)
                {
                    sendMessage(sendRplErr(441, serv, FIND_USER(sd), userNick, channelName), sd);
                    continue;
                }
                std::string userAnswer = userOutput(FIND_USER(userToKickSd));
                userAnswer += "PART " + channelName;
                sendEveryoneInChan(userAnswer, FIND_CHANNEL(channelName));
                FIND_CHANNEL(channelName)->leftUser(userToKickSd);
                if (FIND_CHANNEL(channelName)->getUsersNumber() == 0)
                {
                    delete serv->getChannels().find(channelName)->second;
                    serv->getChannels().erase(channelName);
                }
                FIND_USER(userToKickSd)->getChannels().erase(channelName);
            }
        }
        if (buf.find('\r') != std::string::npos)
            buf.erase(0, buf.find("\r\n") + 2);
        else
            buf.erase(0, buf.find('\n') + 1);
    }
}
