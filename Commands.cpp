/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: htouil <htouil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 00:02:13 by htouil            #+#    #+#             */
/*   Updated: 2024/11/25 23:21:36 by htouil           ###   ########.fr       */
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

void	display_err_msg(Client &client, std::string err_msg)
{
	std::string	tmp;

	tmp = display_current_time() + err_msg;
	if (send(client.GetFd(), (tmp).c_str(), tmp.size(), 0) == -1)
		std::cerr << "Failed to send to client " << client.GetFd() << std::endl;
}

void	Server::commands(std::pair<std::string, std::vector<std::string> > args, Client &client)
{
	if (args.first == "PASS" || args.first == "pass")
	{
		if (client.GetifReg() == true)
			return (display_err_msg(client, ERR_ALREADYREGISTERED(client.GetNickname())));
		if (args.second.size() < 1)
			return (display_err_msg(client, ERR_NOTENOUGHPARAMS(client.GetNickname())));
		if (args.second.size() > 1)
			return (display_err_msg(client, ERR_TOOMANYPARAMS(client.GetNickname())));
		if (args.second.front() != this->Spassword)
			return (display_err_msg(client, ERR_PASSWDMISMATCH(client.GetNickname())));
		client.SetPassword(true);
	}
	else if (args.first == "NICK" || args.first == "nick")
	{
		if (client.GetPassword() == false)
			return (display_err_msg(client, ERR_PASSWDMISMATCH(client.GetNickname())));
		if (args.second.size() < 1)
			return (display_err_msg(client, ERR_NONICKNAMEGIVEN(client.GetNickname())));
		if (args.second.size() > 1)
			return (display_err_msg(client, ERR_TOOMANYPARAMS(client.GetNickname())));
		if (this->find_nickname(args.second.front()) > -1)
			return (display_err_msg(client, ERR_NICKNAMEINUSE(client.GetNickname(), args.second.front())));
		std::string	tmp = args.second.front();
		if (tmp[0] == '#' || tmp[0] == '&' || tmp[0] == ':' || std::isdigit(tmp[0]))
			return (display_err_msg(client, ERR_ERRONEUSNICKNAME(client.GetNickname(), args.second.front())));
		size_t	i;
		for (i = 0; i < tmp.size(); i++)
		{
			if (tmp[i] == ' ' || (!std::isalpha(tmp[i]) && !std::isdigit(tmp[i]) && tmp[i] != '[' && tmp[i] != ']' && tmp[i] != '{' && tmp[i] != '}' && tmp[i] != '\\' && tmp[i] != '|' && (i > 0 && tmp[i] != ':')))
				return (display_err_msg(client, ERR_ERRONEUSNICKNAME(client.GetNickname(), args.second.front())));
		}
		if (client.GetifReg() == true)
			display_err_msg(client, ":" + client.GetNickname() + " NICK " + args.second.front());
		client.SetNickname(args.second.front());
	}
	else if (args.first == "USER" || args.first == "user")
	{
		if (client.GetPassword() == false)
			return (display_err_msg(client, ERR_PASSWDMISMATCH(client.GetNickname())));
	}
}
