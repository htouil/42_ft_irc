/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helper.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: htouil <htouil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/23 20:21:59 by htouil            #+#    #+#             */
/*   Updated: 2024/11/23 22:50:16 by htouil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./Server/Server.hpp"

int		Server::find_nickname(std::string to_find)
{
	size_t	i;

	for (i = 0; i < this->Clients.size(); i++)
	{
		if (this->Clients[i].GetNickname() == to_find)
			return (i);
	}
	return (-1);
}

int		Server::find_fd(int to_find)
{
	size_t	i;

	for (i = 0; i < this->Clients.size(); i++)
	{

		if (this->Clients[i].GetFd() == to_find)
			return (i);
	}
	return (-1);
}
