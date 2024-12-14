/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: htouil <htouil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 00:02:13 by htouil            #+#    #+#             */
/*   Updated: 2024/12/14 21:53:12 by htouil           ###   ########.fr       */
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

void	Server::send_server_msg(Client &client, std::string err_msg)
{
	std::string	tmp;

	tmp = display_current_time() + err_msg;
	if (send(client.GetFd(), (err_msg).c_str(), err_msg.size(), 0) == -1)
		std::cerr << "Failed to send to client " << client.GetFd() << std::endl;
}

void	Server::pass(std::pair<std::string, std::vector<std::string> > args, Client &client)
{
	// std::cout << "EWAAAA" << std::endl;
	if (client.GetifReg() == true)
		return (send_server_msg(client, ERR_ALREADYREGISTERED(client.GetNickname())));
	if (client.GetIfPassCorr() == true)
		return ;
	if (args.second.size() < 1)
		return (send_server_msg(client, ERR_NOTENOUGHPARAMS(client.GetNickname())));
	if (args.second.size() > 1)
		return (send_server_msg(client, ERR_TOOMANYPARAMS(client.GetNickname())));
	if (args.second.front() != this->Spassword)
		return (send_server_msg(client, ERR_PASSWDMISMATCH(client.GetNickname())));
	client.SetPassword(args.second.front());
}

void	Server::nick(std::pair<std::string, std::vector<std::string> > args, Client &client)
{
	if (client.GetIfPassCorr() == false)
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
	if (client.GetIfPassCorr() == false)
		return (send_server_msg(client, ERR_PASSWDMISMATCH(client.GetNickname())));
	if (args.second.size() < 4)
		return (send_server_msg(client, ERR_NOTENOUGHPARAMS(client.GetNickname())));
	if (args.second.size() > 4)
		return (send_server_msg(client, ERR_TOOMANYPARAMS(client.GetNickname())));
	// if (args.second[1] != "0" || args.second[2] != "*")
	// 	return (send_server_msg(client, ERR_NOTENOUGHPARAMS(client.GetNickname())));
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
	if (args.second.size() == 1 && args.second.front() == "0") //send a server msg so that hexchat acknowledge the command. 
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
	size_t	x,y;
	x = std::count(args.second[0].begin(), args.second[0].end(), ',');
	if (args.second.size() == 2)
		y = std::count(args.second[1].begin(), args.second[1].end(), ',');
	if (args.second.size() == 2 && x != y)
		return (send_server_msg(client, ERR_NOTENOUGHPARAMS(client.GetNickname())));
	// std::cout << "SALAM" << std::endl; // segfault in join
	chans = split_input(args.second[0], ",");
	if (args.second.size() > 0 && chans.size() != x + 1)
		return (send_server_msg(client, ERR_NOTENOUGHPARAMS(client.GetNickname())));
	if (args.second.size() == 2)
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
		bool	flag = false;
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
				std::vector<Client> &mbs = this->Channels[j].GetMemberlist();;
				// mbs = this->Channels[j].GetMemberlist();
				if (mbs.size() == this->Channels[j].GetLimit())
					return (send_server_msg(client, ERR_CHANNELISFULL(client.GetNickname(), chans[i])));
				client.SetChanmod("+");
				mbs.push_back(client);
				// std::cout << this->Channels[j].GetName() << " | " << mbs.size() << std::endl;
				size_t	k;
				for (k = 0; k < mbs.size(); k++)
				{
					// std::cout << k << ": " << mbs[i].GetFd() << std::endl;
					send_server_msg(mbs[k], ":" + client.GetNickname() + "!" + client.GetUsername() + "@" + client.GetIPaddr() + " JOIN :" + chans[i] + "\r\n");
				}
				if (this->Channels[j].GetTopic() != "")
					send_server_msg(client, RPL_TOPIC(client.GetNickname(), this->Channels[j].GetName(), this->Channels[j].GetTopic()));
				std::ostringstream	oss;
				for (k = 0; k < mbs.size(); k++)
				{
					if (k == 0)
						oss << "@" << mbs[k].GetNickname();
					else if (k == mbs.size() - 1)
						oss << "+" << mbs[k].GetNickname();
					else
						oss << "+" << mbs[k].GetNickname();
					if (k < mbs.size() - 1)
						oss  << " ";
				}
				oss  << "\r\n";
				for (k = 0; k < mbs.size(); k++)
				{
					send_server_msg(mbs[k], RPL_NAMREPLY(mbs[k].GetNickname(), (this->Channels[j].GetName()), oss.str()));
					send_server_msg(mbs[k], RPL_ENDOFNAMES(mbs[k].GetNickname(), this->Channels[j].GetName()));	
				}
				flag = true;
				break ;
			}
		}
		if (flag == true)
			continue ;
		// std::cout << "ZABBB" << std::endl;
		Channel	ch(chans[i], "");
		std::vector<Client> &mbs = ch.GetMemberlist();;

		if (keys.size() >= i + 1)
			ch.SetKey(keys[i]);
		// mbs = ch.GetMemberlist();
		client.SetChanmod("@");
		mbs.push_back(client);
		this->Channels.push_back(ch);
		send_server_msg(client, ":" + client.GetNickname() + "!" + client.GetUsername() + "@" + client.GetIPaddr() + " JOIN :" + chans[i] + "\r\n");
		std::string user = ":" + client.GetNickname() + "!" + client.GetUsername() + "@" + client.GetIPaddr();
		// std::ostringstream timeStr;
		// timeStr << std::time(0);
		// send_server_msg(client, RPL_TOPICWHOTIME(client.GetNickname(), chans[i], user, timeStr.str())); 
		std::ostringstream	tmp;
		tmp << "@" << client.GetNickname() << "\r\n";
		send_server_msg(client, RPL_NAMREPLY(client.GetNickname(), this->Channels[j].GetName(), tmp.str()));
		send_server_msg(client, RPL_ENDOFNAMES(client.GetNickname(), chans[i]));
	}
}

void	Server::commands(std::pair<std::string, std::vector<std::string> > args, Client &client)
{
	// std::cout << "Command: \'" << args.first << "\'" << std::endl;
	// for (size_t j = 0; j < args.second.size(); j++)
	// 	std::cout << "Arg " << j + 1 << ": \'" << args.second[j] << "\'"<< std::endl;
	// std::cout << "----------------------" << std::endl;
	if (args.first == "CAP" || args.first == "cap")
		return ;
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
	if (client.GetifReg() == false && client.GetIfPassCorr() == true && client.GetNickname() != "*" && client.GetUsername() != "*" && client.GetRealname() != "*")
	{
		client.SetifReg(true);
		return (send_server_msg(client, RPL_WELCOME(client.GetNickname(), client.GetUsername(), client.GetIPaddr())));
	}
}
