/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: htouil <htouil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/08 14:54:11 by htouil            #+#    #+#             */
/*   Updated: 2024/12/17 19:25:20 by htouil           ###   ########.fr       */
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
# include <cstring>
# include <utility>
# include <sstream>
# include <ctime>
# include <iomanip>
# include <algorithm>
# include "../Client/Client.hpp"
# include "../Channel/Channel.hpp"

//server messages:
# define USERLEN 10
# define RPL_WELCOME(nickname, username, IPaddr) ("001 " + nickname + " :Weclome to Discord Mdere7 server " + nickname + "!" + username + "@" + IPaddr + "\r\n")
# define ERR_NOTENOUGHPARAMS(nickname) ("461 " + nickname + " :Not enough parameters.\r\n")
# define ERR_TOOMANYPARAMS(nickname) ("1001 " + nickname + " :Too many parameters.\r\n")
# define ERR_PASSWDMISMATCH(nickname) ("464 " + nickname + " :Password incorrect.\r\n")
# define ERR_NOTREGISTERED(nickname) ("451 " + nickname + " :You have not registered.\r\n")
# define ERR_ALREADYREGISTERED(nickname) ("462 " + nickname + " :You may not reregister.\r\n")
# define ERR_NONICKNAMEGIVEN(nickname) ("431 " + nickname + " :No nickname given.\r\n")
# define ERR_NICKNAMEINUSE(oldnick, newnick) ("433 " + oldnick + " " + newnick + " :Nickname already in use.\r\n")
# define ERR_ERRONEUSNICKNAME(oldnick, newnick) ("432 " + oldnick + " " + newnick + " :Erroneus nickname.\r\n")
# define ERR_BANNEDFROMCHAN(nickname, channel) ("474 " + nickname + " " + channel + " :Cannot join channel (+b).\r\n")
# define ERR_INVITEONLYCHAN(nickname, channel) ("473 " + nickname + " " + channel + " :Cannot join channel (+i).\r\n")
# define ERR_BADCHANNELKEY(nickname, channel) ("475 " + nickname + " " + channel + " :Cannot join channel (+k).\r\n")
# define ERR_BADCHANMASK(nickname, channel) ("475 " + nickname + " " + channel + " :Invalid channel name.\r\n")
# define ERR_CHANNELISFULL(nickname, channel) ("471 " + nickname + " " + channel + " :Cannot join channel (+l).\r\n")
# define RPL_TOPIC(nickname, channel, topic) (":ircserv 332 " + nickname + " " + channel + " :" + topic + "\r\n")
# define RPL_TOPICWHOTIME(nickname, channel, user, timestamp) (":ircserv 333 " + nickname + " " + channel + " " + user + " " + timestamp + "\r\n")
# define RPL_NAMREPLY(nickname, channel, member) (":ircserv 353 " + nickname + " = " + channel + " :" + member)
# define RPL_ENDOFNAMES(nickname, channel) (":ircserv 366 " + nickname + " " + channel + " :End of /NAMES list.\r\n")
# define ERR_UNKNOWNERROR(nickname, command) ("400 " + nickname + " " + command + " :Command needs to end with a pair of CR('\\r') and NL('\\n').\r\n")

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
		int			find_nickname(std::string to_find, std::vector<Client> list);
		int			find_fd(int to_find, std::vector<Client> list);
		int			find_IP(std::string to_find, std::vector<Client> list);
		void		pass(std::pair<std::string, std::vector<std::string> > args, Client &client);
		void		nick(std::pair<std::string, std::vector<std::string> > args, Client &client);
		void		user(std::pair<std::string, std::vector<std::string> > args, Client &client);
		void		quit(std::pair<std::string, std::vector<std::string> > args, Client &client);
		void		help(Client &client);
		void		join(std::pair<std::string, std::vector<std::string> > args, Client &client);
		void		commands(std::pair<std::string, std::vector<std::string> > args, Client &client);
		void		send_server_msg(Client &client, std::string err_msg);
};

// std::vector<std::string>	split_input(std::string buffer, char delimiter);
std::vector<std::string>	split_input(std::string buffer, std::string delimiter);
std::string					remove_crln(std::string msg);
std::string					display_current_time();
std::string					get_cli_source(Client cli);

#endif
