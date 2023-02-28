#pragma once

#include "main.hpp"

class User;

class Channel
{
    private:
        std::map<int, User*> _users;
        std::map<int, User*> _chanops;
        std::map<int, User*> _voices;
        std::string _channelName;
        std::string _topic;
        std::string _mode;
        std::string _key;
        int _maxUser;
        bool _botInchan;
        std::map<std::string, std::string> _banList;
        time_t _lastJoke;

    public:
        Channel(std::string channelName);
        ~Channel();
        std::map<int, User*> &getUsers();
        std::map<int, User*> &getChanops();
        std::map<int, User*> &getVoices();
        std::string getTopic() const;
        std::string getChannelName() const;
        std::map<std::string, std::string> &getBanList();
        int getUsersNumber() const;
        std::string getMode() const;
        std::string getKey() const;
        int getMaxUser() const;
        bool getBot() const;
        time_t getLastJoke() const;
        void setTopic(std::string topic);
        void setMode(std::string mode);
        void setKey(std::string key);
        void setMaxUser(int maxUser);
        void setBot();
        void setLastJoke();
        void addUser(int sd, User *user);
        void addChanops(int sd, User *user);
        void addVoices(int sd, User *user);
        void leftUser(int sd);
        int searchUserByNickname(std::string nickname);
        std::string getListOfUserInChan();
        std::string getListOfUserBan();
        bool isChanop(int sd);
        bool isVoices(int sd);
        bool isBan(std::string nickname);
};
