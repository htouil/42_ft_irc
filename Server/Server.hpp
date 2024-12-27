/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: htouil <htouil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/08 14:54:11 by htouil            #+#    #+#             */
/*   Updated: 2024/12/27 20:51:39 by htouil           ###   ########.fr       */
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

// Server replies:
# define RPL_WELCOME(nickname, username, IPaddr) (":ircserv 001 " + nickname + " :Weclome to Discord Mdere7 server " + nickname + "!" + username + "@" + IPaddr + "\r\n")
# define ERR_NEEDMOREPARAMS(nickname, command) (":ircserv 461 " + nickname +  " " + command + " :Not enough parameters.\r\n")
# define ERR_TOOMANYPARAMS(nickname, command) (":ircserv 1001 " + nickname +  " " + command + " :Too many parameters.\r\n")
# define ERR_PASSWDMISMATCH(nickname) (":ircserv 464 " + nickname + " :Password incorrect.\r\n")
# define ERR_NOTREGISTERED(nickname) (":ircserv 451 " + nickname + " :You have not registered.\r\n")
# define ERR_ALREADYREGISTERED(nickname) (":ircserv 462 " + nickname + " :You may not reregister.\r\n")
# define ERR_NONICKNAMEGIVEN(nickname) (":ircserv 431 " + nickname + " :No nickname given.\r\n")
# define ERR_NICKNAMEINUSE(oldnick, newnick) (":ircserv 433 " + oldnick + " " + newnick + " :Nickname already in use.\r\n")
# define ERR_ERRONEUSNICKNAME(oldnick, newnick) (":ircserv 432 " + oldnick + " " + newnick + " :Erroneus nickname.\r\n")
# define ERR_INVITEONLYCHAN(nickname, channel) (":ircserv 473 " + nickname + " " + channel + " :Cannot join channel (+i).\r\n")
# define ERR_BADCHANNELKEY(nickname, channel) (":ircserv 475 " + nickname + " " + channel + " :Cannot join channel (+k).\r\n")
# define ERR_BADCHANMASK(nickname, channel) (":ircserv 475 " + nickname + " " + channel + " :Invalid channel name.\r\n")
# define ERR_CHANNELISFULL(nickname, channel) (":ircserv 471 " + nickname + " " + channel + " :Cannot join channel (+l).\r\n")
# define RPL_TOPIC(nickname, channel, topic) (":ircserv 332 " + nickname + " " + channel + " :" + topic + "\r\n")
# define RPL_NOTOPIC(nickname, channel) (":ircserv 331 " + nickname + " " + channel + " :No topic is set\r\n")
# define RPL_NAMREPLY(nickname, channel, member) (":ircserv 353 " + nickname + " = " + channel + " :" + member)
# define RPL_ENDOFNAMES(nickname, channel) (":ircserv 366 " + nickname + " " + channel + " :End of /NAMES list.\r\n")
# define ERR_UNKNOWNERROR(nickname, command) (":ircserv 400 " + nickname + " " + command + " :Command needs to end with a pair of CR('\\r') and NL('\\n').\r\n")
# define ERR_NOSUCHNICK(nickname, command, type) (":ircserv 401 " + nickname + " " + command + " :No such " + type + "\r\n")
# define ERR_NOTONCHANNEL(nickname, channel) (":ircserv 442 " + nickname + " " + channel + " :You're not on that channel\r\n")
# define ERR_NOSUCHCHANNEL(nickname, channel) (":ircserv 403 " + nickname + " " + channel + " :No such channel\r\n")
# define ERR_CHANOPRIVSNEEDED(nickname, channel) (":ircserv 482 " + nickname + " " + channel + " :You're not a channel operator\r\n")
# define ERR_NOTEXTTOSEND(nickname) ("ircserv 412 " + nickname + " :No text to send\r\n")
# define ERR_NORECIPIENT(nickname) ("ircserv 411 " + nickname + " :No recipient given (PRIVMSG)\r\n")




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
		void		privmsg(std::pair<std::string, std::vector<std::string> > args, Client &client);
		void		topic(std::pair<std::string, std::vector<std::string> > args, Client &client);
		void		commands(std::pair<std::string, std::vector<std::string> > args, Client &client);
		// void		mode(std::pair<std::string, std::vector<std::string> > args, Client &client);
		void		send_server_msg(Client &client, std::string err_msg);
		void		send_to_all_in_chan(std::vector<std::pair<Client, std::string> > &Cmbs, std::string msg);
};

std::vector<std::string>								split_input(std::string buffer, std::string delimiter);
std::string												remove_crln(std::string msg);
std::string												display_current_time();
std::string												get_cli_source(Client cli);
std::vector<std::pair<Client, std::string> >::iterator	comparesymbol(std::vector<std::pair<Client, std::string> > &Cmbs, std::string symbol);
std::vector<std::pair<Client, std::string> >::iterator	findclient(std::vector<std::pair<Client, std::string> > &Cmbs, Client target);
std::vector<Channel>::iterator							findchannel(std::vector<Channel> &Chans, std::string target);

#endif
