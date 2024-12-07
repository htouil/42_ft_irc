/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: htouil <htouil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/08 14:54:11 by htouil            #+#    #+#             */
/*   Updated: 2024/12/07 15:36:32 by htouil           ###   ########.fr       */
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
# include <algorithm>
# include "../Client/Client.hpp"
# include "../Channel/Channel.hpp"

//server messages:
# define USERLEN 10
# define RPL_WELCOME(nickname, username, IPaddr) (":Discord Mdere7 001 " + nickname + " :Weclome to Discord Mdere7 server " + nickname + "!" + username + "@" + IPaddr + "\r\n")
# define ERR_NOTENOUGHPARAMS(nickname) (":Discord Mdere7 461 " + nickname + " :Not enough parameters.\r\n")
# define ERR_TOOMANYPARAMS(nickname) (":Discord Mdere7 1001" + nickname + " :Too many parameters.\r\n")
# define ERR_PASSWDMISMATCH(nickname) (":Discord Mdere7 464 " + nickname + " :Password incorrect.\r\n")
# define ERR_NOTREGISTERED(nickname) (":Discord Mdere7 451 " + nickname + " :You have not registered.\r\n")
# define ERR_ALREADYREGISTERED(nickname) (":Discord Mdere7 462 " + nickname + " :You may not reregister.\r\n")
# define ERR_NONICKNAMEGIVEN(nickname) ("Discord Mdere7 431 " + nickname + " :No nickname given.\r\n")
# define ERR_NICKNAMEINUSE(oldnick, newnick) ("Discord Mdere7 433 " + oldnick + " " + newnick + " :Nickname already in use.\r\n")
# define ERR_ERRONEUSNICKNAME(oldnick, newnick) ("Discord Mdere7 432 " + oldnick + " " + newnick + " :Erroneus nickname.\r\n")
# define ERR_BANNEDFROMCHAN(nickname, channel) (":Discord Mdere7 474 " + nickname + " " + channel + " :Cannot join channel (+b).\r\n")
# define ERR_INVITEONLYCHAN(nickname, channel) (":Discord Mdere7 473 " + nickname + " " + channel + " :Cannot join channel (+i).\r\n")
# define ERR_BADCHANNELKEY(nickname, channel) (":Discord Mdere7 475 " + nickname + " " + channel + " :Cannot join channel (+k).\r\n")
# define ERR_BADCHANMASK(nickname, channel) (":Discord Mdere7 475 " + nickname + " " + channel + " :Invalid channel name.\r\n")
# define ERR_CHANNELISFULL(nickname, channel) (":Discord Mdere7 471 " + nickname + " " + channel + " :Cannot join channel (+l).\r\n")
# define RPL_TOPIC(nickname, channel, topic) (":Discord Mdere7 332 " + nickname + " " + channel + " :" + topic + "\r\n")
# define RPL_NAMREPLY(nickname, channel) (":Discord Mdere7 353 " + nickname + " = " + channel + " :")

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
		std::vector<Channel>		Channels;

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
		int			find_fd(int to_find, std::vector<Client> list);
		int			find_nickname(std::string to_find, std::vector<Client> list);
		void		pass(std::pair<std::string, std::vector<std::string> > args, Client &client);
		void		nick(std::pair<std::string, std::vector<std::string> > args, Client &client);
		void		user(std::pair<std::string, std::vector<std::string> > args, Client &client);
		void		quit(std::pair<std::string, std::vector<std::string> > args, Client &client);
		void		help(Client &client);
		void		join(std::pair<std::string, std::vector<std::string> > args, Client &client);
		void		commands(std::pair<std::string, std::vector<std::string> > args, Client &client);
};

// std::vector<std::string>	split_input(std::string buffer, char delimiter);
std::vector<std::string>	split_input(std::string buffer, std::string delimiter);
std::string					remove_crln(std::string msg);
std::string					display_current_time();

#endif
