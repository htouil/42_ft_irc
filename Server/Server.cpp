/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: htouil <htouil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 23:45:57 by htouil            #+#    #+#             */
/*   Updated: 2024/11/25 16:51:50 by htouil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

bool	Server::Signal = false;

Server::Server(int port, std::string Spassword) : Name("Discord Mdere7"), Port(port), Spassword(Spassword), SockFd(-1)
{
}

void	Server::SetPort(int newPort)
{
	this->Port = newPort;
}

void	Server::Signal_Handler(int signum)
{
	(void)signum;
	Server::Signal = true;
}

void	Server::Disconnect_Everything()
{
	size_t	i;

	for (i = 0; i < this->Clients.size(); i++)
	{
		std::cout << "Client " << this->Clients[i].GetFd() << " Disconnected." << std::endl;
		close(this->Clients[i].GetFd());
	}
	if (this->SockFd != -1)
	{
		std::cout << display_current_time() + "Server Disconnected." << std::endl;
		close(this->SockFd);
	}
}

void	Server::Remove_Client(int rfd)
{
	size_t	i;

	for (i = 0; i < this->Fds.size(); i++)
	{
		if (this->Fds[i].fd == rfd)
		{
			this->Fds.erase(this->Fds.begin() + i);
			break ;
		}
	}
	for (i = 0; i < this->Clients.size(); i++)
	{
		if (this->Clients[i].GetFd() == rfd)
		{
			close(this->Clients[i].GetFd());
			this->Clients.erase(this->Clients.begin() + i);
			break ;
		}
	}
}

std::vector<std::string>	split_input(char *buffer, std::string delimiter)
{
		std::string					buf(buffer);
		std::vector<std::string>	cmd;
		size_t						start = 0;
		size_t						end = 0;

		while ((end = buf.find(delimiter, end)) != std::string::npos)
		{
			cmd.push_back(buf.substr(start, end - start));
			start = end + delimiter.length();
		}
		if (start < buf.size()) {
	        cmd.push_back(buf.substr(start));
	    }
	return (cmd);
}

// template<typename T>
// size_t	Server::find_client(T to_find, std::string type)
// {
// 	size_t	i;

// 	for (i = 0; i < this->Clients.size(); i++)
// 	{
// 		if (type == "fd")
// 		{
// 			if (this->Clients[i].GetFd() == (to_find))
// 				return (i);
// 		}
// 		if (type == "nick")
// 		{
// 		if (this->Clients[i].GetNickname() == (to_find))
// 			return (i);
// 		}
// 	}
// 	return (-1);
// }

std::pair<std::string, std::vector<std::string>	>	extract_args(std::string cmd)
{
	std::istringstream									ss(cmd);
	std::string											cmdName;
	std::string											arg;
	std::pair<std::string, std::vector<std::string>	>	args;

	ss >> cmdName;
	args.first = cmdName;
	while (ss >> std::ws)
	{
		// std::cout << "hna: \'" << arg << "\'" << std::endl;
		if (ss.peek() == ':')
		{
			// ss.get();
			if (std::getline(ss, arg))
				args.second.push_back(arg);
			break ;
		}
		if (std::getline(ss, arg, ' '))
			args.second.push_back(arg);
	}
	return (args);
}

void	Server::receive_request(int clifd)
{
	char												tmp[6000];
	size_t												bytes;
	std::vector<std::string>							cmds;
	// size_t												i;
	std::pair<std::string, std::vector<std::string>	>	args;
	int													pos;

	memset(tmp, 0, sizeof(tmp));
	bytes = recv(clifd, tmp, sizeof(tmp) - 1, 0);
	// for (i = 0; i < buffer.size(); i++)
	// 	std::cout << (int)buffer[i] << " , ";
	// std::cout << std::endl << "------" << std::endl;
	std::string	buffer(tmp);
	buffer = remove_crln(buffer);
	// for (i = 0; i < buffer.size(); i++)
	// 	std::cout << (int)buffer[i] << " , ";
	// std::cout << std::endl << "------" << std::endl;
	if (bytes == 0)
	{
		std::cerr << "Client: " << clifd << " Hung up." << std::endl;
		this->Remove_Client(clifd);
		close(clifd);
	}
	else if (bytes > 0)
	{
		if (buffer.empty())
			return ;
		pos = this->find_fd(clifd);
		if (pos != -1 && this->Clients[pos].GetifReg() == false)
		{
			// cmds = split_input(tmp, "\r\n");
			args = extract_args(buffer);
			std::cout << "Command: " << args.first << std::endl;
			for (size_t j = 0; j < args.second.size(); j++)
				std::cout << "Arg " << j + 1 << ": \'" << args.second[j] << "\'"<< std::endl;
			std::cout << "----------------------" << std::endl;
			commands(args, this->Clients[pos]);
		}
	}
}

void	Server::Accept_New_Client()
{
	struct sockaddr_in	newcliaddr;
	struct pollfd		newpoll;
	int					clifd;
	socklen_t			clilen;
	Client				newclient;
	
	
	clilen = sizeof(newcliaddr);
	clifd = accept(this->SockFd, (struct sockaddr *)&newcliaddr, &clilen);
	if (clifd == -1)
	{
		std::cerr << "Failed to accept a new client." << std::endl;
		return ;
	}
	if (fcntl(clifd, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cerr << "Failed to set the client socket to non-blocking." << std::endl;
		return ;
	}
	newclient.SetFd(clifd);
	newclient.SetIPaddr(inet_ntoa(newcliaddr.sin_addr));
	this->Clients.push_back(newclient);
	newpoll.fd = clifd;
	newpoll.events = POLLIN;
	newpoll.revents = 0;
	this->Fds.push_back(newpoll);
	std::cout << "New Client Connected." << std::endl;
}

void	Server::Server_Initialization(char **av)
{
	size_t	i;

	this->Port = atoi(av[1]);
	Server_Socket_Creation();
	std::cout << "Server connected." << std::endl;
	while (1)
	{
		if (Server::Signal == false)
		{
			if (Server::Signal == false && poll(&this->Fds[0], this->Fds.size(), -1) == -1)
				throw (std::runtime_error("Failed to check for event."));
			for (i = 0; i < this->Fds.size(); i++)
			{
				if (this->Fds[i].revents & POLLIN)
				{
					if (i == 0)
						Accept_New_Client();
					else
					{
						// std::cout << "hnaaa: " << Server::Signal << std::endl;
						receive_request(this->Fds[i].fd);
					}
				}
			}
		}
		else
		{
			this->Disconnect_Everything();
			exit(1);
		}	
	}
}

void	Server::Server_Socket_Creation()
{
	struct sockaddr_in	newsockaddr;
	struct pollfd		newpoll;
	int					optval;

	this->SockFd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->SockFd == -1)
		throw (std::runtime_error("Failed to create a socket for the server."));
	optval = 1;
	if (setsockopt(this->SockFd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
		throw (std::runtime_error("Failed to set the server socket options."));
	if (fcntl(this->SockFd, F_SETFL, O_NONBLOCK) == -1)
		throw (std::runtime_error("Failed to set the server socket to non-blocking."));
	newsockaddr.sin_family = AF_INET;
	newsockaddr.sin_port = htons(this->Port);
	newsockaddr.sin_addr.s_addr = INADDR_ANY;
	if (bind(this->SockFd, (struct sockaddr *)&newsockaddr, sizeof(newsockaddr)) == -1)
		throw (std::runtime_error("Failed to bind the server socket to the port."));
	if (listen(this->SockFd, SOMAXCONN) == -1)
		throw (std::runtime_error("Failed to switch the server socket to a passive socket."));
	newpoll.fd = this->SockFd;
	newpoll.events = POLLIN;
	newpoll.revents = 0;
	this->Fds.push_back(newpoll);
}
