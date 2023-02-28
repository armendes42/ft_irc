#include "../../includes/main.hpp"

IRCbot::IRCbot() : _name("ArmendestheBOT")
{
	this->_commandhandler.insert(std::pair<std::string, command>("wakeup", &connect));
    this->_commandhandler.insert(std::pair<std::string, command>("help", &help));
    this->_commandhandler.insert(std::pair<std::string, command>("joke", &makeJoke));
    this->_commandhandler.insert(std::pair<std::string, command>("date", &giveDate));
    this->_commandhandler.insert(std::pair<std::string, command>("time", &giveTime));
    this->_commandhandler.insert(std::pair<std::string, command>("hangman", &startHangmanGame));
    this->_commandhandler.insert(std::pair<std::string, command>("sleep", &quit));
}

IRCbot::~IRCbot()
{
    this->_commandhandler.clear();
    this->_hangmanWords.clear();
}

std::string IRCbot::getName() const
{
    return this->_name;
}


void IRCbot::findCmd(Server * serv, Channel *chan, int sd, std::string cmd)
{
    if (_commandhandler.find(cmd) != _commandhandler.end())
        this->_commandhandler[cmd](serv, chan, sd);
    else
        sendMessage(sendRplErr(669, serv, FIND_USER(sd), cmd, ""), sd);
}

void connect(Server *serv, Channel *chan, int sd)
{
    if (chan->isChanop(sd))
    {
        if (chan && chan->getBot() == false)
        {
            chan->setBot();
            sendEveryoneInChan(":" + serv->getBot()->getName() + " JOIN " + chan->getChannelName(), chan);
        }
        else
            sendMessage(":" + serv->getBot()->getName() + " PRIVMSG " + chan->getChannelName() + " :I am already awake.", sd);
    }
    else
        sendMessage(sendRplErr(482, serv, FIND_USER(sd), chan->getChannelName(), ""), sd);
}

void quit(Server *serv, Channel *chan, int sd)
{
    if (chan->isChanop(sd))
    {
        if (chan && chan->getBot() == true)
        {
            chan->setBot();
            sendEveryoneInChan(":" + serv->getBot()->getName() + " PART " + chan->getChannelName(), chan);
        }
        else
            sendMessage(":" + serv->getBot()->getName() + " PRIVMSG " + chan->getChannelName() + " :I am sleeping right now, wake me up to play with me.", sd);
    }
    else
        sendMessage(sendRplErr(482, serv, FIND_USER(sd), chan->getChannelName(), ""), sd);
}

void makeJoke(Server *serv, Channel *chan, int sd)
{
    int lines = 1;
    if (chan && chan->getBot() == true)
    {
        srand(time(NULL));
        time_t delay = getTime() - chan->getLastJoke();
        if (delay >= 15)
        {
            std::string line;
            std::ifstream file("bot/srcs/blagues.txt");
            if (file.is_open())
            {
                while (std::getline(file, line))
                    lines++;
                file.close();
                int random = rand() % lines;
                std::ifstream file("bot/srcs/blagues.txt");
                if (file.is_open())
                {
                    for (int i = 0; i < lines; i++)
                    {
                        std::getline(file, line);
                        if (i == random)
                            break;
                    }
                    file.close();
                }
            }
            sendEveryoneInChan(":" + serv->getBot()->getName() + " PRIVMSG " + chan->getChannelName() + " :" + line, chan);
            chan->setLastJoke();
        }
        else
        {
            long a = 15 - delay;
            std::stringstream ss;
            ss << a;
            sendMessage(":" + serv->getBot()->getName() + " PRIVMSG " + chan->getChannelName() + " :You have to wait " + ss.str() + " seconds before sending another joke!", sd);
        }
    }
    else
        sendMessage(":" + serv->getBot()->getName() + " PRIVMSG " + chan->getChannelName() + " :I am sleeping right now, wake me up to play with me.", sd);
}

void giveTime(Server *serv, Channel *chan, int sd)
{
    if (chan && chan->getBot() == true)
    {
        std::time_t t = std::time(0);
        std::tm* now = std::localtime(&t);
        std::stringstream sshour;
        sshour << now->tm_hour;
        std::stringstream ssmin;
        ssmin << now->tm_min;
        sendMessage(":" + serv->getBot()->getName() + " PRIVMSG " + chan->getChannelName() + " :It is " + sshour.str() + ":" + ssmin.str() + ".", sd);
    }
    else
        sendMessage(":" + serv->getBot()->getName() + " PRIVMSG " + chan->getChannelName() + " :I am sleeping right now, wake me up to play with me.", sd);

}

void giveDate(Server *serv, Channel *chan, int sd)
{
    if (chan && chan->getBot() == true)
    {
        std::string months[12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
        std::time_t t = std::time(0);
        std::tm* now = std::localtime(&t);
        std::stringstream ssyear;
        ssyear << now->tm_year + 1900;
        std::stringstream ssmday;
        if (now->tm_mday == 1)
            ssmday << now->tm_mday << "st";
        else if (now->tm_mday == 2)
            ssmday << now->tm_mday << "nd";
        else if (now->tm_mday == 3)
            ssmday << now->tm_mday << "rd";
        else
            ssmday << now->tm_mday << "th";

        sendMessage(":" + serv->getBot()->getName() + " PRIVMSG " + chan->getChannelName() + " :Today we are on " + months[now->tm_mon] + " " + ssmday.str() + " of " + ssyear.str() + ".", sd);
    }
    else
        sendMessage(":" + serv->getBot()->getName() + " PRIVMSG " + chan->getChannelName() + " :I am sleeping right now, wake me up to play with me.", sd);
}

void help(Server *serv, Channel *chan, int sd)
{
    std::string line = "!wakeup  - Connect the bot to the channel.";
    sendMessage(":" + serv->getBot()->getName() + " PRIVMSG " + chan->getChannelName() + " :" + line, sd);
    line = "!joke    - Let the bot make a joke.";
    sendMessage(":" + serv->getBot()->getName() + " PRIVMSG " + chan->getChannelName() + " :" + line, sd);
    line = "!date    - Give today's date.";
    sendMessage(":" + serv->getBot()->getName() + " PRIVMSG " + chan->getChannelName() + " :" + line, sd);
    line = "!time    - Give the current local time.";
    sendMessage(":" + serv->getBot()->getName() + " PRIVMSG " + chan->getChannelName() + " :" + line, sd);
    line = "!hangman - Play the hangman game.";
    sendMessage(":" + serv->getBot()->getName() + " PRIVMSG " + chan->getChannelName() + " :" + line, sd);
    line = "!sleep   - Disconnect the bot.";
    sendMessage(":" + serv->getBot()->getName() + " PRIVMSG " + chan->getChannelName() + " :" + line, sd);
}

void IRCbot::insertWord(std::string message, std::string messageGuess, int sd)
{
    std::pair<std::string, int> pr = std::pair<std::string, int>(messageGuess, 10);
    this->_hangmanWords.insert(std::pair<int, std::pair<std::string, std::pair<std::string, int > > >(sd, std::pair<std::string, std::pair<std::string, int> >(message, pr)));
}

Channel *userInChanBot(Server *serv, User *user)
{
    for (std::set<std::string>::iterator it = user->getChannels().begin(); it != user->getChannels().end(); it++)
        if (FIND_CHANNEL(*it)->getBot() == true)
            return FIND_CHANNEL(*it);
    return NULL;
}

void IRCbot::hangmanGame(Server *serv, std::string str, int sd)
{
    Channel *tmp;
    if (str.compare("play") == 0 && (tmp = userInChanBot(serv, FIND_USER(sd))) != NULL)
        startHangmanGame(serv, tmp, sd);
    else if (this->_hangmanWords.find(sd) != this->_hangmanWords.end())
    {
        if (str.size() != 1)
            sendMessage(":" + serv->getBot()->getName() + " PRIVMSG " + FIND_USER(sd)->getNickname() + " :You have to put only one character.", sd);
        else if (str[0] < 65 || (str[0] > 90 && str[0] < 97) || str[0] > 122)
            sendMessage(":" + serv->getBot()->getName() + " PRIVMSG " + FIND_USER(sd)->getNickname() + " :You have to put a letter (Capital or not).", sd);
        else
        {
            char c = str[0];
            std::string message = this->_hangmanWords.find(sd)->second.first;
            std::string &messageGuess = this->_hangmanWords.find(sd)->second.second.first;
            int &life = this->_hangmanWords.find(sd)->second.second.second;
            if (message.find(tolower(c)) != std::string::npos || message.find(toupper(c)) != std::string::npos)
            {
                if (messageGuess.find(c) != std::string::npos)
                    sendMessage(":" + this->_name + " PRIVMSG " + FIND_USER(sd)->getNickname() + " :You already found this letter.", sd);
                else
                {
                    for (int i = 0; message[i]; i++)
                        if (toupper(message[i]) == toupper(c))
                            messageGuess[i] = message[i];
                    sendMessage(":" + this->_name + " PRIVMSG " + FIND_USER(sd)->getNickname() + " :Nice!", sd);
                }
            }
            else
            {
                life--;
                if (life == 0)
                {
                    sendMessage(":" + this->_name + " PRIVMSG " + FIND_USER(sd)->getNickname() + " :You lost the game, the word was : " + message + ".", sd);
                    this->_hangmanWords.erase(sd);
                    return;
                }
                else
                {
                    std::stringstream ss;
                    ss << life;
                    if (life > 1)
                        sendMessage(":" + this->_name + " PRIVMSG " + FIND_USER(sd)->getNickname() + " :This character is not in the word... You have " + ss.str() + " tries left.", sd);
                    else
                        sendMessage(":" + this->_name + " PRIVMSG " + FIND_USER(sd)->getNickname() + " :This character is not in the word... You have " + ss.str() + " try left.", sd);
                }
            }
            if (messageGuess.find('_') == std::string::npos)
            {
                sendMessage(":" + this->_name + " PRIVMSG " + FIND_USER(sd)->getNickname() + " :Congratulation! You found the word : " + messageGuess, sd);
                this->_hangmanWords.erase(sd);
            }
            else
                sendMessage(":" + this->_name + " PRIVMSG " + FIND_USER(sd)->getNickname() + " :Your word is : " + messageGuess, sd);
        }
    }
    else
        sendMessage(":" + this->_name + " PRIVMSG " + FIND_USER(sd)->getNickname() + " :You didnt start the game yet... Type \"play\" to start another game.", sd);
}

void startHangmanGame(Server *serv, Channel *chan, int sd)
{
    int lines = 1;
    if (chan && chan->getBot() == true)
    {
        srand(time(NULL));
        std::string word;
        std::ifstream file("bot/srcs/mots.txt");
        if (file.is_open())
        {
            while (std::getline(file, word))
                lines++;
            file.close();
            int random = rand() % lines;
            std::ifstream file("bot/srcs/mots.txt");
            if (file.is_open())
            {
                for (int i = 0;std::getline(file, word) && i < random; i++);
                file.close();
            }
        }
        std::string result;
        result.insert(0, word.size(), '_');
        serv->getBot()->insertWord(word, result, sd);
        sendMessage(":" + serv->getBot()->getName() + " PRIVMSG " + FIND_USER(sd)->getNickname() + " :You are playing Hangman Game, enter a letter to start the game.", sd);
    }
    else
        sendMessage(":" + serv->getBot()->getName() + " PRIVMSG " + chan->getChannelName() + " :I am sleeping right now, wake me up to play with me.", sd);
}
