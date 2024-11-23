/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: htouil <htouil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 00:02:13 by htouil            #+#    #+#             */
/*   Updated: 2024/11/23 01:52:36 by htouil           ###   ########.fr       */
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

void	display_err_msg(Client &client, std::string err_msg)
{
	if (send(client.GetFd(), err_msg.c_str(), err_msg.size(), 0) == -1)
		std::cerr << "Failed to send to client " << client.GetFd() << std::endl;
}

void	commands(std::pair<std::string, std::vector<std::string>	> args, Client &client)
{
	std::string	pwdinput;

	if (client.GetifReg() == false)
	{
		if (args.first == "PASS" || args.first == "pass")
		{
			if (args.second.size() != 1)
				return (display_err_msg(client, ERR_NOTENOUGHPARAMS(client.GetNickname())));
		}
	}
}
