/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: htouil <htouil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 23:45:57 by htouil            #+#    #+#             */
/*   Updated: 2024/11/16 23:49:41 by htouil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

bool	Server::Signal = false;

Server::Server(int port, std::string password) : Name("Discord Mdere7"), Port(port), Password(password), SockFd(-1)
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
		std::cout << "Server Disconnected." << std::endl;
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

// void	Accept_New_Client()
// {
	
// }

// void	Recieve_New_Data()
// {
	
// }

void	Server::Server_Initialization(char **av)
{
	size_t	i;

	this->Port = atoi(av[1]);
	Server_Socket_Creation();
	std::cout << "Server connected." << std::endl;
	while (Server::Signal == false)
	{
		if (Server::Signal == false && poll(&this->Fds[0], this->Fds.size(), -1) == -1)
			throw (std::runtime_error("Failed to check for event."));
		for (i = 0; i < this->Fds.size(); i++)
		{
			if (this->Fds[i].revents & POLLIN)
			{
				// if (this->Fds[i].fd == this->SockFd)
				// 	Accept_New_Client();
				// else
				// 	Recieve_New_Data();
			}
		}
	}
	this->Disconnect_Everything();
}

void	Server::Server_Socket_Creation()
{
	struct sockaddr_in	newsockaddr;
	struct pollfd		newPoll;
	int					optval;

	this->SockFd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->SockFd == -1)
		throw (std::runtime_error("Couldn't create a socket for the server."));
	optval = 1;
	if (setsockopt(this->SockFd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
		throw (std::runtime_error("Couldn't set the server socket options."));
	if (fcntl(this->SockFd, F_SETFL, O_NONBLOCK) == -1)
		throw (std::runtime_error("Couldn't set the server socket to non-blocking."));
	newsockaddr.sin_family = AF_INET;
	newsockaddr.sin_port = htons(this->Port);
	newsockaddr.sin_addr.s_addr = INADDR_ANY;
	if (bind(this->SockFd, (struct sockaddr *)&newsockaddr, sizeof(newsockaddr)) == -1)
		throw (std::runtime_error("Couldn't bind the server socket to the port."));
	if (listen(this->SockFd, SOMAXCONN) == -1)
		throw (std::runtime_error("Couldn't switch the server socket to a passive socket."));
	newPoll.fd = this->SockFd;
	newPoll.events = POLL_IN;
	newPoll.revents = 0;
	this->Fds.push_back(newPoll);
}
