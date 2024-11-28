/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: htouil <htouil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 00:02:13 by htouil            #+#    #+#             */
/*   Updated: 2024/11/28 23:44:54 by htouil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./Server/Server.hpp"

std::string	remove_crln(std::string msg)
{
	size_t	pos;

	pos = msg.find_first_of("\r\n");
	if (pos != std::string::npos)
		return (msg.substr(0, pos));
	return (msg);
}

std::string	display_current_time()
{
	std::time_t			rn = std::time(0);
	std::tm				*now = std::localtime(&rn);
	std::ostringstream	oss;

	oss << "["
		<< (now->tm_year + 1900) << "/"
		<< std::setw(2) << std::setfill('0') << (now->tm_mon + 1) << "/"
		<< std::setw(2) << std::setfill('0') << (now->tm_mday) << " "
		<< std::setw(2) << std::setfill('0') << (now->tm_hour) << ":"
		<< std::setw(2) << std::setfill('0') << (now->tm_min) << ":" 
		<< std::setw(2) << std::setfill('0') << (now->tm_sec) << "] ";
	return (oss.str());
}

void	send_server_msg(Client &client, std::string err_msg)
{
	std::string	tmp;

	tmp = display_current_time() + err_msg;
	if (send(client.GetFd(), (tmp).c_str(), tmp.size(), 0) == -1)
		std::cerr << "Failed to send to client " << client.GetFd() << std::endl;
}

void	Server::pass(std::pair<std::string, std::vector<std::string> > args, Client &client)
{
	if (client.GetifReg() == true)
		return (send_server_msg(client, ERR_ALREADYREGISTERED(client.GetNickname())));
	if (client.GetPassword() == true)
		return ;
	if (args.second.size() < 1)
		return (send_server_msg(client, ERR_NOTENOUGHPARAMS(client.GetNickname())));
	if (args.second.size() > 1)
		return (send_server_msg(client, ERR_TOOMANYPARAMS(client.GetNickname())));
	if (args.second.front() != this->Spassword)
		return (send_server_msg(client, ERR_PASSWDMISMATCH(client.GetNickname())));
	client.SetPassword(true);
}

void	Server::nick(std::pair<std::string, std::vector<std::string> > args, Client &client)
{
	if (client.GetPassword() == false)
		return (send_server_msg(client, ERR_PASSWDMISMATCH(client.GetNickname())));
	if (args.second.size() < 1)
		return (send_server_msg(client, ERR_NONICKNAMEGIVEN(client.GetNickname())));
	if (args.second.size() > 1)
		return (send_server_msg(client, ERR_TOOMANYPARAMS(client.GetNickname())));
	if (this->find_nickname(args.second.front()) > -1)
		return (send_server_msg(client, ERR_NICKNAMEINUSE(client.GetNickname(), args.second.front())));
	std::string	tmp = args.second.front();
	if (tmp[0] == '#' || tmp[0] == '&' || tmp[0] == ':' || std::isdigit(tmp[0]))
		return (send_server_msg(client, ERR_ERRONEUSNICKNAME(client.GetNickname(), args.second.front())));
	size_t	i;
	for (i = 0; i < tmp.size(); i++)
	{
		if (tmp[i] == ' ' || (!std::isalpha(tmp[i]) && !std::isdigit(tmp[i]) && tmp[i] != '[' && tmp[i] != ']' && tmp[i] != '{' && tmp[i] != '}' && tmp[i] != '\\' && tmp[i] != '|' && (i > 0 && tmp[i] != ':')))
			return (send_server_msg(client, ERR_ERRONEUSNICKNAME(client.GetNickname(), args.second.front())));
	}
	// if (client.GetifReg() == true) //should be put in the nick_update() function
	// 	send_server_msg(client, ":" + client.GetNickname() + " NICK " + args.second.front());
	client.SetNickname(args.second.front());
}

void	Server::user(std::pair<std::string, std::vector<std::string> > args, Client &client)
{
	if (client.GetifReg() == true)
		return (send_server_msg(client, ERR_ALREADYREGISTERED(client.GetNickname())));
	if (client.GetPassword() == false)
		return (send_server_msg(client, ERR_PASSWDMISMATCH(client.GetNickname())));
	if (args.second.size() < 4)
		return (send_server_msg(client, ERR_NOTENOUGHPARAMS(client.GetNickname())));
	if (args.second.size() > 4)
		return (send_server_msg(client, ERR_TOOMANYPARAMS(client.GetNickname())));
	if (args.second[1] != "0" || args.second[2] != "*")
		return (send_server_msg(client, ERR_NOTENOUGHPARAMS(client.GetNickname())));
	if (args.second[0].size() > 10)
		client.SetUsername(args.second[0].substr(0, 10));
	client.SetUsername(args.second[0]);
	if (args.second[3][0] != ':' || !std::isalpha(args.second[3][1]))
		return (send_server_msg(client, ERR_NOTENOUGHPARAMS(client.GetNickname())));
	std::string	tmp = args.second[3];
	size_t	i;
	for (i = 1; i < tmp.size(); i++)
	{
		if (!std::isalpha(tmp[i]) && tmp[i] != ' ') 
			return (send_server_msg(client, ERR_NOTENOUGHPARAMS(client.GetNickname())));
	}
	size_t pos;
	pos = args.second[3].find_last_not_of(" ");
	if (pos != std::string::npos)
		args.second[3].erase(pos + 1);
	client.SetRealname(args.second[3].substr(1));
	// std::cout << "Realname: \'" << client.GetRealname() << "\'" << std::endl;
}

void	Server::quit(std::pair<std::string, std::vector<std::string> > args, Client &client)
{
	if (args.second.size() > 0)
		return (send_server_msg(client, ERR_TOOMANYPARAMS(client.GetNickname())));
	std::cerr << "Client: " << client.GetFd() << " Hung up." << std::endl;
	this->Remove_Client(client.GetFd());
	close(client.GetFd());
}

void	Server::help(Client &client)
{
	std::ostringstream	oss;
	oss << std::endl 
		<< "===========================" << std::endl
		<< "       IRC HELP GUIDE      " << std::endl
		<< "===========================" << std::endl
		<< std::endl << "COMMANDS:" << std::endl
		<< std::endl << "1. " << "PASS:" << std::endl
		<< "- Description: Sets the password for connecting to the server." << std::endl
		<< "-Usage: PASS <password>" << std::endl
		<< "- Note: Must be the first command sent by the client before any other command." << std::endl
		<< std::endl << "2. " << "NICK:" << std::endl
		<< "- Description: Sets or changes your nickname." << std::endl
		<< "- Usage: NICK <nickname>" << std::endl
		<< "- Note: Nicknames must be unique and cannot contain spaces." << std::endl
		<< std::endl << "3. " << "USER:" << std::endl
		<< "- Description: Sets your username and real name." << std::endl
		<< "- Usage: USER <username> 0 * :<realname>" << std::endl
		<< std::endl << "4. " << "QUIT:" << std::endl
		<< "- Description: Disconnects from the server with an optional farewell message." << std::endl
		<< "- Usage: QUIT :<message>" << std::endl
		<< "- Note: If no message is provided, a default quit message will be sent." << std::endl
		<< std::endl << "5. " << "HELP:" << std::endl
		<< "- Description: Displays this help guide." << std::endl
		<< "- Usage: HELP" << std::endl
		<< std::endl 
		<< "===========================" << std::endl
		<< "        END OF GUIDE       " << std::endl
		<< "===========================" << std::endl << std::endl;
	send_server_msg(client, oss.str());
}

void	Server::commands(std::pair<std::string, std::vector<std::string> > args, Client &client)
{
	if (client.GetifReg() == false)
	{
		if (args.first == "PASS" || args.first == "pass")
			pass(args, client);
		else if (args.first == "NICK" || args.first == "nick")
			nick(args, client);
		else if (args.first == "USER" || args.first == "user")
			user(args, client);
		else if (args.first == "QUIT" || args.first == "quit")
			quit(args, client);
		else if (args.first == "HELP" || args.first == "help")
			help(client);
		if (client.GetPassword() == true && client.GetNickname() != "*" && client.GetUsername() != "*" && client.GetRealname() != "*")
		{
			client.SetifReg(true);
			send_server_msg(client, RPL_WELCOME(client.GetNickname(), client.GetUsername(), client.GetIPaddr()));
		}
	}
	else if (client.GetifReg() == true)
	{
		
	}
}
