/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: htouil <htouil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 00:02:13 by htouil            #+#    #+#             */
/*   Updated: 2024/12/07 15:37:46 by htouil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./Server/Server.hpp"

std::string	remove_crln(std::string msg)
{
	size_t	pos;

	pos = msg.find_first_of("\r\n");
	if (pos != std::string::npos)
		return (msg.substr(0, pos));
	return (msg);
}

std::string	display_current_time()
{
	std::time_t			rn = std::time(0);
	std::tm				*now = std::localtime(&rn);
	std::ostringstream	oss;

	oss << "["
		<< (now->tm_year + 1900) << "/"
		<< std::setw(2) << std::setfill('0') << (now->tm_mon + 1) << "/"
		<< std::setw(2) << std::setfill('0') << (now->tm_mday) << " "
		<< std::setw(2) << std::setfill('0') << (now->tm_hour) << ":"
		<< std::setw(2) << std::setfill('0') << (now->tm_min) << ":" 
		<< std::setw(2) << std::setfill('0') << (now->tm_sec) << "] ";
	return (oss.str());
}

void	send_server_msg(Client &client, std::string err_msg)
{
	std::string	tmp;

	tmp = display_current_time() + err_msg;
	if (send(client.GetFd(), (tmp).c_str(), tmp.size(), 0) == -1)
		std::cerr << "Failed to send to client " << client.GetFd() << std::endl;
}

void	Server::pass(std::pair<std::string, std::vector<std::string> > args, Client &client)
{
	if (client.GetifReg() == true)
		return (send_server_msg(client, ERR_ALREADYREGISTERED(client.GetNickname())));
	if (client.GetPassword() == true)
		return ;
	if (args.second.size() < 1)
		return (send_server_msg(client, ERR_NOTENOUGHPARAMS(client.GetNickname())));
	if (args.second.size() > 1)
		return (send_server_msg(client, ERR_TOOMANYPARAMS(client.GetNickname())));
	if (args.second.front() != this->Spassword)
		return (send_server_msg(client, ERR_PASSWDMISMATCH(client.GetNickname())));
	client.SetPassword(true);
}

void	Server::nick(std::pair<std::string, std::vector<std::string> > args, Client &client)
{
	if (client.GetPassword() == false)
		return (send_server_msg(client, ERR_PASSWDMISMATCH(client.GetNickname())));
	if (args.second.size() < 1)
		return (send_server_msg(client, ERR_NONICKNAMEGIVEN(client.GetNickname())));
	if (args.second.size() > 1)
		return (send_server_msg(client, ERR_TOOMANYPARAMS(client.GetNickname())));
	std::string	tmp = args.second.front();
	if (this->find_nickname(tmp, this->Clients) > -1)
		return (send_server_msg(client, ERR_NICKNAMEINUSE(client.GetNickname(), tmp)));
	if (tmp[0] == '#' || tmp[0] == '&' || tmp[0] == ':' || std::isdigit(tmp[0]))
		return (send_server_msg(client, ERR_ERRONEUSNICKNAME(client.GetNickname(), args.second.front())));
	size_t	i;
	for (i = 0; i < tmp.size(); i++)
	{
		if (tmp[i] == ' ' || (!std::isalpha(tmp[i]) && !std::isdigit(tmp[i]) && tmp[i] != '[' && tmp[i] != ']' && tmp[i] != '{' && tmp[i] != '}' && tmp[i] != '\\' && tmp[i] != '|' && (i > 0 && tmp[i] != ':')))
			return (send_server_msg(client, ERR_ERRONEUSNICKNAME(client.GetNickname(), args.second.front())));
	}
	// if (client.GetifReg() == true) //should be put in the nick_update() function
	// 	send_server_msg(client, ":" + client.GetNickname() + " NICK " + args.second.front());
	client.SetNickname(args.second.front());
}

void	Server::user(std::pair<std::string, std::vector<std::string> > args, Client &client)
{
	if (client.GetifReg() == true)
		return (send_server_msg(client, ERR_ALREADYREGISTERED(client.GetNickname())));
	if (client.GetPassword() == false)
		return (send_server_msg(client, ERR_PASSWDMISMATCH(client.GetNickname())));
	if (args.second.size() < 4)
		return (send_server_msg(client, ERR_NOTENOUGHPARAMS(client.GetNickname())));
	if (args.second.size() > 4)
		return (send_server_msg(client, ERR_TOOMANYPARAMS(client.GetNickname())));
	if (args.second[1] != "0" || args.second[2] != "*")
		return (send_server_msg(client, ERR_NOTENOUGHPARAMS(client.GetNickname())));
	if (args.second[0].size() > 10)
		client.SetUsername(args.second[0].substr(0, 10));
	client.SetUsername(args.second[0]);
	if (args.second[3][0] != ':' || !std::isalpha(args.second[3][1]))
		return (send_server_msg(client, ERR_NOTENOUGHPARAMS(client.GetNickname())));
	std::string	tmp = args.second[3];
	size_t	i;
	for (i = 1; i < tmp.size(); i++)
	{
		if (!std::isalpha(tmp[i]) && tmp[i] != ' ') 
			return (send_server_msg(client, ERR_NOTENOUGHPARAMS(client.GetNickname())));
	}
	size_t pos;
	pos = args.second[3].find_last_not_of(" ");
	if (pos != std::string::npos)
		args.second[3].erase(pos + 1);
	client.SetRealname(args.second[3].substr(1));
	// std::cout << "Realname: \'" << client.GetRealname() << "\'" << std::endl;
}

void	Server::quit(std::pair<std::string, std::vector<std::string> > args, Client &client)
{
	if (args.second.size() > 0)
		return (send_server_msg(client, ERR_TOOMANYPARAMS(client.GetNickname())));
	std::cerr << "Client: " << client.GetFd() << " Hung up." << std::endl;
	this->Remove_Client(client.GetFd());
	close(client.GetFd());
}

void	Server::help(Client &client)
{
	std::ostringstream	oss;
	oss << std::endl 
		<< "===========================" << std::endl
		<< "       IRC HELP GUIDE      " << std::endl
		<< "===========================" << std::endl
		<< std::endl << "COMMANDS:" << std::endl
		<< std::endl << "1. " << "PASS:" << std::endl
		<< "  - Description: Sets the password for connecting to the server." << std::endl
		<< "  - Usage: PASS <password>" << std::endl
		<< "  - Note: Must be the first command sent by the client before any other command." << std::endl
		<< std::endl << "2. " << "NICK:" << std::endl
		<< "  - Description: Sets or changes your nickname." << std::endl
		<< "  - Usage: NICK <nickname>" << std::endl
		<< "  - Note: Nicknames must be unique and cannot contain spaces." << std::endl
		<< std::endl << "3. " << "USER:" << std::endl
		<< "  - Description: Sets your username and real name." << std::endl
		<< "  - Usage: USER <username> 0 * :<realname>" << std::endl
		<< std::endl << "4. " << "QUIT:" << std::endl
		<< "  - Description: Disconnects from the server with an optional farewell message." << std::endl
		<< "  - Usage: QUIT :<message>" << std::endl
		<< "  - Note: If no message is provided, a default quit message will be sent." << std::endl
		<< std::endl << "5. " << "HELP:" << std::endl
		<< "  - Description: Displays this help guide." << std::endl
		<< "  - Usage: HELP" << std::endl
		<< std::endl 
		<< "===========================" << std::endl
		<< "        END OF GUIDE       " << std::endl
		<< "===========================" << std::endl << std::endl;
	send_server_msg(client, oss.str());
}

// void	Server::join_help()

void	Server::join(std::pair<std::string, std::vector<std::string> > args, Client &client)
{
	size_t						i;
	size_t						j;
	std::vector<std::string>	chans;
	std::vector<std::string>	keys;

	if (client.GetifReg() == false)
		return (send_server_msg(client, ERR_NOTREGISTERED(client.GetNickname())));
	if (args.second.empty())
		return (send_server_msg(client, ERR_NOTENOUGHPARAMS(client.GetNickname())));
	if (args.second.size() > 2)
		return (send_server_msg(client, ERR_TOOMANYPARAMS(client.GetNickname())));
	if (args.second.size() == 1 && args.second.front() == "0")
	{
		for (i = 0; i < this->Channels.size(); i++)
		{
			std::vector<Client>	Cmembers = this->Channels[i].GetMemberlist();
			for (j = 0; j < Cmembers.size(); j++)
			{
				if (client.GetFd() == Cmembers[j].GetFd())
					Cmembers.erase(Cmembers.begin() + j);
			}
		}
		return ;
	}
	int	x,y;
	x = std::count(args.second[0].begin(), args.second[0].end(), ',');
	y = std::count(args.second[1].begin(), args.second[1].end(), ',');
	if (args.second.size() == 2 && ((args.second[0].size() != x) || (x != y)))
		return (send_server_msg(client, ERR_NOTENOUGHPARAMS(client.GetNickname())));
	chans = split_input(args.second[0], ",");
	keys = split_input(args.second[1], ",");
	// std::cout << "size: " << chans.size() << std::endl;
	// for (i = 0; i < chans.size(); i++)
	// 	std::cout << "- \'" << chans[i] << "\'" << std::endl;
	// std::cout << "size: " << keys.size() << std::endl;
	// for (i = 0; i < keys.size(); i++)
	// 	std::cout << "- \'" << keys[i] << "\'" << std::endl;
	// Channel	chh("#ch", "key");
	// this->Channels.push_back(chh);
	for (i = 0; i < chans.size(); i++)
	{
		if (chans[i][0] != '#' || std::count(chans[i].begin(), chans[i].end(), ' ') || std::count(chans[i].begin(), chans[i].end(), ','))
			return (send_server_msg(client, ERR_BADCHANMASK(client.GetNickname(), chans[i])));
		for (j = 0; j < this->Channels.size(); j++)
		{
			if (chans[i] == this->Channels[j].GetName())
			{
				if (this->find_fd(client.GetFd(), this->Channels[j].GetBannedlist()) > -1)
					return (send_server_msg(client, ERR_BANNEDFROMCHAN(client.GetNickname(), chans[i])));
				if (this->Channels[j].Getifinvonly() == true)
					return (send_server_msg(client, ERR_INVITEONLYCHAN(client.GetNickname(), chans[i])));
				if (this->Channels[j].GetKey() != "" && keys[i] != this->Channels[j].GetKey())
					return (send_server_msg(client, ERR_BADCHANNELKEY(client.GetNickname(), chans[i])));
				std::vector<Client> ch;
				ch = this->Channels[j].GetMemberlist();
				if (ch.size() == this->Channels[j].GetLimit())
					return (send_server_msg(client, ERR_CHANNELISFULL(client.GetNickname(), chans[i])));
				ch.push_back(client);
				std::cout << "size: " << ch.size() << std::endl;
				size_t	k;
				for (k = 0; k < ch.size(); k++)
					send_server_msg(ch[i], ":" + client.GetNickname() + "!" + client.GetUsername() + "@" + client.GetIPaddr() + " JOIN :" + chans[i] + "\r\n");
				if (this->Channels[j].GetTopic() != "")
					send_server_msg(client, RPL_TOPIC(client.GetNickname(), this->Channels[j].GetName(), this->Channels[j].GetTopic()));
				std::ostringstream	oss;
				for (k = 0; k < ch.size(); k++)
				{
					if (k == 0)
						oss << "@" << ch[k].GetNickname() << " ";
					else if (k == ch.size() - 1)
						oss << "+" << ch[k].GetNickname() << "\r\n";
					else
						oss << "+" << ch[k].GetNickname() << " ";
				}
				return (send_server_msg(client, RPL_NAMREPLY(client.GetNickname(), this->Channels[j].GetName()) + oss.str()));
			}
			else
			{
				// here to create a new channel
			}
		}
	}
}

void	Server::commands(std::pair<std::string, std::vector<std::string> > args, Client &client)
{
	if (args.first == "PASS" || args.first == "pass")
		pass(args, client);
	else if (args.first == "NICK" || args.first == "nick")
		nick(args, client);
	else if (args.first == "USER" || args.first == "user")
		user(args, client);
	else if (args.first == "QUIT" || args.first == "quit")
		quit(args, client);
	else if (args.first == "HELP" || args.first == "help")
		help(client);
	else if (args.first == "JOIN" || args.first == "join")
		join(args, client);
	if (client.GetifReg() == false && client.GetPassword() == true && client.GetNickname() != "*" && client.GetUsername() != "*" && client.GetRealname() != "*")
	{
		client.SetifReg(true);
		return (send_server_msg(client, RPL_WELCOME(client.GetNickname(), client.GetUsername(), client.GetIPaddr())));
	}
}
