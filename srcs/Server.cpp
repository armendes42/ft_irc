#include "main.hpp"

int clientSocket[maxClients];
bool isAlive = true;

Server::Server(const std::string &port, const std::string &password) : _port(port), _password(password), _serverName(), _isRestart(false){
	this->_commandhandler.insert(std::pair<std::string, command>("NICK", &nick));
	this->_commandhandler.insert(std::pair<std::string, command>("JOIN", &join));
	this->_commandhandler.insert(std::pair<std::string, command>("PRIVMSG", &privmsg));
	this->_commandhandler.insert(std::pair<std::string, command>("NOTICE", &privmsg));
	this->_commandhandler.insert(std::pair<std::string, command>("PING", &ping));
	this->_commandhandler.insert(std::pair<std::string, command>("PONG", &pong));
	this->_commandhandler.insert(std::pair<std::string, command>("PART", &part));
	this->_commandhandler.insert(std::pair<std::string, command>("TOPIC", &topic));
	this->_commandhandler.insert(std::pair<std::string, command>("KICK", &kick));
	this->_commandhandler.insert(std::pair<std::string, command>("MODE", &mode));
	this->_commandhandler.insert(std::pair<std::string, command>("OPER", &oper));
	this->_commandhandler.insert(std::pair<std::string, command>("kill", &kill));
	this->_commandhandler.insert(std::pair<std::string, command>("KILL", &kill));
	this->_commandhandler.insert(std::pair<std::string, command>("QUIT", &quit));
	this->_commandhandler.insert(std::pair<std::string, command>("RESTART", &restart));
	this->_commandhandler.insert(std::pair<std::string, command>("restart", &restart));
	this->_bot = new IRCbot;
}

Server::~Server()
{
	this->_commandhandler.clear();
	delete this->_bot;
}

void sendMOTD(int sd)
{
	sendMessage(" ___   ___    _______     ", sd);
	sendMessage("|\\  \\ |\\  \\  /  ___  \\   ", sd);
	sendMessage("\\ \\\\  \\_\\  \\/__/|_/  /|   ", sd);
	sendMessage(" \\ \\______  \\__|//  / /   ", sd);
	sendMessage("  \\|_____|\\  \\  /  /_/__  ", sd);
	sendMessage("         \\ \\__\\|\\________\\", sd);
	sendMessage("          \\|__| \\|_______|", sd);
	sendMessage("", sd);
}

int Server::newSocket()
{
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
		throw std::runtime_error("Error creating socket.\n");
	int tmp = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &tmp, sizeof(tmp)))
		throw std::runtime_error("Error while setting up socket.\n");
	if (fcntl(sock, F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error("Error while setting socket NON-BLOCKING mode.\n");
	_server.sin_addr.s_addr = INADDR_ANY;
	_server.sin_port = htons(static_cast<unsigned short>(std::strtoul(this->_port.c_str(), NULL, 0)));
	_server.sin_family = AF_INET;
	if (bind(sock, (const struct sockaddr*)&_server, sizeof(_server)) < 0)
		throw std::runtime_error("Error binding socket.\n");
	if (listen(sock, 10) < 0)
		throw std::runtime_error("Error listening on socket.\n");
	return sock;
}

void Server::newConnection(void)
{
	socklen_t csize = sizeof(_server);
	if ((this->_sockcom = accept(this->_sockserver, (struct sockaddr *)&_server, &csize)) < 0)
	{
		perror("accept");
		exit(EXIT_FAILURE);
	}
	//inform user of socket number - used in send and receive commands
	std::cout << "New connection , socket fd is " << this->_sockcom << " , ip is : " << inet_ntoa(_server.sin_addr) << " , port : " <<  ntohs(_server.sin_port) << std::endl;
	std::string ret;
	//send new connection greeting message
	size_t occ;
	size_t firstOcc;
	bool isPassGood = false, isNickGood = false, isUserGood = false;
	std::string nick = "", user = "", host = "", serverName = "", realName = "", pass = "";

	ret = this->receiveMessage(this->_sockcom);
	if (((ret.find("CAP LS") != std::string::npos && ret.find("PASS ") == std::string::npos) || (ret.find("CAP LS") != std::string::npos && ret.find("PASS ") == std::string::npos && ret.find("NICK ") != std::string::npos)) && ret.find("USER ") == std::string::npos)
		ret = this->receiveMessage(this->_sockcom);
	if ((occ = ret.find("PASS ")) != std::string::npos)
	{
		if ((firstOcc = ret.find_first_not_of(sep, occ + 5)) == std::string::npos)
		{
			sendMessage(sendRplErr(461, this, NULL, "PASS", ""), this->_sockcom);
			close(this->_sockcom);
		}
		else
		{
			for (int i = 0; ret[firstOcc + i] && sep.find(ret[firstOcc + i]) == std::string::npos; i++)
				pass += ret[firstOcc + i];
			if (pass.empty())
			{
				sendMessage(sendRplErr(461, this, NULL, "PASS", ""), this->_sockcom);
				close(this->_sockcom);
			}
			else if (pass.compare(this->_password) != 0)
			{
				sendMessage("WRONG PASSWORD", this->_sockcom);
				close(this->_sockcom);
			}
			else
				isPassGood = true;
		}
	}
	else
	{
		sendMessage("You need to enter a pass!", this->_sockcom);
		close(this->_sockcom);
	}
	if (isPassGood == true)
	{
		if (ret.find("NICK ") == std::string::npos)
			ret = this->receiveMessage(this->_sockcom);
		if ((occ = ret.find("NICK ")) != std::string::npos)
		{
			if ((firstOcc = ret.find_first_not_of(sep, occ + 5)) == std::string::npos)
			{
				sendMessage(sendRplErr(432, this, NULL, nick, ""), this->_sockcom);
				close(this->_sockcom);
			}
			else
			{
				nick = ret.substr(firstOcc, ret.find_first_of(endBuf, firstOcc) - firstOcc);
				nick = nick.substr(0, nick.find_last_not_of(sep, nick.size()) + 1);
				if (!nicknameIsValid(nick))
				{
					sendMessage(sendRplErr(432, this, NULL, nick, ""), this->_sockcom);
					close(this->_sockcom);	
				}
				else if (nicknameIsInUse(this, nick))
				{
					sendMessage(sendRplErr(433, this, NULL, nick, ""), this->_sockcom);
					sendMessage("Please try reconnect with an available nickname.", this->_sockcom);
					close(this->_sockcom);
				}
				else
					isNickGood = true;
			}
		}
		else
		{
			sendMessage("You have to enter a nickname\nUsage: NICK [nickname]", this->_sockcom);
			close(this->_sockcom);
		}
		if (isUserGood == false && isNickGood == true)
		{
			if (ret.find("USER ") == std::string::npos)
				ret = this->receiveMessage(this->_sockcom);
			if ((occ = ret.find("USER ")) != std::string::npos)
			{
				int i = 0;
				//username
				if ((firstOcc = ret.find_first_not_of(sep, occ + 5)) == std::string::npos)
					sendMessage(sendRplErr(461, this, NULL, "USER", ""), this->_sockcom);
				else
				{
					user = ret.substr(firstOcc, (i = ret.find_first_of(sep, firstOcc)) - firstOcc);
					//hostname
					if ((firstOcc = ret.find_first_not_of(sep, i)) == std::string::npos)
						sendMessage(sendRplErr(461, this, NULL, "USER", ""), this->_sockcom);
					else
					{
						host = ret.substr(firstOcc, (i = ret.find_first_of(sep, firstOcc)) - firstOcc);
						//serverName
						if ((firstOcc = ret.find_first_not_of(sep, i)) == std::string::npos)
							sendMessage(sendRplErr(461, this, NULL, "USER", ""), this->_sockcom);
						else
						{
							serverName = ret.substr(firstOcc, (i = ret.find_first_of(sep, firstOcc)) - firstOcc);
							//realName
							if ((firstOcc = ret.find_first_not_of(sep, i)) == std::string::npos)
								sendMessage(sendRplErr(461, this, NULL, "USER", ""), this->_sockcom);
							else
							{
								realName = ret.substr(firstOcc, (i = ret.find_first_of(sep, firstOcc)) - firstOcc);
								realName = realName.substr(0, realName.find_last_not_of(sep, realName.size()) + 1);
							}
						}
					}
				}
				if (!(user.empty() || host.empty() || serverName.empty() || realName.empty()))
					isUserGood = true;
			}
		}
		if (isUserGood == false && isNickGood == true)
		{
			sendMessage("Usage: USER [username] [hostname] [serverName] [realName]", this->_sockcom);
			close(this->_sockcom);
		}
	}
	if (isPassGood == true && _users.size() < 10 && isNickGood == true && isUserGood == true && isAlive == true)
	{
		this->_serverName = serverName;
		User *newUser = new User(nick, user, host, realName);
		this->setUsers(this->_sockcom, newUser);
		std::cout << "Number of user connected on the server: " << this->_users.size() << std::endl;
		sendMessage(sendRplErr(001, this, newUser, "", ""), this->_sockcom);
		sendMessage(sendRplErr(002, this, newUser, "", ""), this->_sockcom);
		sendMessage(sendRplErr(003, this, newUser, "", ""), this->_sockcom);
		sendMessage(sendRplErr(004, this, newUser, "", ""), this->_sockcom);
		sendMOTD(this->_sockcom);
		//add new socket to array of sockets
		for (int i = 0; i < maxClients; i++)
		{
			//if position is empty
			if (clientSocket[i] == 0)
			{
				clientSocket[i] = this->_sockcom;
				break;
			}
		}
	}
	else if (isPassGood == true && isNickGood == true && isAlive == true && isUserGood == true)
		sendMessage(sendRplErr(005, this, NULL, nick, ""), this->_sockcom);
}

void    handler(int signum)
{
	(void)signum;
	isAlive = false;
}

void Server::connectToServer()
{
	this->_sockserver = newSocket();
	fd_set readfds;
	int sd, activity, maxSd;
	for (int i = 0; i < maxClients; i++)
        clientSocket[i] = 0;
	std::cout << "listening..." << std::endl;
	while (this->_isRestart == false && isAlive == true)
	{
    	std::signal(SIGINT, handler);
		//clear the socket set
        FD_ZERO(&readfds);
        //add master socket to set
        FD_SET(this->_sockserver, &readfds);
        maxSd = this->_sockserver;
        //add child sockets to set
        for (int i = 0; i < maxClients; i++)
        {
            //socket descriptor
            sd = clientSocket[i];
            //if valid socket descriptor then add to read list
            if (sd > 0)
                FD_SET(sd , &readfds);
            //highest file descriptor number, need it for the select function
            if (sd > maxSd)
                maxSd = sd;
        }
        //wait for an activity on one of the sockets , timeout is NULL ,
        //so wait indefinitely
        activity = select(maxSd + 1 , &readfds , NULL , NULL , NULL);
        if ((activity < 0) && (errno != EINTR))
            std::cerr << ("select error") << std::endl;
		//connect new user
		if (FD_ISSET(this->_sockserver, &readfds) && isAlive == true)
            newConnection();
		else if (isAlive == true)
		{
			for (int i = 0; i < maxClients; i++)
			{
				sd = clientSocket[i];
				if (FD_ISSET(sd , &readfds))
				{
					std::string buf;
					//Check if it was for closing , and also read the
					//incoming message
					buf = receiveMessage(sd);
					if (!buf.empty())
					{
						std::cout << "\033[1;34mCOMMAND RECEIVED :\033[0m " << buf;
						std::string command(buf);
						int occ = buf.find_first_not_of(sep, 0);
						buf = command.substr(occ, buf.length() - occ);
						command = buf.substr(0, buf.find_first_of(sep, 0));
						if (_commandhandler.find(command) != _commandhandler.end())
							(_commandhandler[command])(this, buf, sd);
						break;
					}
				}
			}
		}
	}
	clearAll();
	for (int i = 0; i < maxClients; i++)
	{
		if (clientSocket[i] != 0)
		{
			close(clientSocket[i]);
			clientSocket[i] = 0;
		}
	}
	close(this->_sockserver);
	if (this->_isRestart == true && isAlive == true)
	{
		this->_isRestart = false;
		std::cout << "SERVER RESTARTING..." << std::endl;
		connectToServer();
	}
}

std::string Server::receiveMessage(int sd) const
{
	char buffer[1024];
	std::string buf = "";
	memset(buffer, 0, 1024);
	while ((buf += buffer).find('\n') == std::string::npos && isAlive == true)
		if (recv(sd, buffer, 1024, 0) < 0)
			throw std::runtime_error("Error receiving message");
	return buf;
}

std::string Server::getServerName() const
{
	return this->_serverName;
}

std::string Server::getPort() const
{
	return this->_port;
}

std::map<std::string, Channel*> & Server::getChannels()
{
	return this->_channels;
}

std::map<int, User*> & Server::getUsers()
{
	return this->_users;
}

struct sockaddr_in Server::getServer()
{
	return this->_server;
}

IRCbot* Server::getBot() const
{
	return this->_bot;
}

void Server::setChannels(std::string channelName, Channel *chan)
{
	this->_channels.insert(std::make_pair(channelName, chan));
}

void Server::setUsers(int sd, User *user)
{
	this->_users.insert(std::make_pair(sd, user));
}

void Server::setIsRestart()
{
	this->_isRestart = !this->_isRestart;
}

int Server::searchUserByNickname(std::string nickname)
{
    for (std::map<int, User*>::iterator it = this->_users.begin(); it != this->_users.end(); it++)
    {
        if (nickname.compare(it->second->getNickname()) == 0)
            return it->first;
    }
    return (-1);
}

std::ostream	&operator<<(std::ostream &stdout, std::map<std::string, Channel*> &channels)
{
	int i = 0;
	for (std::map<std::string, Channel*>::iterator it = channels.begin(); it != channels.end(); it++, i++)
	{
		stdout << "Channel " << i << " name is " << it->first << "." << std::endl;
	}
	return (stdout);
}

std::ostream	&operator<<(std::ostream &stdout, std::map<int, User*> &users)
{
	int i = 0;
	for (std::map<int, User*>::iterator it = users.begin(); it != users.end(); it++, i++)
	{
		stdout << "User " << i << " nick is " << it->second->getNickname() << "." << std::endl;
	}
	return (stdout);
}

std::ostream	&operator<<(std::ostream &stdout, User &user)
{
    int i = 0;
    std::set<std::string> channels = user.getChannels();
    for (std::set<std::string>::iterator it = channels.begin(); it != channels.end(); it++, i++)
    {
        stdout << "Channel " << i << " of User " << user.getNickname() << " is called " << *it << std::endl;
    }
    return (stdout);
}

void Server::clearAll()
{
	for (std::map<int, User*>::iterator it = this->_users.begin(); it != this->_users.end(); it++)
		delete it->second;
	this->_users.clear();
	for (std::map<std::string, Channel*>::iterator it = this->_channels.begin(); it != this->_channels.end(); it++)
		delete it->second;
	this->_channels.clear();
}

