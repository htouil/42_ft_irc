/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: htouil <htouil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 23:46:41 by htouil            #+#    #+#             */
/*   Updated: 2025/01/02 01:57:39 by htouil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <iostream>
# include <vector>
# include <ctime>

class Client
{
	private:
		
		int			Fd;
		std::string	IPaddr;
		std::string	InBuffer;
		bool		IfPassCorr;
		std::string	InPass;
		std::string	Nickname;
		std::string	Username;
		std::string	Realname;
		bool		Registered;
		std::vector<std::time_t>	CperT;

	public:
					Client();
		int			GetFd() const;
		void		SetFd(int newFd);
		std::string	GetNickname() const;
		void		SetNickname(std::string newName);
		std::string	GetInBuffer();
		void		SetInBuffer(std::string newName);
		void		ClearBuffer();
		std::string	GetUsername();
		void		SetUsername(std::string newName);
		std::string	GetRealname();
		void		SetRealname(std::string newName);
		bool		GetIfPassCorr();
		void		SetPassword(std::string newPass);
		std::string	GetIPaddr();
		void		SetIPaddr(std::string newIPaddr);
		bool		GetifReg();
		void		SetifReg(bool reg);
		std::vector<std::time_t>	&GetCperT();
};

#endif
