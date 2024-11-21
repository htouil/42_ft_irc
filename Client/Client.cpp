/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: htouil <htouil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/15 23:49:35 by htouil            #+#    #+#             */
/*   Updated: 2024/11/20 23:08:01 by htouil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client()
{
}

int	Client::GetFd()
{
	return (this->Fd);
}

void	Client::SetFd(int newFd)
{
	this->Fd = newFd;
}

std::string	Client::GetNickname()
{
	return (this->Nickname);
}

void	Client::SetNickname(std::string newName)
{
	this->Nickname = newName;
}

std::string	Client::GetUsername()
{
	return (this->Username);
}

void	Client::SetUsername(std::string newName)
{
	this->Username = newName;
}

std::string	Client::GetIPaddr()
{
	return (this->IPaddr);
}

void	Client::SetIPaddr(std::string newIPaddr)
{
	this->IPaddr = newIPaddr;
}

bool	Client::GetifReg()
{
	return (this->Registered);
}

void	Client::SetifReg(bool reg)
{
	this->Registered = reg;
}
