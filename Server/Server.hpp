/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: htouil <htouil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/08 14:54:11 by htouil            #+#    #+#             */
/*   Updated: 2024/11/28 22:58:25 by htouil           ###   ########.fr       */
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

# define RESET       "\033[0m"
# define RED         "\033[31m"
# define GREEN       "\033[32m"
# define YELLOW      "\033[33m"
# define BLUE        "\033[34m"
# define MAGENTA     "\033[35m"
# define CYAN        "\033[36m"
# define GREY        "\033[90m"
# define WHITE       "\033[37m"
# define GOLD        "\033[38;5;178m"
# define ORANGE      "\033[38;5;202m"
# define PINK        "\033[38;5;206m"
# define PURPLE      "\033[38;5;141m"
# define BROWN       "\033[38;5;94m"
# define BEIGE       "\033[38;5;224m"
# define OLIVE_GREEN "\033[38;5;58m"
# define TEAL        "\033[38;5;30m"
# define AQUA        "\033[38;5;51m"
# define MAROON      "\033[38;5;52m"
# define CRIMSON     "\033[38;5;160m"
# define CORAL       "\033[38;5;203m"
# define SALMON      "\033[38;5;209m"
# define TURQUOISE   "\033[38;5;45m"
# define INDIGO      "\033[38;5;54m"

//server messages:
# define USERLEN 10
# define ERR_NOTENOUGHPARAMS(nickname) (":Discord Mdere7 461 " + nickname + " :Not enough parameters.\r\n")
# define ERR_TOOMANYPARAMS(nickname) (":Discord Mdere7 1001" + nickname + " :Too many parameters.\r\n")
# define ERR_PASSWDMISMATCH(nickname) (":Discord Mdere7 464 " + nickname + " :Password incorrect.\r\n")
# define ERR_ALREADYREGISTERED(nickname) (":Discord Mdere7 461 " + nickname + " :You may not reregister.\r\n")
# define ERR_NONICKNAMEGIVEN(nickname) ("Discord Mdere7 431 " + nickname + " :No nickname given.\r\n")
# define ERR_NICKNAMEINUSE(oldnick, newnick) ("Discord Mdere7 433 " + oldnick + " " + newnick + " :Nickname already in use.\r\n")
# define ERR_ERRONEUSNICKNAME(oldnick, newnick) ("Discord Mdere7 432 " + oldnick + " " + newnick + " :Erroneus nickname.\r\n")
# define RPL_WELCOME(nickname, username, IPaddr) (":Discord Mdere7 001 " + nickname + " :Weclome to Discord Mdere7 server " + nickname + "!" + username + "@" + IPaddr + "\r\n")








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
		void		pass(std::pair<std::string, std::vector<std::string> > args, Client &client);
		void		nick(std::pair<std::string, std::vector<std::string> > args, Client &client);
		void		user(std::pair<std::string, std::vector<std::string> > args, Client &client);
		void		quit(std::pair<std::string, std::vector<std::string> > args, Client &client);
		void		help(Client &client);
		void		commands(std::pair<std::string, std::vector<std::string> > args, Client &client);
};

std::string	remove_crln(std::string);
std::string	display_current_time();

#endif
