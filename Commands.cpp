/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: htouil <htouil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 00:02:13 by htouil            #+#    #+#             */
/*   Updated: 2025/01/03 20:32:23 by htouil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./Server/Server.hpp"

void	Server::send_server_msg(Client &client, std::string msg)
{
	ssize_t sent = send(client.GetFd(), msg.c_str(), msg.size(), 0);
	if (sent == -1)
		std::cerr << "Failed to send to client " << client.GetFd() << " (" << client.GetNickname() << "): " << strerror(errno) << std::endl;
	else if (sent < static_cast<ssize_t>(msg.size()))
		std::cerr << "Partial send to client " << client.GetFd() << ": " << sent << "/" << msg.size() << " bytes" << std::endl;
}

void	Server::send_to_all_in_chan(std::vector<std::pair<Client, std::string> > &Cmbs, std::string msg)
{
	std::vector<std::pair<Client, std::string> >::iterator it;

	for (it = Cmbs.begin(); it != Cmbs.end(); ++it)
		send_server_msg(it->first, msg);
}

void	Server::pass(std::pair<std::string, std::vector<std::string> > args, Client &client)
{
	// std::cout << "EWAAAA" << std::endl;
	if (client.GetifReg() == true)
		return (send_server_msg(client, ERR_ALREADYREGISTERED(client.GetNickname())));
	if (client.GetIfPassCorr() == true)
		return ;
	if (args.second.size() < 1)
		return (send_server_msg(client, ERR_NEEDMOREPARAMS("PASS")));
	if (args.second.size() > 1)
		return (send_server_msg(client, ERR_TOOMANYPARAMS("PASS")));
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
		return (send_server_msg(client, ERR_TOOMANYPARAMS("NICK")));
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
	if (client.GetifReg() == true)
		send_server_msg(client, ":" + get_cli_source(client) + " NICK :" + args.second.front() + "\r\n");
	for (i = 0; i < this->Channels.size(); i++)
	{
		std::vector<std::pair<Client, std::string> >				&Cmbs = this->Channels[i].GetMemberlist();
		std::vector<std::pair<Client, std::string> >::iterator		it1 = findclient(Cmbs, client.GetNickname());

		if (it1 != Cmbs.end())
		{
			for (size_t j = 0; j < Cmbs.size(); j++)
			{
				if (client.GetFd() == Cmbs[j].first.GetFd())
						continue ;
				send_server_msg(Cmbs[j].first, ":" + get_cli_source(client) + " NICK :" + args.second.front() + "\r\n");
			}
		}
	}
	client.SetNickname(args.second.front());
}

void	Server::user(std::pair<std::string, std::vector<std::string> > args, Client &client)
{
	if (client.GetifReg() == true)
		return (send_server_msg(client, ERR_ALREADYREGISTERED(client.GetNickname())));
	if (client.GetIfPassCorr() == false)
		return (send_server_msg(client, ERR_PASSWDMISMATCH(client.GetNickname())));
	if (args.second.size() < 4)
		return (send_server_msg(client, ERR_NEEDMOREPARAMS("USER")));
	if (args.second.size() > 4)
		return (send_server_msg(client, ERR_TOOMANYPARAMS("USER")));
	// if (args.second[1] != "0" || args.second[2] != "*")
	// 	return (send_server_msg(client, ERR_NEEDMOREPARAMS("USER")));
	if (args.second[0].size() > 10)
		client.SetUsername(args.second[0].substr(0, 10));
	client.SetUsername(args.second[0]);
	if (args.second[3][0] != ':' || !std::isalpha(args.second[3][1]))
		return (send_server_msg(client, ERR_NEEDMOREPARAMS("USER")));
	std::string	tmp = args.second[3];
	size_t	i;
	for (i = 1; i < tmp.size(); i++)
	{
		if (!std::isalpha(tmp[i]) && tmp[i] != ' ') 
			return (send_server_msg(client, ERR_NEEDMOREPARAMS("USER")));
	}
	size_t pos = args.second[3].find_last_not_of(" ");

	if (pos != std::string::npos)
		args.second[3].erase(pos + 1);
	client.SetRealname(args.second[3].substr(1));
	// std::cout << "Realname: \'" << client.GetRealname() << "\'" << std::endl;
}

struct IsSymbol
{
	std::string symbol_;	

	IsSymbol(std::string symbol) : symbol_(symbol) {}
	bool operator()(std::pair<Client, std::string> &member)
	{
		return member.second == symbol_;
	}
};

void	Server::quit(std::pair<std::string, std::vector<std::string> > args, Client &client)
{
	if (args.second.size() > 1)
		return (send_server_msg(client, ERR_TOOMANYPARAMS("QUIT")));
	size_t	i;
	std::string	&reason = args.second[0];
	size_t pos = args.second[0].find_last_not_of(" ");

	if (pos != std::string::npos)
		args.second[0].erase(pos + 1);
	if (reason[0] == ':')
		reason = reason.substr(1);
	if (reason[0] == ':')
		reason = reason.substr(1);
	// std::cout << "reason: " << reason << std::endl;
	send_server_msg(client, "ERROR :Closing Link: " + client.GetNickname() + " (" + reason + ")\r\n");
	for (i = 0; i < this->Channels.size(); i++)
	{
		std::vector<std::pair<Client, std::string> >				&Cmbs = this->Channels[i].GetMemberlist();
		std::vector<std::pair<Client, std::string> >::iterator		it1 = findclient(Cmbs, client.GetNickname());

		if (it1 != Cmbs.end())
		{
			if (reason.empty())
				send_to_all_in_chan(Cmbs, ":" + get_cli_source(client) + " QUIT :Quit:\r\n");
			else
				send_to_all_in_chan(Cmbs, ":" + get_cli_source(client) + " QUIT :" + reason + "\r\n");
			if (it1->second == "@" && Cmbs.size() >= 2 && std::count_if(Cmbs.begin(), Cmbs.end(), IsSymbol("@")) == 1)
			{
				std::vector<std::pair<Client, std::string> >::iterator		it2 = comparesymbol(Cmbs, "+");

				if (it2 != Cmbs.end())
				{
					it2->second = "@";
					for (size_t j = 0; j < Cmbs.size(); j++)
					{
						if (client.GetFd() == Cmbs[j].first.GetFd())
							continue ;
						send_server_msg(it2->first, ":ircserv MODE " + this->Channels[i].GetName() + " +o " + it2->first.GetNickname() + "\r\n");
					}
				}
			}
			Cmbs.erase(it1);
			if (Cmbs.empty())
				this->Channels.erase(this->Channels.begin() + i);
		}
	}
	std::cout << display_current_time() << "Client: " << client.GetFd() << " (" << client.GetNickname() << ") Hung up." << std::endl;
	this->Remove_Client(client.GetFd());
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
	help.append("\n5. JOIN:\n");
	help.append("  - Description: Joins a specified channel.\n");
	help.append("  - Usage: JOIN <channel>\n");
	help.append("\n6. PART:\n");
	help.append("  - Description: Removes the client from the given channel(s).\n");
	help.append("  - Usage: PART <channel>\n");
	help.append("\n7. INVITE:\n");
	help.append("  - Description: Invites a user to a channel.\n");
	help.append("  - Usage: INVITE <nickname> <channel>\n");
	help.append("\n8. PRIVMSG:\n");
	help.append("  - Description: Sends a private message to a user or channel.\n");
	help.append("  - Usage: PRIVMSG <recipient> <message>\n");
	help.append("  - Note: Ensure the recipient (user or channel) is valid and connected.\n");
	help.append("\n9. TOPIC:\n");
	help.append("  - Description: Sets or retrieves the topic of a specified channel.\n");
	help.append("  - Usage: TOPIC <channel> <topic>\n");
	help.append("  - Note: If the topic provided is empty, the channel's topic will be set to empty.\n");
	help.append("\n10. KICK:\n");
	help.append("  - Description: Kicks a user from a channel with an optional reason.\n");
	help.append("  - Usage: KICK <channel> <user> [<reason>]\n");
	help.append("  - Note: Only channel operators can kick users from a channel.\n");
	help.append("\n11. MODE:\n");
	help.append("  - Description: Used to set or remove options (or modes) from a given channel.\n");
	help.append("  - Usage: MODE #<channel> <mode> <target>\n");
	help.append("           MODE #<channel> <mode>\n");
	help.append("  - Note : The modes can be edited using a '-' or a '+' followed by one of these letters: \"i, t, k, o, l\".\n");
	help.append("           i: Invite-Only Mode.\n");
	help.append("           t: Topic Protection.\n");
	help.append("           k: Key/Password.\n");
	help.append("           o: Operator Privileges.\n");
	help.append("           t: User Limit.\n");
	help.append("\n12. HELP:\n");
	help.append("  - Description: Displays this help guide.\n");
	help.append("  - Usage: HELP\n");
	help.append("\n"); 
	help.append("===========================\n");
	help.append("        END OF GUIDE       \n");
	help.append("===========================\n\r\n");
	send_server_msg(client, help);
}

// void	Server::join_help()

void	Server::join(std::pair<std::string, std::vector<std::string> > args, Client &client)
{
	std::vector<std::string>	chans;
	std::vector<std::string>	keys;

	if (client.GetifReg() == false)
		return (send_server_msg(client, ERR_NOTREGISTERED("JOIN")));
	if (args.second.empty())
		return (send_server_msg(client, ERR_NEEDMOREPARAMS("JOIN")));
	if (args.second.size() > 2)
		return (send_server_msg(client, ERR_TOOMANYPARAMS("JOIN")));
	size_t	i;

	if (args.second.size() == 1 && args.second.front() == "0")
	{
		for (i = 0; i < this->Channels.size(); i++)
		{
			std::vector<std::pair<Client, std::string> >				&Cmbs = this->Channels[i].GetMemberlist();
			std::vector<std::pair<Client, std::string> >::iterator		it1 = findclient(Cmbs, client.GetNickname());

			if (it1 != Cmbs.end())
			{
				send_to_all_in_chan(Cmbs, ":" + get_cli_source(client) + " PART :" + this->Channels[i].GetName() + "\r\n");
				if (it1->second == "@" && Cmbs.size() >= 2 && std::count_if(Cmbs.begin(), Cmbs.end(), IsSymbol("@")) == 1)
				{
					std::vector<std::pair<Client, std::string> >::iterator		it2 = comparesymbol(Cmbs, "+");

					if (it2 != Cmbs.end())
					{
						it2->second = "@";
						for (size_t j = 0; j < Cmbs.size(); j++)
						{
							if (client.GetFd() == Cmbs[j].first.GetFd())
								continue ;
							send_server_msg(it2->first, ":ircserv MODE " + this->Channels[i].GetName() + " +o " + it2->first.GetNickname() + "\r\n");
						}
					}
				}
				Cmbs.erase(it1);
				if (Cmbs.empty())
					this->Channels.erase(this->Channels.begin() + i);
			}
		}
	}
	size_t	x,y;

	x = std::count(args.second[0].begin(), args.second[0].end(), ',');
	if (args.second.size() == 2)
		y = std::count(args.second[1].begin(), args.second[1].end(), ',');
	if (args.second.size() == 2 && x != y)
		return (send_server_msg(client, ERR_NEEDMOREPARAMS("JOIN")));
	chans = split_input(args.second[0], ",");
	if (args.second.size() > 0 && chans.size() != x + 1)
		return (send_server_msg(client, ERR_NEEDMOREPARAMS("JOIN")));
	if (args.second.size() == 2)
		keys = split_input(args.second[1], ",");
	// std::cout << "size chans: " << chans.size() << std::endl;
	// std::cout << "size x: " << x << std::endl;
	// for (i = 0; i < chans.size(); i++)
	// 	std::cout << "- \'" << chans[i] << "\'" << std::endl;
	// std::cout << "size: " << keys.size() << std::endl;
	// for (i = 0; i < keys.size(); i++)
	// 	std::cout << "- \'" << keys[i] << "\'" << std::endl;
	bool	flag;

	for (i = 0; i < chans.size(); i++)
	{
		if (chans[i][0] != '#' || std::count(chans[i].begin(), chans[i].end(), ' ') || std::count(chans[i].begin(), chans[i].end(), ',')
			|| chans[i].size() < 1 || chans[i].size() > 21)
			return (send_server_msg(client, ERR_BADCHANMASK(client.GetNickname(), chans[i])));
		flag = false;
		size_t	j;

		for (j = 0; j < this->Channels.size(); j++)
		{
			if (chans[i] == this->Channels[j].GetName())
			{
				std::vector<std::pair<Client, std::string> >			&Cmbs = this->Channels[j].GetMemberlist();
				std::vector<std::pair<Client, std::string> >::iterator	it = findclient(Cmbs, client.GetNickname());
				std::vector<Client>										&Invs = this->Channels[j].GetInvitedlist();

				if (it != Cmbs.end())
					continue;
				if (this->Channels[j].GetifInvonly() == true && this->find_nickname(client.GetNickname(), Invs) == -1)
					return (send_server_msg(client, ERR_INVITEONLYCHAN(client.GetNickname(), chans[i])));
				if (this->Channels[j].GetKey() != "" && (keys.empty() || (keys[i] != this->Channels[j].GetKey())))
					return (send_server_msg(client, ERR_BADCHANNELKEY(client.GetNickname(), chans[i])));
				if (Cmbs.size() == this->Channels[j].GetLimit())
					return (send_server_msg(client, ERR_CHANNELISFULL(client.GetNickname(), chans[i])));
				Cmbs.push_back(std::make_pair(client, "+"));
				send_to_all_in_chan(Cmbs, ":" + get_cli_source(client) + " JOIN :" + chans[i] + "\r\n");
				send_server_msg(client, RPL_TOPIC(client.GetNickname(), this->Channels[j].GetName(), this->Channels[j].GetTopic()));
				// std::string modes = "+";
				// std::string modeParams = "";

				// if (this->Channels[j].GetCantopic())
				// 	modes += "t";
				// if (this->Channels[j].GetifInvonly())
				// 	modes += "i";
				// if (!this->Channels[j].GetKey().empty())
				// {
				// 	modes += "k";
				// 	modeParams += " " + this->Channels[j].GetKey();
				// }
				// if (this->Channels[j].GetLimit() > 0)
				// {
				// 	modes += "l";
				// 	std::ostringstream ss;
				// 	ss << " " << this->Channels[j].GetLimit();
				// 	modeParams += ss.str();
				// }
				// send_server_msg(client, RPL_CHANNELMODEIS(client.GetNickname(), chans[i], modes + modeParams));
				std::ostringstream	ops;
				std::ostringstream	usrs;
				size_t				k;

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
				for (k = 0; k < Cmbs.size(); k++)
				{
					send_server_msg(Cmbs[k].first, RPL_NAMREPLY(Cmbs[k].first.GetNickname(), chans[i], ops.str() + usrs.str()));
					send_server_msg(Cmbs[k].first, RPL_ENDOFNAMES(Cmbs[k].first.GetNickname(), chans[i]));
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
		// std::string modes = "+";
		// std::string modeParams = "";

		// if (ch.GetCantopic())
		// 	modes += "t";
		// if (ch.GetifInvonly())
		// 	modes += "i";
		// if (!ch.GetKey().empty())
		// {
		// 	modes += "k";
		// 	modeParams += " " + ch.GetKey();
		// }
		// if (ch.GetLimit() > 0)
		// {
		// 	modes += "l";
		// 	std::ostringstream ss;
		// 	ss << " " << ch.GetLimit();
		// 	modeParams += ss.str();
		// }
		// send_server_msg(client, RPL_CHANNELMODEIS(client.GetNickname(), chans[i], modes + modeParams));
		std::ostringstream	tmp;
		tmp << "@" << client.GetNickname() << "\r\n";
		send_server_msg(client, RPL_NAMREPLY(client.GetNickname(), chans[i], tmp.str()));
		send_server_msg(client, RPL_ENDOFNAMES(client.GetNickname(), chans[i]));
	}
}

void	Server::privmsg(std::pair<std::string, std::vector<std::string> > args, Client &client)
{
	if (client.GetifReg() == false)
		return (send_server_msg(client, ERR_NOTREGISTERED("PRIVMSG")));
	if (args.second.size() < 2)
		return (send_server_msg(client, ERR_NEEDMOREPARAMS("PRIVMSG")));
	if (args.second.size() > 2)
		return (send_server_msg(client, ERR_TOOMANYPARAMS("PRIVMSG")));
	std::string	&message = args.second[1];
	size_t posa = message.find_last_not_of(" ");

	if (posa != std::string::npos)
		message.erase(posa + 1);
	if (message[0] == ':')
		message = message.substr(1);
	if (message[0] == ':')
		message = message.substr(1);
	if (message.empty())
		return (send_server_msg(client, ERR_NOTEXTTOSEND(client.GetNickname())));
	std::vector<std::string>	targets = split_input(args.second[0], ",");

	// std::cout << "size: " << targets.size() << std::endl;
	if (targets.empty())
		return (send_server_msg(client, ERR_NORECIPIENT(client.GetNickname())));
	// size_t	j;
	// for (j = 0; j < targets.size(); j++)
	// 	std::cout << "- \'" << targets[j] << "\'" << std::endl;
	size_t	x = std::count(args.second[0].begin(), args.second[0].end(), ',');

	if (targets.size() != x + 1)
		return (send_server_msg(client, ERR_NEEDMOREPARAMS("PRIVMSG")));
	for (size_t	i = 0; i < targets.size(); i++)
	{
		if (std::count(targets.begin(), targets.begin() + i, targets[i]) > 1)
			continue ;
		if (targets[i][0] == '#')
		{
			std::vector<Channel>::iterator	Cit = findchannel(this->Channels, targets[i]);

			if (Cit == this->Channels.end())
				send_server_msg(client, ERR_NOSUCHCHANNEL(client.GetNickname(), args.second[0]));
			else
			{
				std::vector<std::pair<Client, std::string> >	&Cmbs = Cit->GetMemberlist();

				for (size_t j = 0; j < Cmbs.size(); j++)
				{
					if (client.GetFd() == Cmbs[j].first.GetFd())
						continue ;
					send_server_msg(Cmbs[j].first, ":" + get_cli_source(client) + " PRIVMSG " + Cit->GetName() + " :" + message + "\r\n");
				}
			}
		}
		else
		{
			int	pos = this->find_nickname(targets[i], this->Clients);

			if (pos == -1)
				send_server_msg(client, ERR_NOSUCHNICK(targets[i], "PRIVMSG"));
			else
				send_server_msg(this->Clients[pos], ":" + get_cli_source(client) + " PRIVMSG " + this->Clients[pos].GetNickname() + " :" + message + "\r\n");
		}
	}
}

void	Server::topic(std::pair<std::string, std::vector<std::string> > args, Client &client)
{
	if (client.GetifReg() == false)
		return (send_server_msg(client, ERR_NOTREGISTERED("TOPIC")));
	if (args.second.size() < 1)
		return (send_server_msg(client, ERR_NEEDMOREPARAMS("TOPIC")));
	if (args.second.size() > 2)
		return (send_server_msg(client, ERR_TOOMANYPARAMS("TOPIC")));
	std::vector<Channel>::iterator	Cit = findchannel(this->Channels, args.second[0]);

	if (Cit == this->Channels.end())
		return (send_server_msg(client, ERR_NOSUCHCHANNEL(client.GetNickname(), args.second[0])));
	std::vector<std::pair<Client, std::string> > 			&Cmbs = Cit->GetMemberlist();
	std::vector<std::pair<Client, std::string> >::iterator	Mit = findclient(Cmbs, client.GetNickname());

	if (Mit == Cmbs.end())
		return (send_server_msg(client, ERR_NOTONCHANNEL(client.GetNickname(), Cit->GetName())));
	if (args.second.size() == 1)
	{
		if (Cit->GetTopic() == "")
			return (send_server_msg(client, RPL_NOTOPIC(client.GetNickname(), Cit->GetName())));
		return send_server_msg(client, RPL_TOPIC(client.GetNickname(), Cit->GetName(), Cit->GetTopic()));
	}
	else
	{
		if (Cit->GetCantopic() == true && Mit->second != "@")
			return (send_server_msg(client, ERR_CHANOPRIVSNEEDED(client.GetNickname(), Cit->GetName())));
		std::string	&topic = args.second[1];

		if (topic.size() >= 2 && topic[0] == ':' && topic[1] == ':')
			topic = topic.substr(1);
		size_t pos = topic.find_last_not_of(" ");

		if (pos != std::string::npos)
			topic.erase(pos + 1);
		if (topic.size() == 1)
			Cit->SetTopic("");
		else
			Cit->SetTopic(topic.substr(1));
		send_to_all_in_chan(Cmbs, RPL_TOPIC(client.GetNickname(), Cit->GetName(), Cit->GetTopic()));
	}
}

void	Server::kick(std::pair<std::string, std::vector<std::string> > args, Client &client)
{
	if (client.GetifReg() == false)
		return (send_server_msg(client, ERR_NOTREGISTERED("KICK")));
	if (args.second.size() < 2)
		return (send_server_msg(client, ERR_NEEDMOREPARAMS("KICK")));
	if (args.second.size() > 3)
		return (send_server_msg(client, ERR_TOOMANYPARAMS("KICK")));
	std::vector<Channel>::iterator	Cit = findchannel(this->Channels, args.second[0]);

	if (Cit == this->Channels.end())
		return (send_server_msg(client, ERR_NOSUCHCHANNEL(client.GetNickname(), args.second[0])));
	std::vector<std::pair<Client, std::string> >			&Cmbs = Cit->GetMemberlist();
	std::vector<std::pair<Client, std::string> >::iterator	kickerIt = findclient(Cmbs, client.GetNickname());
	
	if (kickerIt == Cmbs.end())
		return (send_server_msg(client, ERR_NOTONCHANNEL(client.GetNickname(), Cit->GetName())));
	if (kickerIt->second != "@")
		return (send_server_msg(client, ERR_CHANOPRIVSNEEDED(client.GetNickname(), Cit->GetName())));
	std::vector<std::string>	targets = split_input(args.second[1], ",");
	std::string					kickMsg = args.second.size() == 3 ? args.second[2] : ":No reason given";
	
	if (kickMsg[0] == ':')
		kickMsg = kickMsg.substr(1);
	if (kickMsg[0] == ':')
		kickMsg = kickMsg.substr(1);
	size_t pos = kickMsg.find_last_not_of(" ");

	if (pos != std::string::npos)
		kickMsg.erase(pos + 1);
	for (size_t i = 0; i < targets.size(); i++)
	{
		std::vector<std::pair<Client, std::string> >::iterator	targetIt = findclient(Cmbs, targets[i]);
		std::vector<Client> 									&Invs = Cit->GetInvitedlist();

		if (targetIt == Cmbs.end())
		{
			send_server_msg(client, ERR_USERNOTINCHANNEL(client.GetNickname(), targets[i], Cit->GetName()));
			continue ;
		}
		if (targetIt->second == "@")
		{
			send_server_msg(client, ERR_USERISOPERATOR(client.GetNickname(), targets[i], Cit->GetName()));
			continue ;
		}
		if (targetIt->first.GetFd() == kickerIt->first.GetFd())
		{
			send_server_msg(client, ERR_CANTKICKYOURSELF(client.GetNickname(), targets[i], Cit->GetName()));
			continue ;
		}
		send_to_all_in_chan(Cmbs, ":" + get_cli_source(client) + " KICK " + Cit->GetName() + " " + targetIt->first.GetNickname() + " :" + kickMsg + "\r\n");
		int pos = this->find_nickname(targetIt->first.GetNickname(), Invs);

		if (pos != -1)
			Invs.erase(Invs.begin() + pos);
		Cmbs.erase(targetIt);
	}
}

void	Server::part(std::pair<std::string, std::vector<std::string> > args, Client &client)
{
	if (client.GetifReg() == false)
		return (send_server_msg(client, ERR_NOTREGISTERED("PART")));
	if (args.second.empty())
		return (send_server_msg(client, ERR_NEEDMOREPARAMS("PART")));
	if (args.second.size() > 2)
		return (send_server_msg(client, ERR_TOOMANYPARAMS("PART")));
	std::string message = args.second.size() == 2 ? args.second[1] : ":Leaving";
	
	if (message[0] == ':')
		message = message.substr(1);
	if (message[0] == ':')	
		message = message.substr(1);
	size_t pos = message.find_last_not_of(" ");

	if (pos != std::string::npos)
		message.erase(pos + 1);
	std::vector<std::string> targets = split_input(args.second[0], ",");
	size_t	x = std::count(args.second[0].begin(), args.second[0].end(), ',');

	if (targets.size() != x + 1)
		return (send_server_msg(client, ERR_NEEDMOREPARAMS("PART")));
	for (size_t i = 0; i < targets.size(); i++)
	{
		std::vector<Channel>::iterator Cit = findchannel(this->Channels, targets[i]);

		if (Cit == this->Channels.end())
		{
			send_server_msg(client, ERR_NOSUCHCHANNEL(client.GetNickname(), targets[i]));
			continue ;
		}
		std::vector<std::pair<Client, std::string> >			&Cmbs = Cit->GetMemberlist();
		std::vector<std::pair<Client, std::string> >::iterator	Mit = findclient(Cmbs, client.GetNickname());

		if (Mit == Cmbs.end())
		{
			send_server_msg(client, ERR_NOTONCHANNEL(client.GetNickname(), targets[i]));
			continue ;
		}
		send_to_all_in_chan(Cmbs, ":" + get_cli_source(client) + " PART " + targets[i] + " :" + message + "\r\n");
		if (Mit->second == "@" && Cmbs.size() >= 2 && std::count_if(Cmbs.begin(), Cmbs.end(), IsSymbol("@")) == 1)
		{
			std::vector<std::pair<Client, std::string> >::iterator nextOp = comparesymbol(Cmbs, "+");

			if (nextOp != Cmbs.end())
			{
				nextOp->second = "@";                                     
				for (size_t j = 0; j < Cmbs.size(); j++)
				{
					if (client.GetFd() == Cmbs[j].first.GetFd())
						continue ;
					send_server_msg(Cmbs[j].first, ":ircserv MODE " + targets[i] + " +o " + nextOp->first.GetNickname() + "\r\n");
				}
			}
		}
		Cmbs.erase(Mit);
		if (Cmbs.empty()) 
			this->Channels.erase(Cit);
	}
}

void	Server::mode(std::pair<std::string, std::vector<std::string> > args, Client &client)
{
	if (client.GetifReg() == false)
		return (send_server_msg(client, ERR_NOTREGISTERED("MODE")));
	if (args.second.size() < 1)
		return (send_server_msg(client, ERR_NEEDMOREPARAMS("MODE")));
	if (args.second.size() > 3)
		return (send_server_msg(client, ERR_TOOMANYPARAMS("MODE")));
	std::vector<Channel>::iterator Cit = findchannel(this->Channels, args.second[0]);

	if (Cit == this->Channels.end())
		return (send_server_msg(client, ERR_NOSUCHCHANNEL(client.GetNickname(), args.second[0])));

	if (args.second.size() == 1)
	{
		std::string modes = "+";
		std::string modeParams = "";

		if (Cit->GetCantopic())
			modes += "t";
		if (Cit->GetifInvonly())
			modes += "i";
		if (!Cit->GetKey().empty())
		{
			modes += "k";
			modeParams += " " + Cit->GetKey();
		}
		if (Cit->GetLimit() > 0)
		{
			modes += "l";
			std::ostringstream ss;
			ss << " " << Cit->GetLimit();
			modeParams += ss.str();
		}
		send_server_msg(client, RPL_CHANNELMODEIS(client.GetNickname(), Cit->GetName(), modes + modeParams));
		return ;
	}
	std::vector<std::pair<Client, std::string> > &Cmbs = Cit->GetMemberlist();
	std::vector<std::pair<Client, std::string> >::iterator Mit = findclient(Cmbs, client.GetNickname());
	
	if (Mit == Cmbs.end())
		return (send_server_msg(client, ERR_NOTONCHANNEL(client.GetNickname(), Cit->GetName())));
	if (Mit->second != "@")
		return (send_server_msg(client, ERR_CHANOPRIVSNEEDED(client.GetNickname(), Cit->GetName())));
	std::string modeString = args.second[1];
	std::string parameter = args.second.size() > 2 ? args.second[2] : "";
	bool adding = true;

	for (size_t i = 0; i < modeString.length(); i++)
	{
		char c = modeString[i];
		
		switch (c)
		{
			case '+':
				adding = true;
				break;
			case '-':
				adding = false;
				break;
			case 'i':
				Cit->Setifinvonly(adding);
				send_to_all_in_chan(Cmbs, ":" + get_cli_source(client) + " MODE " + Cit->GetName() + 
					" " + (adding ? "+" : "-") + "i\r\n");
				break;
			case 't':
				Cit->SetCantopic(adding);
				send_to_all_in_chan(Cmbs, ":" + get_cli_source(client) + " MODE " + Cit->GetName() + 
					" " + (adding ? "+" : "-") + "t\r\n");
				break;
			case 'k':
				Cit->SetKey(adding ? parameter : "");
				send_to_all_in_chan(Cmbs, ":" + get_cli_source(client) + " MODE " + Cit->GetName() + 
					" " + (adding ? "+" : "-") + "k " + parameter + "\r\n");
				break;
			case 'o':
				{
					if (parameter.empty()) {
						send_server_msg(client, ERR_NEEDMOREPARAMS("MODE"));
						break;
					}
					std::vector<std::pair<Client, std::string> >::iterator target = findclient(Cmbs, parameter);

					if (target != Cmbs.end())
					{
						if (!adding && target->first.GetNickname() == client.GetNickname())
						{
							send_server_msg(client, ERR_CANTDEMOTEYOURSELF(client.GetNickname(), target->first.GetNickname(),Cit->GetName()));
							break;
						}
						target->second = adding ? "@" : "+";
						send_to_all_in_chan(Cmbs, ":" + get_cli_source(client) + " MODE " + Cit->GetName() + " " + (adding ? "+" : "-") + "o " + parameter + "\r\n");
						std::string namesList;

						for (size_t k = 0; k < Cmbs.size(); k++) //TODO to modify
						{
							if (k > 0)
								namesList += " ";
							namesList += Cmbs[k].second + Cmbs[k].first.GetNickname();
						}
						namesList += "\r\n";

						// Send updated names list to all members
						for (size_t k = 0; k < Cmbs.size(); k++) 
						{
							send_server_msg(Cmbs[k].first, RPL_NAMREPLY(Cmbs[k].first.GetNickname(), Cit->GetName(), namesList));
							send_server_msg(Cmbs[k].first, RPL_ENDOFNAMES(Cmbs[k].first.GetNickname(), Cit->GetName()));
						}
					}
					else
						send_server_msg(client, ERR_NOSUCHNICK(parameter, "MODE")); 
				}
				break;
			case 'l':
				Cit->SetLimit(adding ? std::atoi(parameter.c_str()) : 0);
				send_to_all_in_chan(Cmbs, ":" + get_cli_source(client) + " MODE " + Cit->GetName() + " " + (adding ? "+" : "-") + "l " + parameter + "\r\n");
				break;
			default:
				send_server_msg(client, ERR_UNKNOWNMODE(client.GetNickname(), std::string(1, c)));
				break;
		}
	}
}

void	Server::invite(std::pair<std::string, std::vector<std::string> > args, Client &client)
{
	if (client.GetifReg() == false)
		return (send_server_msg(client, ERR_NOTREGISTERED("INVITE")));
	if (args.second.size() < 2)
		return (send_server_msg(client, ERR_NEEDMOREPARAMS("INVITE")));
	if (args.second.size() > 2)
		return (send_server_msg(client, ERR_TOOMANYPARAMS("INVITE")));
	std::vector<Channel>::iterator Cit = findchannel(this->Channels, args.second[1]);

	if (Cit == this->Channels.end())
		return (send_server_msg(client, ERR_NOSUCHCHANNEL(client.GetNickname(), args.second[1])));
	std::vector<std::pair<Client, std::string> > &Cmbs = Cit->GetMemberlist();
	std::vector<std::pair<Client, std::string> >::iterator inviterIt = findclient(Cmbs, client.GetNickname());

	if (inviterIt == Cmbs.end())
		return (send_server_msg(client, ERR_NOTONCHANNEL(client.GetNickname(), Cit->GetName())));
	int targetIndex = find_nickname(args.second[0], this->Clients);

	if (targetIndex == -1)
		return (send_server_msg(client, ERR_NOSUCHNICK(args.second[0], "INVITE")));
	// Check if target is already in channel
	std::vector<std::pair<Client, std::string> >::iterator	targetIt = findclient(Cmbs, args.second[0]);

	if (targetIt != Cmbs.end())
		return (send_server_msg(client, ERR_USERONCHANNEL(client.GetNickname(), args.second[0], Cit->GetName())));
	std::vector<Client>										&Invs = Cit->GetInvitedlist();

	// std::cout << "HNA: " << targetIt->first.GetNickname() << std::endl;
	Invs.push_back(this->Clients[targetIndex]);
	send_server_msg(this->Clients[targetIndex], ":" + get_cli_source(client) + " INVITE " + args.second[0] + " " + Cit->GetName() + "\r\n");
	send_server_msg(client, RPL_INVITING(client.GetNickname(), Cit->GetName(), args.second[0]));
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
	else if (args.first =="MODE" || args.first == "mode")
		mode(args, client);
	else if (args.first == "PART" || args.first == "part")
		part(args, client);
	else if (args.first == "KICK" || args.first == "kick")
		kick(args, client);
	else if (args.first == "INVITE" || args.first == "invite")
		invite(args, client);
	else
		send_server_msg(client, ERR_UNKNOWNCOMMAND(client.GetNickname(), args.first));
	if (client.GetifReg() == false && client.GetIfPassCorr() == true && client.GetNickname() != "*" && client.GetUsername() != "*" && client.GetRealname() != "*")
	{
		client.SetifReg(true);
		return (send_server_msg(client, RPL_WELCOME(client.GetNickname(), client.GetUsername(), client.GetIPaddr())));
	}
}
