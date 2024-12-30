/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amirabendhia <amirabendhia@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 00:02:13 by htouil            #+#    #+#             */
/*   Updated: 2024/12/30 06:26:24 by amirabendhi      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./Server/Server.hpp"

class NicknameMatcher {
	std::string nickname_;
public:
	NicknameMatcher(const std::string& nick) : nickname_(nick) {}
	bool operator()(const std::pair<Client, std::string>& member) const {
		return member.first.GetNickname() == nickname_;
	}
};

void	Server::send_server_msg(Client &client, std::string msg)
{
	ssize_t sent = send(client.GetFd(), msg.c_str(), msg.size(), 0);
	if (sent == -1) {
		std::cerr << "Failed to send to client " << client.GetFd() 
				 << " (" << client.GetNickname() << "): " << strerror(errno) << std::endl;
	} else if (sent < static_cast<ssize_t>(msg.size())) {
		std::cerr << "Partial send to client " << client.GetFd() 
				 << ": " << sent << "/" << msg.size() << " bytes" << std::endl;
	}
}

void	Server::send_to_all_in_chan(std::vector<std::pair<Client, std::string> > &Cmbs, std::string msg)
{
	std::vector<std::pair<Client, std::string> >::iterator it;
	for (it = Cmbs.begin(); it != Cmbs.end(); ++it) {
		send_server_msg(it->first, msg);
	}
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
		return (send_server_msg(client, ERR_TOOMANYPARAMS(client.GetNickname(), "QUIT")));
	size_t	i;
	std::string	&reason = args.second[0];
	size_t pos;

	pos = args.second[0].find_last_not_of(" ");
	if (pos != std::string::npos)
		args.second[0].erase(pos + 1);
	if (reason[0] == ':' && reason[1] == ':')
		reason = reason.substr(1);
	reason = reason.substr(1);
	// std::cout << "reason: " << reason << std::endl;
	send_server_msg(client, "ERROR :Closing Link: " + client.GetNickname() + " (" + reason + ")\r\n");
	for (i = 0; i < this->Channels.size(); i++)
	{
		std::vector<std::pair<Client, std::string> >				&Cmbs = this->Channels[i].GetMemberlist();
		std::vector<std::pair<Client, std::string> >::iterator		it1;
		std::vector<std::pair<Client, std::string> >::iterator		it2;

		it1 = findclient(Cmbs, client); 
		if (it1 != Cmbs.end())
		{
			send_to_all_in_chan(Cmbs, ":" + get_cli_source(client) + " QUIT :Quit: " + reason + "\r\n");
			if (it1->second == "@" && Cmbs.size() >= 2 && std::count_if(Cmbs.begin(), Cmbs.end(), IsSymbol("@")) == 1)
			{
				it2 = comparesymbol(Cmbs, "+");
				if (it2 != Cmbs.end())
					send_server_msg(it2->first, ":ircserv MODE " + this->Channels[i].GetName() + " +o " + it2->first.GetNickname() + "\r\n");
			}
			Cmbs.erase(it1);
			if (Cmbs.empty())
				this->Channels.erase(this->Channels.begin() + i);
			// if (args.second.size() == 0)
			// {
			// 	for (j = 0; j < Cmbs.size(); j++)
			// 		send_server_msg(Cmbs[i].first, ":" + get_cli_source(client) + " QUIT :Quit: \r\n");
			// }
			// else
			// {
			// 	for (j = 0; j < Cmbs.size(); j++)
			// }
			// Cmbs.erase(it);
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
	help.append("\n6. PRIVMSG:\n");
	help.append("  - Description: Sends a private message to a user or channel.\n");
	help.append("  - Usage: PRIVMSG <recipient> <message>\n");
	help.append("  - Note: Ensure the recipient (user or channel) is valid and connected.\n");
	help.append("\n7. TOPIC:\n");
	help.append("  - Description: Sets or retrieves the topic of a specified channel.\n");
	help.append("  - Usage: TOPIC <channel> [<topic>]\n");
	help.append("  - Note: If the topic provided is empty, the channel's topic will be set to empty.\n");
	help.append("\n8. HELP:\n");
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
		return (send_server_msg(client, ERR_NOTREGISTERED(client.GetNickname())));
	if (args.second.empty())
		return (send_server_msg(client, ERR_NEEDMOREPARAMS(client.GetNickname(), "JOIN")));
	if (args.second.size() > 2)
		return (send_server_msg(client, ERR_TOOMANYPARAMS(client.GetNickname(), "JOIN")));
	size_t	i;

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
				send_to_all_in_chan(Cmbs, ":" + get_cli_source(client) + " PART :" + this->Channels[i].GetName() + "\r\n");
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
	if (args.second.size() > 0 && chans.size() != x + 1)
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
				std::vector<std::pair<Client, std::string> >::iterator	it;

				it = findclient(Cmbs, client);
				if (it != Cmbs.end())
					continue ;
				if (this->Channels[j].GetifInvonly() == true)
					return (send_server_msg(client, ERR_INVITEONLYCHAN(client.GetNickname(), chans[i])));
				if (this->Channels[j].GetKey() != "" && (keys.empty() || (keys[i] != this->Channels[j].GetKey())))
					return (send_server_msg(client, ERR_BADCHANNELKEY(client.GetNickname(), chans[i])));
				if (Cmbs.size() == this->Channels[j].GetLimit())
					return (send_server_msg(client, ERR_CHANNELISFULL(client.GetNickname(), chans[i])));
				Cmbs.push_back(std::make_pair(client, "+"));
				send_to_all_in_chan(Cmbs, ":" + get_cli_source(client) + " JOIN :" + chans[i] + "\r\n");
				// if (this->Channels[j].GetTopic() != "")
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
					send_server_msg(Cmbs[k].first, RPL_NAMREPLY(Cmbs[k].first.GetNickname(), (this->Channels[j].GetName()), ops.str() + usrs.str()));
					send_server_msg(Cmbs[k].first, RPL_ENDOFNAMES(Cmbs[k].first.GetNickname(), this->Channels[j].GetName()));	
				}
				// std::cout << "Channel size: " << Cmbs.size() << std::endl;
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
	std::string	&message = args.second[1];
	size_t posa;

	posa = message.find_last_not_of(" ");
	if (posa != std::string::npos)
		message.erase(posa + 1);
	if (message[0] == ':' && message[1] == ':')
		message = message.substr(1);
	message = message.substr(1);
	if (message.empty())
		return (send_server_msg(client, ERR_NOTEXTTOSEND(client.GetNickname())));
	targets = split_input(args.second[0], ",");
	// std::cout << "size: " << targets.size() << std::endl;
	if (targets.empty())
		return (send_server_msg(client, ERR_NORECIPIENT(client.GetNickname())));
	size_t	j;
	// for (j = 0; j < targets.size(); j++)
	// 	std::cout << "- \'" << targets[j] << "\'" << std::endl;
	x = std::count(args.second[0].begin(), args.second[0].end(), ',');
	if (targets.size() != x + 1)
		return (send_server_msg(client, ERR_NEEDMOREPARAMS(client.GetNickname(), "PRIVMSG")));
	size_t	i;
	for (i = 0; i < targets.size(); i++)
	{
		if (std::count(targets.begin(), targets.begin() + i, targets[i]) > 1)
			continue ;
		if (targets[i][0] == '#')
		{
			std::vector<Channel>::iterator	it;

			it = findchannel(this->Channels, targets[i]);
			if (it == this->Channels.end())
				send_server_msg(client, ERR_NOSUCHCHANNEL(client.GetNickname(), args.second[0]));
			else
			{
				std::vector<std::pair<Client, std::string> >	&Cmbs = it->GetMemberlist();

				size_t	i;

				for (i = 0; i < Cmbs.size(); i++)
				{
					if (client.GetFd() == Cmbs[i].first.GetFd())
						continue ;
					send_server_msg(Cmbs[i].first, ":" + get_cli_source(client) + " PRIVMSG " + it->GetName() + " :" + message + "\r\n");
				}
			}
		}
		else
		{
			int	pos;

			pos = this->find_nickname(targets[i], this->Clients);
			if (pos == -1)
				send_server_msg(client, ERR_NOSUCHNICK(targets[i], "PRIVMSG", "nick"));
			else
				send_server_msg(this->Clients[pos], ":" + get_cli_source(client) + " PRIVMSG " + this->Clients[pos].GetNickname() + " :" + message + "\r\n");
		}
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

		if (topic.size() >= 2 && topic[0] == ':' && topic[1] == ':')
			topic = topic.substr(1);
		size_t pos;

		pos = topic.find_last_not_of(" ");
		if (pos != std::string::npos)
			topic.erase(pos + 1);
		if (topic.size() == 1)
			Cit->SetTopic("");
		else
			Cit->SetTopic(topic.substr(1));
		send_to_all_in_chan(Cmbs, RPL_TOPIC(client.GetNickname(), Cit->GetName(), Cit->GetTopic()));
	}
}

void	Server::mode(std::pair<std::string, std::vector<std::string> > args, Client &client)
{
	if (client.GetifReg() == false)
		return (send_server_msg(client, ERR_NOTREGISTERED(client.GetNickname())));
	if (args.second.size() < 1)
		return (send_server_msg(client, ERR_NEEDMOREPARAMS(client.GetNickname(), "MODE")));
	if (args.second.size() > 3)
		return (send_server_msg(client, ERR_TOOMANYPARAMS(client.GetNickname(), "MODE")));

	std::vector<Channel>::iterator Cit = findchannel(this->Channels, args.second[0]);
	if (Cit == this->Channels.end())
		return (send_server_msg(client, ERR_NOSUCHCHANNEL(client.GetNickname(), args.second[0])));

	if (args.second.size() == 1) {
		// TODO: Implement showing current channel modes
		return;
	}

	std::vector<std::pair<Client, std::string> > &Cmbs = Cit->GetMemberlist();
	std::vector<std::pair<Client, std::string> >::iterator Mit = findclient(Cmbs, client);
	
	if (Mit == Cmbs.end())
		return (send_server_msg(client, ERR_NOTONCHANNEL(client.GetNickname(), Cit->GetName())));
	
	if (Mit->second != "@")
		return (send_server_msg(client, ERR_CHANOPRIVSNEEDED(client.GetNickname(), Cit->GetName())));

	std::string modeString = args.second[1];
	std::string parameter = args.second.size() > 2 ? args.second[2] : "";
	bool adding = true;

	for (size_t i = 0; i < modeString.length(); i++) {
		char c = modeString[i];
		
		switch (c) {
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
						send_server_msg(client, ERR_NEEDMOREPARAMS(client.GetNickname(), "MODE"));
						break;
					}
					
					std::vector<std::pair<Client, std::string> >::iterator target = std::find_if(
						Cmbs.begin(), 
						Cmbs.end(),
						NicknameMatcher(parameter)
					);

					if (target != Cmbs.end()) {
						if (!adding && target->first.GetNickname() == client.GetNickname()) {
							send_server_msg(client, ERR_CHANOPRIVSNEEDED(client.GetNickname(), Cit->GetName()));
							break;
						}
						
						target->second = adding ? "@" : "+";
						send_to_all_in_chan(Cmbs, ":" + get_cli_source(client) + " MODE " + Cit->GetName() + 
							" " + (adding ? "+" : "-") + "o " + parameter + "\r\n");
					} else {
						send_server_msg(client, ERR_NOSUCHNICK(parameter, "MODE", "nick")); 
					}
				}
				break;
			case 'l':
				Cit->SetLimit(adding ? std::atoi(parameter.c_str()) : 0);
				send_to_all_in_chan(Cmbs, ":" + get_cli_source(client) + " MODE " + Cit->GetName() + 
					" " + (adding ? "+" : "-") + "l " + parameter + "\r\n");
				break;
			default:
				send_server_msg(client, ERR_UNKNOWNMODE(client.GetNickname(), std::string(1, c)));
				break;
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
	else if (args.first =="MODE" || args.first == "mode")
		mode(args, client);
	// else
	// 	send_server_msg(client, ERR_UNKNOWNCOMMAND(client.GetNickname(), args.first));
	if (client.GetifReg() == false && client.GetIfPassCorr() == true && client.GetNickname() != "*" && client.GetUsername() != "*" && client.GetRealname() != "*")
	{
		client.SetifReg(true);
		return (send_server_msg(client, RPL_WELCOME(client.GetNickname(), client.GetUsername(), client.GetIPaddr())));
	}
}
