/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: htouil <htouil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/30 16:46:42 by htouil            #+#    #+#             */
/*   Updated: 2024/12/05 23:26:57 by htouil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel(std::string Name, std::string Key) : Name(Name), Key(Key), Topic("*"), inv_only(false)
{
}

std::string	Channel::GetName()
{
	return (this->Name);
}

void	Channel::SetName(std::string newName)
{
	this->Name = newName;
}

std::string	Channel::GetKey()
{
	return (this->Key);
}

void	Channel::SetKey(std::string newKey)
{
	this->Key = newKey;
}

std::string	Channel::GetTopic()
{
	return (this->Topic);
}

void	Channel::SetTopic(std::string newTopic)
{
	this->Topic = newTopic;
}

bool	Channel::Getifinvonly()
{
	return (this->inv_only);
}

std::vector<Client>	Channel::GetMemberlist()
{
	return (this->Members);
}

std::vector<Client>	Channel::GetBannedlist()
{
	return (this->Banned);
}

