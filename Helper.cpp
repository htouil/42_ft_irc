/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Helper.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: htouil <htouil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/23 20:21:59 by htouil            #+#    #+#             */
/*   Updated: 2025/01/03 20:07:00 by htouil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./Server/Server.hpp"

std::vector<std::string>	split_input(std::string buffer, std::string delimiter)
{
	std::vector<std::string>	cmd;
	size_t						start = 0;
	size_t						end = 0;

	while (end != std::string::npos)
	{
		end = buffer.find(delimiter, start);
		cmd.push_back(buffer.substr(start, end - start));
		start = end + delimiter.length();
	}
	return (cmd);
}

std::string	remove_crln(std::string msg)
{
	size_t	pos;

	pos = msg.find_first_of("\r\n");
	if (pos != std::string::npos)
		return (msg.substr(0, pos));
	return (msg);
}


int		Server::find_nickname(std::string to_find, std::vector<Client> list)
{
	size_t	i;

	for (i = 0; i < list.size(); i++)
	{
		if (list[i].GetNickname() == to_find)
			return (i);
	}
	return (-1);
}

int		Server::find_fd(int to_find, std::vector<Client> list)
{
	size_t	i;

	for (i = 0; i < list.size(); i++)
	{
		if (list[i].GetFd() == to_find)
			return (i);
	}
	return (-1);
}

int		Server::find_IP(std::string to_find, std::vector<Client> list)
{
	size_t	i;

	for (i = 0; i < list.size(); i++)
	{
		if (list[i].GetIPaddr() == to_find)
			return (i);
	}
	return (-1);
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

std::string	get_cli_source(Client cli)
{
	std::string	source;

	source = cli.GetNickname() + "!" + cli.GetUsername() + "@" + cli.GetIPaddr();
	return (source);
}

std::vector<std::pair<Client, std::string> >::iterator	comparesymbol(std::vector<std::pair<Client, std::string> > &Cmbs, std::string symbol)
{
	std::vector<std::pair<Client, std::string> >::iterator	it;

	for (it = Cmbs.begin(); it != Cmbs.end(); ++it)
	{
		if (it->second == symbol)
			return (it);
	}
	return (Cmbs.end());
}

std::vector<std::pair<Client, std::string> >::iterator	findclient(std::vector<std::pair<Client, std::string> > &Cmbs, std::string target)
{
	std::vector<std::pair<Client, std::string> >::iterator	it;

	for (it = Cmbs.begin(); it != Cmbs.end(); ++it)
	{
		if (it->first.GetNickname() == target)
			return (it);
	}
	return (Cmbs.end());
}


std::vector<Channel>::iterator	findchannel(std::vector<Channel> &Chans, std::string target)
{
	std::vector<Channel>::iterator	it;

	for (it = Chans.begin(); it != Chans.end(); ++it)
	{
		if (it->GetName() == target)
			return (it);
	}
	return (Chans.end());
}
