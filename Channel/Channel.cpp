/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: htouil <htouil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/30 16:46:42 by htouil            #+#    #+#             */
/*   Updated: 2024/12/03 23:09:02 by htouil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel(std::string Name, std::string Key) : Name(Name), Key(Key), Topic("*")
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

std::string	Channel::GetTopic()
{
	return (this->Topic);
}

void	Channel::SetTopic(std::string newTopic)
{
	this->Topic = newTopic;
}

std::vector<int>	Channel::GetMemberlist()
{
	return (this->Members);
}
