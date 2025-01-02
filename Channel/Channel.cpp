/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: htouil <htouil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/30 16:46:42 by htouil            #+#    #+#             */
/*   Updated: 2025/01/02 22:08:58 by htouil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel(std::string Name, std::string Key) : Name(Name), Key(Key), Topic(""), Limit(10), Inv_only(false), Can_topic(true)
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

size_t	Channel::GetLimit()
{
	return (this->Limit);
}

void	Channel::SetLimit(size_t newLimit)
{
	this->Limit = newLimit;
}

bool	Channel::GetifInvonly()
{
	return (this->Inv_only);
}
void	Channel::Setifinvonly(bool ifinv)
{
	this->Inv_only = ifinv;
}
bool	Channel::GetCantopic()
{
	return (this->Can_topic);
}
void	Channel::SetCantopic(bool iftopic)
{
	this->Can_topic = iftopic;
}

std::vector<std::pair<Client, std::string> >	&Channel::GetMemberlist()
{
	return (this->Members);
}

std::vector<Client>	&Channel::GetInvitedlist()
{
	return (this->Invited);
}
