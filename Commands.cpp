/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: htouil <htouil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 00:02:13 by htouil            #+#    #+#             */
/*   Updated: 2024/12/24 23:55:49 by htouil           ###   ########.fr       */
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
		return (send_server_msg(client, ERR_NEEDMOREPARAMS(client.GetNickname(), "PASS")));
	if (args.second.size() > 1)
		return (send_server_msg(client, ERR_TOOMANYPARAMS(client.GetNickname(), "PASS")));
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
		return (send_server_msg(client, ERR_TOOMANYPARAMS(client.GetNickname(), "NICK")));
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
		return (send_server_msg(client, ERR_NEEDMOREPARAMS(client.GetNickname(), "USER")));
	if (args.second.size() > 4)
		return (send_server_msg(client, ERR_TOOMANYPARAMS(client.GetNickname(), "USER")));
	// if (args.second[1] != "0" || args.second[2] != "*")
	// 	return (send_server_msg(client, ERR_NEEDMOREPARAMS(client.GetNickname(), "USER")));
	if (args.second[0].size() > 10)
		client.SetUsername(args.second[0].substr(0, 10));
	client.SetUsername(args.second[0]);
	if (args.second[3][0] != ':' || !std::isalpha(args.second[3][1]))
		return (send_server_msg(client, ERR_NEEDMOREPARAMS(client.GetNickname(), "USER")));
	std::string	tmp = args.second[3];
	size_t	i;
	for (i = 1; i < tmp.size(); i++)
	{
		if (!std::isalpha(tmp[i]) && tmp[i] != ' ') 
			return (send_server_msg(client, ERR_NEEDMOREPARAMS(client.GetNickname(), "USER")));
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
		return (send_server_msg(client, ERR_TOOMANYPARAMS(client.GetNickname(), "QUIT")));
	std::cerr << "Client: " << client.GetFd() << " Hung up." << std::endl;
	this->Remove_Client(client.GetFd());
	close(client.GetFd());
}

void	Server::help(Client &client)
{
	std::string	help;
	
	help.append("\n");
	help.append("===========================\n");
	help.append("       IRC HELP GUIDE      \n");
	help.append("===========================\n");
	help.append("\nCOMMANDS:\n");
	help.append("\n1. PASS:\n");
	help.append("  - Description: Sets the password for connecting to the server.\n");
	help.append("  - Usage: PASS <password>\n");
	help.append("  - Note: Must be the first command sent by the client before any other command.\n");
	help.append("\n2. NICK:\n");
	help.append("  - Description: Sets or changes your nickname.\n");
	help.append("  - Usage: NICK <nickname>\n");
	help.append("  - Note: Nicknames must be unique and cannot contain spaces.\n");
	help.append("\n3. USER:\n");
	help.append("  - Description: Sets your username and real name.\n");
	help.append("  - Usage: USER <username> 0 * :<realname>\n");
	help.append("\n4. QUIT:\n");
	help.append("  - Description: Disconnects from the server with an optional farewell message.\n");
	help.append("  - Usage: QUIT :<message>\n");
	help.append("  - Note: If no message is provided, a default quit message will be sent.\n");
	help.append("\n5. HELP:\n");
	help.append("  - Description: Displays this help guide.\n");
	help.append("  - Usage: HELP\n");
	help.append("\n"); 
	help.append("===========================\n");
	help.append("        END OF GUIDE       \n");
	help.append("===========================\n\r\n");
	send_server_msg(client, help);
}

// void	Server::join_help()

struct IsSymbol
{
	std::string symbol_;

	IsSymbol(std::string symbol) : symbol_(symbol) {}
	bool operator()(std::pair<Client, std::string> &member)
	{
		return member.second == symbol_;
	}
};

void	Server::join(std::pair<std::string, std::vector<std::string> > args, Client &client)
{
	size_t						i;
	size_t						j;
	std::vector<std::string>	chans;
	std::vector<std::string>	keys;

	if (client.GetifReg() == false)
		return (send_server_msg(client, ERR_NOTREGISTERED(client.GetNickname())));
	if (args.second.empty())
		return (send_server_msg(client, ERR_NEEDMOREPARAMS(client.GetNickname(), "JOIN")));
	if (args.second.size() > 2)
		return (send_server_msg(client, ERR_TOOMANYPARAMS(client.GetNickname(), "JOIN")));
	if (args.second.size() == 1 && args.second.front() == "0")
	{
		for (i = 0; i < this->Channels.size(); i++)
		{
			std::vector<std::pair<Client, std::string> >				&Cmbs = this->Channels[i].GetMemberlist();
			std::vector<std::pair<Client, std::string> >::iterator		it1;
			std::vector<std::pair<Client, std::string> >::iterator		it2;

			it1 = findclient(Cmbs, client); 
			if (it1 != Cmbs.end())
			{
				size_t	k;

				for (k = 0; k < Cmbs.size(); k++)
					send_server_msg(Cmbs[k].first, ":" + get_cli_source(client) + " PART :" + this->Channels[i].GetName() + "\r\n");
				
				if (it1->second == "@" && Cmbs.size() >= 2 && std::count_if(Cmbs.begin(), Cmbs.end(), IsSymbol("@")) == 1)
				{
					it2 = comparesymbol(Cmbs, "+");
					if (it2 != Cmbs.end())
						send_server_msg(it2->first, ":ircserv MODE " + this->Channels[i].GetName() + " +o " + it2->first.GetNickname() + "\r\n");
				}
				Cmbs.erase(it1);
				if (Cmbs.empty())
					this->Channels.erase(this->Channels.begin() + i);
			}
		}
		return ;
	}
	size_t	x,y;
	x = std::count(args.second[0].begin(), args.second[0].end(), ',');
	if (args.second.size() == 2)
		y = std::count(args.second[1].begin(), args.second[1].end(), ',');
	if (args.second.size() == 2 && x != y)
		return (send_server_msg(client, ERR_NEEDMOREPARAMS(client.GetNickname(), "JOIN")));
	chans = split_input(args.second[0], ",");
	if (args.second.size() > 0 && chans.size() != x + 1) //maybe good
		return (send_server_msg(client, ERR_NEEDMOREPARAMS(client.GetNickname(), "JOIN")));
	if (args.second.size() == 2)
		keys = split_input(args.second[1], ",");
	// std::cout << "size chans: " << chans.size() << std::endl;
	// std::cout << "size x: " << x << std::endl;
	// for (i = 0; i < chans.size(); i++)
	// 	std::cout << "- \'" << chans[i] << "\'" << std::endl;
	// std::cout << "size: " << keys.size() << std::endl;
	// for (i = 0; i < keys.size(); i++)
	// 	std::cout << "- \'" << keys[i] << "\'" << std::endl;
	for (i = 0; i < chans.size(); i++)
	{
		//check private channel &
		if (chans[i][0] != '#' || std::count(chans[i].begin(), chans[i].end(), ' ') || std::count(chans[i].begin(), chans[i].end(), ',')
			|| chans[i].size() < 1 || chans[i].size() > 21)
			return (send_server_msg(client, ERR_BADCHANMASK(client.GetNickname(), chans[i])));
		bool	flag = false;
		for (j = 0; j < this->Channels.size(); j++)
		{
			if (chans[i] == this->Channels[j].GetName())
			{
				std::vector<std::pair<Client, std::string> >			&Cmbs = this->Channels[j].GetMemberlist();
				std::vector<std::pair<Client, std::string> >::iterator	it;

				it = findclient(Cmbs, client);
				if (it != Cmbs.end())
					continue ;
				if (this->find_fd(client.GetFd(), this->Channels[j].GetBannedlist()) > -1)
					return (send_server_msg(client, ERR_BANNEDFROMCHAN(client.GetNickname(), chans[i])));
				if (this->Channels[j].GetifInvonly() == true)
					return (send_server_msg(client, ERR_INVITEONLYCHAN(client.GetNickname(), chans[i])));
				if (this->Channels[j].GetKey() != "" && (keys.empty() || (keys[i] != this->Channels[j].GetKey())))
					return (send_server_msg(client, ERR_BADCHANNELKEY(client.GetNickname(), chans[i])));
				if (Cmbs.size() == this->Channels[j].GetLimit())
					return (send_server_msg(client, ERR_CHANNELISFULL(client.GetNickname(), chans[i])));
				Cmbs.push_back(std::make_pair(client, "+"));
				size_t	k;

				for (k = 0; k < Cmbs.size(); k++)
					send_server_msg(Cmbs[k].first, ":" + client.GetNickname() + "!" + client.GetUsername() + "@" + client.GetIPaddr() + " JOIN :" + chans[i] + "\r\n");
				if (this->Channels[j].GetTopic() != "")
					send_server_msg(client, RPL_TOPIC(client.GetNickname(), this->Channels[j].GetName(), this->Channels[j].GetTopic()));
				// for (k = 0; k < Cmbs.size(); k++)
				// {
				// 	// std::cout << "Client " << k << ": " << Cmbs[k].second << std::endl;
				// 	if (Cmbs[k].second == "@")
				// 		ops << "@" << Cmbs[k].first.GetNickname();
				// 	else if (Cmbs[k].second == "+")
				// 		ops << "+" << Cmbs[k].first.GetNickname();
				// 	if (k < Cmbs.size() - 1)
				// 		ops  << " ";
				// }
				// ops  << "\r\n";
				std::ostringstream	ops;
				std::ostringstream	usrs;
				for (k = 0; k < Cmbs.size(); k++)
				{
					if (Cmbs[k].second == "@")
					{
						ops << "@" << Cmbs[k].first.GetNickname();
						if (k < Cmbs.size() - 1)
							ops  << " ";
					}
					else if (Cmbs[k].second == "+")
					{
						usrs << "+" << Cmbs[k].first.GetNickname();
						if (k < Cmbs.size() - 1)
							usrs  << " ";
					}
				}
				usrs << "\r\n";
				// std::cout << "List of members: " << ops.str() + usrs.str() << std::endl;
				for (k = 0; k < Cmbs.size(); k++)
				{
					send_server_msg(Cmbs[k].first, RPL_NAMREPLY(Cmbs[k].first.GetNickname(), (this->Channels[j].GetName()), ops.str() + usrs.str()));
					send_server_msg(Cmbs[k].first, RPL_ENDOFNAMES(Cmbs[k].first.GetNickname(), this->Channels[j].GetName()));	
				}
				flag = true;
				break ;
			}
		}
		if (flag == true)
			continue ;
		Channel	ch(chans[i], "");
		std::vector<std::pair<Client, std::string> > &Cmbs = ch.GetMemberlist();

		if (keys.size() >= i + 1)
			ch.SetKey(keys[i]);
		Cmbs.push_back(std::make_pair(client, "@"));
		this->Channels.push_back(ch);
		send_server_msg(client, ":" + get_cli_source(client) + " JOIN :" + chans[i] + "\r\n");
		std::ostringstream	tmp;
		tmp << Cmbs[0].second << client.GetNickname() << "\r\n";
		send_server_msg(client, RPL_NAMREPLY(client.GetNickname(), this->Channels[j].GetName(), tmp.str()));
		send_server_msg(client, RPL_ENDOFNAMES(client.GetNickname(), chans[i]));
	}
	return ;
}

void	Server::privmsg(std::pair<std::string, std::vector<std::string> > args, Client &client)
{
	std::vector<std::string>	targets;
	size_t						x;

	if (client.GetifReg() == false)
		return (send_server_msg(client, ERR_NOTREGISTERED(client.GetNickname())));
	if (args.second.size() < 2)
		return (send_server_msg(client, ERR_NEEDMOREPARAMS(client.GetNickname(), "PRIVMSG")));
	if (args.second.size() > 2)
		return (send_server_msg(client, ERR_TOOMANYPARAMS(client.GetNickname(), "PRIVMSG")));
	targets = split_input(args.second[0], ",");
	std::cout << "size: " << targets.size() << std::endl;
	// size_t	j;
	// for (j = 0; j < targets.size(); j++)
	// 	std::cout << "- \'" << targets[j] << "\'" << std::endl;
	x = std::count(args.second[0].begin(), args.second[0].end(), ',');
	if (targets.size() != x + 1) // recheck
		return (send_server_msg(client, ERR_NEEDMOREPARAMS(client.GetNickname(), "PRIVMSG")));
	// if (args.second[1][0] != ':')
	// 	return (send_server_msg(client, ERR_NEEDMOREPARAMS(client.GetNickname(), "PRIVMSG")));
	size_t	i;
	for (i = 0; i < targets.size(); i++)
	{
		if (std::count(targets.begin(), targets.begin() + i, targets[i]) > 1)
			continue ;
		int	pos;
		pos = this->find_nickname(targets[i], this->Clients);
		if (pos == -1)
		{
			send_server_msg(client, ERR_NOSUCHNICK(targets[i], "PRIVMSG", "nick"));
			continue ;
		}
		else
			send_server_msg(this->Clients[pos], ":" + get_cli_source(client) + " PRIVMSG " + this->Clients[pos].GetNickname() + " " + args.second[1] + "\r\n");
	}
}

void	Server::topic(std::pair<std::string, std::vector<std::string> > args, Client &client)
{
	if (client.GetifReg() == false)
		return (send_server_msg(client, ERR_NOTREGISTERED(client.GetNickname())));
	if (args.second.size() < 1)
		return (send_server_msg(client, ERR_NEEDMOREPARAMS(client.GetNickname(), "TOPIC")));
	if (args.second.size() > 2)
		return (send_server_msg(client, ERR_TOOMANYPARAMS(client.GetNickname(), "TOPIC")));
	std::vector<Channel>::iterator	Cit;

	Cit = findchannel(this->Channels, args.second[0]);
	if (Cit == this->Channels.end())
		return (send_server_msg(client, ERR_NOSUCHCHANNEL(client.GetNickname(), args.second[0])));
	std::vector<std::pair<Client, std::string> > 			&Cmbs = Cit->GetMemberlist();
	std::vector<std::pair<Client, std::string> >::iterator	Mit;

	Mit = findclient(Cmbs, client);
	if (Mit == Cmbs.end())
		return (send_server_msg(client, ERR_NOTONCHANNEL(client.GetNickname(), Cit->GetName())));
	if (this->find_fd(client.GetFd(), Cit->GetBannedlist()) > -1)
		return (send_server_msg(client, ERR_BANNEDFROMCHAN(client.GetNickname(), Cit->GetName())));
	if (args.second.size() == 1)
	{
		if (Cit->GetTopic() == "")
			return (send_server_msg(client, RPL_NOTOPIC(client.GetNickname(), Cit->GetName())));
		return send_server_msg(client, RPL_TOPIC(client.GetNickname(), Cit->GetName(), Cit->GetTopic()));
	}
	else
	{
		if (Cit->GetCantopic() == false && Mit->second != "@")
			return (send_server_msg(client, ERR_CHANOPRIVSNEEDED(client.GetNickname(), Cit->GetName())));
		std::string	&topic = args.second[1];
		size_t	i;

		if (topic.size() >= 2 && topic[0] == ':' && topic[1] == ':')
			topic = topic.substr(1);
		size_t pos;

		pos = topic.find_last_not_of(" ");
		if (pos != std::string::npos)
			topic.erase(pos + 1);
		if (topic.size() == 1)
		{
			Cit->SetTopic("");
			for (i = 0; i < Cmbs.size(); i++)
			{
				send_server_msg(Cmbs[i].first, RPL_TOPIC(client.GetNickname(), Cit->GetName(), Cit->GetTopic()));
				// send_server_msg(Cmbs[i].first, RPL_NOTOPIC(client.GetNickname(), Cit->GetName()));
			}
			return ;
		}
		else
		{
			Cit->SetTopic(topic.substr(1));
			for (i = 0; i < Cmbs.size(); i++)
				send_server_msg(Cmbs[i].first, RPL_TOPIC(client.GetNickname(), Cit->GetName(), Cit->GetTopic()));
			return ;
		}
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
	else if (args.first == "PRIVMSG" || args.first == "privmsg")
		privmsg(args, client);
	else if (args.first == "TOPIC" || args.first == "topic")
		topic(args, client);
	if (client.GetifReg() == false && client.GetIfPassCorr() == true && client.GetNickname() != "*" && client.GetUsername() != "*" && client.GetRealname() != "*")
	{
		client.SetifReg(true);
		return (send_server_msg(client, RPL_WELCOME(client.GetNickname(), client.GetUsername(), client.GetIPaddr())));
	}
}
