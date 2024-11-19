/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: htouil <htouil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 23:46:41 by htouil            #+#    #+#             */
/*   Updated: 2024/11/19 00:43:14 by htouil           ###   ########.fr       */
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
		std::string	NickName;
		std::string	UserName;

	public:
				Client();
		int		GetFd();
		void	SetFd(int newFd);
		void	SetNickName(std::string newName);
		void	SetIPaddr(std::string newIPaddr);
};

#endif
