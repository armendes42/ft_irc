#include "main.hpp"

void mode_o(Server *serv, Channel *channel, std::string mode, std::string buffer, int sd)
{
    int i = 0;
    for (int j = 0; buffer[i] && j < 3; i++)
    {
        if (buffer[i] == ' ' || buffer[i] == '\t')
        {
            while (buffer[i] == ' ' || buffer[i] == '\t')
                i++;
            j++;
            i--;
        }
    }
    std::string name = buffer.substr(i, (buffer.find_first_of(sep, i) - i));
    int userSd = channel->searchUserByNickname(name);
    if (userSd == -1)
    {
        sendMessage(sendRplErr(441, serv, FIND_USER(sd), name, channel->getChannelName()), sd);
        return;
    }
    channel->leftUser(userSd);
    if (mode[0] == '-')
        channel->addUser(userSd, FIND_USER(userSd));
    else
        channel->addChanops(userSd, FIND_USER(userSd));
    std::string userAnswer = userOutput(FIND_USER(sd));
    if (channel->getMode().find('a') != std::string::npos)
        userAnswer = anonymousOutput();
    userAnswer += "MODE " + channel->getChannelName() + " " + mode + " " + name;
    sendMessage(userAnswer, userSd);
}

void mode_v(Server *serv, Channel *channel, std::string mode, std::string buffer, int sd)
{
    int i = 0;
    for (int j = 0; buffer[i] && j < 3; i++)
    {
        if (buffer[i] == ' ' || buffer[i] == '\t')
        {
            while (buffer[i] == ' ' || buffer[i] == '\t')
                i++;
            j++;
            i--;
        }
    }
    std::string name = buffer.substr(i, (buffer.find_first_of(sep, i) - i));
    int userSd = channel->searchUserByNickname(name);
    if (userSd == -1)
    {
        sendMessage(sendRplErr(441, serv, FIND_USER(sd), name, channel->getChannelName()), sd);
        return;
    }
    if (channel->isChanop(userSd) == true)
        return;
    channel->leftUser(userSd);
    if (mode[0] == '-')
        channel->addUser(userSd, FIND_USER(userSd));
    else
        channel->addVoices(userSd, FIND_USER(userSd));
    std::string userAnswer = userOutput(FIND_USER(sd));
    if (channel->getMode().find('a') != std::string::npos)
        userAnswer = anonymousOutput();
    userAnswer += "MODE " + channel->getChannelName() + " " + mode + " " + name;
    sendMessage(userAnswer, sd);
}

void mode_b(Server *serv, Channel *channel, std::string mode, std::string buffer, int sd)
{
    (void)mode;
    int i = 0;
    for (int j = 0; buffer[i] && j < 3; i++)
    {
        if (buffer[i] == ' ' || buffer[i] == '\t')
        {
            while (buffer[i] == ' ' || buffer[i] == '\t')
                i++;
            j++;
            i--;
        }
    }
    std::string name = buffer.substr(i, (buffer.find_first_of(sep, i) - i));
    if (name.empty())
    {
        std::string banlist = channel->getListOfUserBan();
        if (!banlist.empty())
            sendMessage(sendRplErr(367, serv, FIND_USER(sd), channel->getChannelName(), banlist), sd);
        sendMessage(sendRplErr(368, serv, FIND_USER(sd), channel->getChannelName(), ""), sd);
    }
	else
    {
        std::string nick = name.substr(0, name.find('!'));
        if (nick.length() > 1 || (nick.length() == 1 && nick[0] != '*'))
        {
            if (nick[0] == '*')
                name = &nick[1];
            else
                name = nick;
        }
        else
            return;
        if (mode[0] == '-')
            channel->getBanList().erase(name);
        else
            channel->getBanList().insert(std::make_pair(name, nick));
	}
}

void mode_k(Server *serv, Channel *channel, std::string mode, std::string buffer, int sd)
{
    (void)mode;
    (void)serv;
    if (mode[0] == '-')
    {
        channel->setKey("");
	    return;
    }
    int i = 0;
    for (int j = 0; buffer[i] && j < 3; i++)
    {
        if (buffer[i] == ' ' || buffer[i] == '\t')
        {
            while (buffer[i] == ' ' || buffer[i] == '\t')
                i++;
            j++;
            i--;
        }
    }
    std::string key = buffer.substr(i, (buffer.find_first_of(sep, i) - i));
    if (key.compare("x") == 0)
    {
        sendMessage(sendRplErr(467, serv, FIND_USER(sd), channel->getKey(), ""), sd);
    }
    else
    {
        if (!key.empty())
            channel->setKey(key);
        else
        {
            std::string truc = userOutput(FIND_USER(sd));
            sendMessage(truc + ": Wrong key!", sd);
        }
    }
}

void mode_l(Server *serv, Channel *channel, std::string mode, std::string buffer, int sd)
{
    (void)mode;
    (void)serv;
    if (mode[0] == '-')
    {
	    channel->setMaxUser(-1);
	    return;
    }
    int i = 0;
    for (int j = 0; buffer[i] && j < 3; i++)
    {
        if (buffer[i] == ' ' || buffer[i] == '\t')
        {
            while (buffer[i] == ' ' || buffer[i] == '\t')
                i++;
            j++;
            i--;
        }
    }
    std::string name = buffer.substr(i, buffer.find('\r') != std::string::npos ? buffer.length() - 2 - i : buffer.length() - 1 - i);
    int maxUser = std::strtoul(name.c_str(), NULL, 0);
    if (maxUser < 0)
    {
	    sendMessage("Max user key must be superior to 0.", sd);
	    return;
    }
    channel->setMaxUser(maxUser);
}

bool availableMode(char c, std::string availableMode)
{
    return (!(availableMode.find(c) == std::string::npos));
}

void channelMode(Server *serv, Channel *channel, std::string mode, int sd, std::string buffer)
{
    std::string channelMode = channel->getMode();
    int i;
    typedef void (*modes)(Server *, Channel *, std::string, std::string, int);

    std::map<char, modes> modehandler;

	modehandler.insert(std::make_pair('o', &mode_o));
	modehandler.insert(std::make_pair('v', &mode_v));
	modehandler.insert(std::make_pair('b', &mode_b));
    modehandler.insert(std::make_pair('k', &mode_k));
    modehandler.insert(std::make_pair('l', &mode_l));

    if (mode[0] == '-')
    {
        std::string deletedMode;
        for (i = 1; mode[i]; i++)
        {
            if (availableMode(mode[i], CHANNEL_MODE) == false)
	        {
		        std::string stringMode(1, mode[i]);
                sendMessage(sendRplErr(472, serv, FIND_USER(sd), stringMode, channel->getChannelName()), sd);
	        }
            else if (availableMode(mode[i], "ovbkl") == true)
	        {
		        modehandler[mode[i]](serv, channel, mode, buffer, sd);
	    	    if (mode[i] == 'k' || mode[i] == 'l')
			        deletedMode += mode[i];
	        }
	        else if (channelMode.find(mode[i]) != std::string::npos)
            {
                deletedMode += mode[i];
                channelMode.erase(channelMode.find(mode[i]), 1);
            }
        }
        channel->setMode(channelMode);
        std::string userAnswer = userOutput(FIND_USER(sd));
        if (channel->getMode().find("a") != std::string::npos)
            userAnswer = anonymousOutput();
        if (!deletedMode.empty())
            userAnswer += "MODE " + channel->getChannelName() + " -" + deletedMode;
        if (userAnswer.find("MODE") != std::string::npos)
            sendEveryoneInChan(userAnswer, channel);
    }
    else
    {
        std::string addedMode;
        for ((mode[0] != '+') ? i = 0 : i = 1; mode[i]; i++)
        {
            if (availableMode(mode[i], CHANNEL_MODE) == false)
            {
                std::string stringMode(1, mode[i]);
                sendMessage(sendRplErr(472, serv, FIND_USER(sd), stringMode, channel->getChannelName()), sd);
            }
            else if (availableMode(mode[i], "ovbkl") == true)
            {
                modehandler[mode[i]](serv, channel, mode, buffer, sd);
                if ((mode[i] == 'k' && channel->getKey() != "") || mode[i] == 'l')
                    addedMode += mode[i];
            }
            else if (channelMode.find(mode[i]) == std::string::npos)
                addedMode += mode[i];
        }
        channel->setMode(channelMode + addedMode);
        std::string userAnswer = userOutput(FIND_USER(sd));
        if (channel->getMode().find("a") != std::string::npos)
            userAnswer = anonymousOutput();
        if (!addedMode.empty())
            userAnswer += "MODE " + channel->getChannelName() + " +" + addedMode;
        if (userAnswer.find("MODE") != std::string::npos)
            sendEveryoneInChan(userAnswer, channel);
    }
}

void userMode(Server *serv, User *user, std::string mode, int sd)
{
    std::string userMode = user->getMode();
    int i;
    int userSd = serv->searchUserByNickname(user->getNickname());

    if (mode[0] == '-')
    {
        std::string deletedMode;
        for (i = 1; mode[i]; i++)
        {
            if (availableMode(mode[i], USER_MODE) == false)
                sendMessage(sendRplErr(501, serv, FIND_USER(sd), "", ""), sd);
            else if (userMode.find(mode[i]) != std::string::npos)
            {
                if (mode[i] == 'o' && user->getNickname() != FIND_USER(sd)->getNickname())
                    sendMessage(sendRplErr(481, serv, FIND_USER(sd), "", ""), sd);
                else if (mode[i] == 'r' && ((FIND_USER(sd)->getMode().find('r') != std::string::npos) || (FIND_USER(sd)->getMode().find('o') == std::string::npos )))
                    sendMessage(sendRplErr(481, serv, FIND_USER(sd), "", ""), sd);
                else
                {
                    deletedMode += mode[i];
                    userMode.erase(userMode.find(mode[i]), 1);
                }
            }
        }
        user->setMode(userMode);
        std::string userAnswer = userOutput(FIND_USER(userSd));
        if (!deletedMode.empty())
            userAnswer += "MODE " + user->getNickname() + " -" + deletedMode;
        if (userAnswer.find("MODE") != std::string::npos)
            sendMessage(userAnswer, userSd);
    }
    else
    {
        std::string addedMode;
        for ((mode[0] != '+') ? i = 0 : i = 1; mode[i]; i++)
        {
            if (availableMode(mode[i], USER_MODE) == false)
                sendMessage(sendRplErr(501, serv, FIND_USER(sd), "", ""), sd);
            else if (userMode.find(mode[i]) == std::string::npos)
            {
                if (mode[i] == 'o' && FIND_USER(sd)->getMode().find('o') == std::string::npos)
                    sendMessage(sendRplErr(481, serv, FIND_USER(sd), "", ""), sd);
                else if (mode[i] == 'r' && user->getNickname() == FIND_USER(sd)->getNickname())
                    addedMode += mode[i];
                else if (mode[i] == 'r' && ((FIND_USER(sd)->getMode().find('r') != std::string::npos) || (FIND_USER(sd)->getMode().find('o') == std::string::npos)))
                    sendMessage(sendRplErr(481, serv, FIND_USER(sd), "", ""), sd);
                else
                    addedMode += mode[i];
            }
        }
        user->setMode(userMode + addedMode);
        std::string userAnswer = userOutput(FIND_USER(userSd));
        if (!addedMode.empty())
            userAnswer += "MODE " + user->getNickname() + " +" + addedMode;
        if (userAnswer.find("MODE") != std::string::npos)
            sendMessage(userAnswer, userSd);
    }
}

void mode(Server *serv, std::string buffer, int sd)
{
    std::string buf(buffer);
    int j = 0;
    size_t i;
    std::string msgtarget = "";
    if ((i = buf.find_first_not_of(sep, 5)) != std::string::npos)
        msgtarget = buf.substr(i, ((j = buf.find_first_of(sep, i)) - i));
    if (msgtarget.empty())
    {
        sendMessage(sendRplErr(461, serv, FIND_USER(sd), "MODE", ""), sd);
        return;
    }
    std::string idOfChannel = "#&+";
    j = buf.find_first_not_of(sep, j);
    std::string mode = "";
    if (buf.find_first_of(sep, j) != std::string::npos)
        mode = buf.substr(j, (buf.find_first_of(sep, j) - j));
    if (!msgtarget.empty() && idOfChannel.find(msgtarget[0]) != std::string::npos)
    {
        if (serv->getChannels().find(msgtarget) == serv->getChannels().end())
            sendMessage(sendRplErr(403, serv, FIND_USER(sd), msgtarget, ""), sd);
        else if (FIND_USER(sd)->getMode().find('r') != std::string::npos)
            sendMessage(sendRplErr(484, serv, FIND_USER(sd), "", ""), sd);
        else if (FIND_CHANNEL(msgtarget)->getChanops().find(sd) == FIND_CHANNEL(msgtarget)->getChanops().end())
            sendMessage(sendRplErr(482, serv, FIND_USER(sd), msgtarget, ""), sd);
        else
            channelMode(serv, FIND_CHANNEL(msgtarget), mode, sd, buffer);
    }
    else
    {
        int userSd;
        if ((userSd = serv->searchUserByNickname(msgtarget)) == -1)
            sendMessage(sendRplErr(401, serv, FIND_USER(sd), msgtarget, ""), sd);
        else if (mode.empty() && !FIND_USER(userSd)->getMode().empty())
            sendMessage(sendRplErr(221, serv, FIND_USER(userSd), '+' + FIND_USER(userSd)->getMode(), ""), sd);
        else if (mode.empty() && FIND_USER(userSd)->getMode().empty())
            sendMessage("This user don't have any modes!", sd);
        else
            userMode(serv, FIND_USER(userSd), mode, sd);
    }
    
}
