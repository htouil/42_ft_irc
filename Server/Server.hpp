/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: htouil <htouil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/08 14:54:11 by htouil            #+#    #+#             */
/*   Updated: 2024/11/25 15:58:50 by htouil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <vector>
# include <unistd.h>
# include <csignal>
# include <exception>
# include <cstdlib>
# include <fcntl.h>
# include <poll.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <string>
# include <utility>
# include <sstream>
# include <ctime>
# include <iomanip>
# include "../Client/Client.hpp"

//server messages:
# define ERR_NOTENOUGHPARAMS(nickname) (":Discord Mdere7 461 " + nickname + " :Not enough parameters.\r\n")
# define ERR_TOOMANYPARAMS(nickname) (":Discord Mdere7 1001" + nickname + " :Too many parameters.\r\n")
# define ERR_PASSWDMISMATCH(nickname) (":Discord Mdere7 464 " + nickname + " :Password incorrect.\r\n")
# define ERR_ALREADYREGISTERED(nickname) (":Discord Mdere7 461 " + nickname + " :You may not reregister.\r\n")
# define ERR_NONICKNAMEGIVEN(nickname) ("Discord Mdere7 431 " + nickname + " :No nickname given.\r\n")
# define ERR_NICKNAMEINUSE(oldnick, newnick) ("Discord Mdere7 433 " + oldnick + " " + newnick + " :Nickname already in use.\r\n")
# define ERR_ERRONEUSNICKNAME(oldnick, newnick) ("Discord Mdere7 432 " + oldnick + " " + newnick + " :Erroneus nickname.\r\n")









class Server
{
	private:
		std::string					Name;
		int							Port;
		std::string					Spassword;
		int							SockFd;
		static bool					Signal;
		std::vector<Client>			Clients;
		std::vector<struct pollfd>	Fds;

	public:
					Server(int port, std::string Spassword);
		void		SetPort(int newPort);
		static void	Signal_Handler(int signum);
		void		Disconnect_Everything();
		void		Remove_Client(int rfd);
		void		Server_Initialization(char **av);
		void		Server_Socket_Creation();
		void		Accept_New_Client();
		void		receive_request(int clifd);
		int			find_fd(int to_find);
		int			find_nickname(std::string to_find);
		void		commands(std::pair<std::string, std::vector<std::string> > args, Client &client);
};

std::string	remove_crln(std::string);
std::string	display_current_time();

#endif
