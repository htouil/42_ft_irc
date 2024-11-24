/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: htouil <htouil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 23:46:41 by htouil            #+#    #+#             */
/*   Updated: 2024/11/24 00:59:16 by htouil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <iostream>

class Client
{
	private:
		
		int			Fd;
		std::string	IPaddr;
		bool		Cpassword;
		std::string	Nickname;
		std::string	Username;
		bool		Registered;

	public:
					Client();
		int			GetFd();
		void		SetFd(int newFd);
		std::string	GetNickname();
		void		SetNickname(std::string newName);
		std::string	GetUsername();
		void		SetUsername(std::string newName);
		bool		GetPassword();
		void		SetPassword(bool newPass);
		std::string	GetIPaddr();
		void		SetIPaddr(std::string newIPaddr);
		bool		GetifReg();
		void		SetifReg(bool reg);
};

#endif
