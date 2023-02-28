#include "main.hpp"

void restart(Server *serv, std::string buffer, int sd)
{
	(void)buffer;
	if (FIND_USER(sd)->getMode().find('o') == std::string::npos)
        sendMessage(sendRplErr(481, serv, FIND_USER(sd), "", ""), sd);
	else
		serv->setIsRestart();
}