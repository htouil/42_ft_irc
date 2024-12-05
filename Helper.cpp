/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Helper.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: htouil <htouil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/23 20:21:59 by htouil            #+#    #+#             */
/*   Updated: 2024/12/05 22:57:01 by htouil           ###   ########.fr       */
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
