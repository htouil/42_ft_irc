/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amirabendhia <amirabendhia@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/15 23:49:35 by htouil            #+#    #+#             */
/*   Updated: 2024/12/30 05:22:27 by amirabendhi      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client() : Fd(-1), IPaddr(""), IfPassCorr(false), Nickname("*"), Username("*"), Realname("*"), Registered(false), Chanmod("")
{
}

int	Client::GetFd() const
{
	return (this->Fd);
}

void	Client::SetFd(int newFd)
{
	this->Fd = newFd;
}

std::string	Client::GetNickname() const
{
	return (this->Nickname);
}

void	Client::SetNickname(std::string newName)
{
	this->Nickname = newName;
}

std::string	Client::GetInBuffer()
{
	return (this->InBuffer);
}

void		Client::SetInBuffer(std::string newBuffer)
{
	this->InBuffer += newBuffer;
}

void	Client::ClearBuffer()
{
	this->InBuffer.clear();
}

std::string	Client::GetUsername()
{
	return (this->Username);
}

void	Client::SetUsername(std::string newName)
{
	this->Username = newName;
}

std::string	Client::GetRealname()
{
	return (this->Realname);
}

void	Client::SetRealname(std::string newName)
{
	this->Realname = newName;
}

bool	Client::GetIfPassCorr()
{
	return (this->IfPassCorr);
}

void	Client::SetPassword(std::string newPass)
{
	this->IfPassCorr = true;
	this->InPass = newPass;
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

std::string	Client::GetChanmod()
{
	return (this->Chanmod);
}

void	Client::SetChanmod(std::string newMode)
{
	this->Chanmod = newMode;
}

std::vector<std::time_t>	&Client::GetCperT()
{
	return (this->CperT);
}
